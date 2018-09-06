// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#include <stdio.h>

//#include "ExtendedKalmanFilter.h"


/**
 Functions are still largely copied from UKF, header file being designed first and waiting on decision for how to deal with Jacobians.
**/

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "Exceptions.h"
#include "ConfigMap.h"
#include "Observers/ExtendedKalmanFilter.h"
#include "ThreadSafeLog.h"
#include "UData.h"

namespace PCOE {
    const static Log& log = Log::Instance();
    
    // Configuration Keys
    const std::string Q_KEY = "Observer.Q";
    const std::string R_KEY = "Observer.R";

    // Other string constants
    const std::string MODULE_NAME = "OBS-EKF";
    
    
    ExtendedKalmanFilter::ExtendedKalmanFilter(const Model& m) : Observer(m) {
        xEstimated = model.getStateVector();
        uPrev = model.getInputVector();
        zEstimated = model.getOutputVector();
    }
    
    ExtendedKalmanFilter::ExtendedKalmanFilter(const Model& m, Matrix q, Matrix r)
    : ExtendedKalmanFilter(m) {
        Expect(q.rows() == q.cols(), "q is not square");
        Expect(q.rows() == model.getStateSize(), "Size of q does not match model state size");
        Expect(r.rows() == r.cols(), "q is not square");
        Expect(r.rows() == model.getOutputSize(), "Size of r does not match model output size");
        
        Q = std::move(q);
        R = std::move(r);
    }
    
    // ConfigMap-based Constructor
    ExtendedKalmanFilter::ExtendedKalmanFilter(const Model& model, const ConfigMap& config)
    : ExtendedKalmanFilter(model) {
        requireKeys(config, {Q_KEY, R_KEY});
        
        // Set Q
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Setting Q");
        auto& QValues = config.getVector(Q_KEY);
        Require(std::abs(std::sqrt(QValues.size()) - std::floor(std::sqrt(QValues.size()))) < 1e-12,
                "Q values can not describe a square matrix");
        auto qDim = static_cast<std::size_t>(sqrt(QValues.size()));
        Q.resize(qDim, qDim);
        std::size_t qValueIndex = 0;
        for (std::size_t row = 0; row < qDim; row++) {
            for (std::size_t col = 0; col < qDim; col++) {
                Q[row][col] = std::stod(QValues[qValueIndex]);
                qValueIndex++;
            }
        }
        
        // Set R
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Setting R");
        auto& RValues = config.getVector(R_KEY);
        Require(std::abs(std::sqrt(RValues.size()) - std::floor(std::sqrt(RValues.size()))) < 1e-12,
                "R values can not describe a square matrix");
        auto rDim = static_cast<std::size_t>(sqrt(RValues.size()));
        R.resize(rDim, rDim);
        std::size_t rValueIndex = 0;
        for (std::size_t row = 0; row < rDim; row++) {
            for (std::size_t col = 0; col < rDim; col++) {
                R[row][col] = std::stod(RValues[rValueIndex]);
                rValueIndex++;
            }
        }
        
        log.WriteLine(LOG_INFO, MODULE_NAME, "Created EKF");
    }
    
    
    // Initialize the EKF
    void ExtendedKalmanFilter::initialize(const double t0,
                                           const Model::state_type& x0,
                                           const Model::input_type& u0) {
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initializing");
        
        // Initialize time, state, inputs
        lastTime = t0;
        xEstimated = x0;
        uPrev = u0;
        
        // Initialize P (state covariance matrix) as Q (process noise covariance matrix)
        P = Q;
        
        // Compute corresponding output estimate
        std::vector<double> zeroOutputNoise(model.getOutputSize());
        zEstimated = model.outputEqn(lastTime, xEstimated, uPrev, zeroOutputNoise);
        
        // Set initialized flag
        initialized = true;
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initialize completed");
    }
    
    void ExtendedKalmanFilter::step(const double timestamp,
                                     const Model::input_type& u,
                                     Model::output_type& z) {
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Starting step");
        Expect(isInitialized(), "Not initialized");
        Expect(timestamp - lastTime > 0, "Time has not advanced");
        
        std::vector<double> zeroProcessNoise(model.getStateSize());
        std::vector<double> zeroOutputNoise(model.getOutputSize());
        
        // Update time
        double dt = timestamp - lastTime;
        lastTime = timestamp;
        
        
        //*/*/* Prediction Step */*/*//
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Starting step - predict");
        
        // 1. x_k|k-1 = f(k1k1,uPrev) //calc next state from model assuming no noise
        Model::state_type xkk1 = model.stateEqn(timestamp, xEstimated, uPrev, zeroProcessNoise);
        
        // 2. z_k|k-1 = h(xkk1) //calc next expected sensor readings from expected state
        Model::output_type zkk1 = model.outputEqn(timestamp, xkk1, uPrev, zeroOutputNoise);
        
        //*/*/* Update Step */*/*//
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Starting step - update");
        
        // F = stateJacobian(xkk1,uPrev) //get jacobian eval'd at new expected state, sensor output
        Matrix stateJacobian = model.getStateJacobian(timestamp, xkk1, u, zeroProcessNoise, dt);
        // H = outputJacobian(xkk1,u)
        Matrix outputJacobian = model.getOutputJacobian(timestamp, xkk1, u, zeroOutputNoise);
        
        // 3. Update P_k|k-1 using F,P_k-1|k-1,Q
        Matrix Pkk1 = stateJacobian * P * stateJacobian.transpose() + Q;
        
        // 4. Update Pzz using H, Pkk1, R=sensor noise covariance
        Matrix Pzz = outputJacobian * Pkk1 * outputJacobian.transpose() + R;
        
        // 5. Update Pxz
        Matrix Pxz = Pkk1 * outputJacobian.transpose();
        
        // 6. Compute Kalman Gain Kk
        Matrix Kk = Pxz * Pzz.inverse();
        
        // 7. Update state estimate
        Matrix xkk = Matrix(xkk1) + (Kk * (Matrix(z) - Matrix(zkk1)));
        xEstimated = Model::state_type(static_cast<std::vector<double>>(xkk.col(0)));
        
        // 8. Update Pkk = (I-Kk*Hk)Pkk1
        P = (Matrix::identity(model.getStateSize()) - (Kk * outputJacobian)) * Pkk1;
        
        // Compute output estimate
        zEstimated = model.outputEqn(timestamp, xEstimated, u, zeroOutputNoise);
        
        //Update uPrev
        uPrev = u;

    }
    
    
    std::vector<UData> ExtendedKalmanFilter::getStateEstimate() const {
        std::vector<UData> state(model.getStateSize());
        for (unsigned int i = 0; i < model.getStateSize(); i++) {
            state[i].uncertainty(UType::MeanCovar);
            state[i].npoints(model.getStateSize());
            state[i][MEAN] = xEstimated[i];
            state[i][COVAR()] = static_cast<std::vector<double>>(P.row(i));
        }
        return state;
    }
}
