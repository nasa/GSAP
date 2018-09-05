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
    
    
    // Tentatively OK for EKF
    void ExtendedKalmanFilter::initialize(const double t0,
                                           const Model::state_type& x0,
                                           const Model::input_type& u0) {
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initializing");
        
        // Initialize time, state, inputs
        lastTime = t0;
        xEstimated = x0;
        uPrev = u0;
        
        // Initialize P
        P = Q;
        
        // Compute corresponding output estimate
        std::vector<double> zeroNoiseZ(model.getOutputSize());
        zEstimated = model.outputEqn(lastTime, xEstimated, uPrev, zeroNoiseZ);
        
        // Set initialized flag
        initialized = true;
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initialize completed");
    }
    
    void ExtendedKalmanFilter::step(const double timestamp,
                                     const Model::input_type& u,
                                     const Model::output_type& z) {
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Starting step");
        Expect(isInitialized(), "Not initialized");
        Expect(timestamp - lastTime > 0, "Time has not advanced");
        
        std::vector<double> zeroNoiseZ(model.getOutputSize());

        
        // Update time
        double dt = timestamp - lastTime;
        lastTime = timestamp;
        
        
        // 1. Predict
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Starting step - predict");
        
        // 1. xkk1 = f(k1k1,uPrev) //calc next state from model assuming no noise
        Model::state_type xkk1 = model.stateEqn(lastTime, xEstimated, uPrev, zeroNoiseZ);
        
        // 2. ykk1 = h(xkk1) //calc next expected sensor readings from expected state
        Model::output_type zkk1 = model.outputEqn(lastTime, xkk1, uPrev, zeroNoiseZ);
        
        // F = jacobian(xkk1,ykk1) //get jacobian eval'd at new expected state, sensor output
        Matrix stateJacobian = model.getStateJacobian(lastTime, xkk1, uPrev, zeroNoiseZ);
        
        // 3. Update Pkk1 using F,Pk1k1,Q
        // H = jacobian(xkk1,u)
        // 4. Update Pyy using H, Pkk1, R=sensor noise covariance
        // 5. Update Pxy
        // 6. Update Kalman Gain Kk
        // 7. Update xkk
        // 8. Update Pkk
        
        
        
        // 2. Update
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Starting step - update");
        
        // Compute state-output cross-covariance matrix
        Matrix Pxz(model.getStateSize(), model.getOutputSize());
        for (unsigned int i = 0; i < sigmaPointCount; i++) {
            Matrix columnx(model.getStateSize(), 1);
            Matrix columnz(model.getOutputSize(), 1);
            columnx.col(0, Xkk1.col(i));
            columnz.col(0, Zkk1.col(i));
            
            Matrix xkk1m(model.getStateSize(), 1);
            Matrix zkk1m(model.getOutputSize(), 1);
            xkk1m.col(0, xkk1);
            zkk1m.col(0, zkk1);
            
            Matrix diffx = columnx - xkk1m;
            Matrix diffzT = (columnz - zkk1m).transpose();
            Matrix temp = diffx * diffzT;
            Pxz = Pxz + temp * sigmaX.w[i];
        }
        
        // Compute Kalman gain
        Matrix Kk = Pxz * Pzz.inverse();
        
        // Compute state estimate
        Matrix xkk1m(model.getStateSize(), 1);
        Matrix zkk1m(model.getOutputSize(), 1);
        Matrix zm(model.getOutputSize(), 1);
        xkk1m.col(0, xkk1);
        zkk1m.col(0, zkk1);
        zm.col(0, z.vec());
        Matrix xk1m = xkk1m + Kk * (zm - zkk1m);
        xEstimated = Model::state_type(static_cast<std::vector<double>>(xk1m.col(0)));
        
        // Compute output estimate
        std::vector<double> zeroNoiseZ(model.getOutputSize());
        zEstimated = model.outputEqn(timestamp, xEstimated, u, zeroNoiseZ);
        
        // Compute covariance
        P = Pkk1 - Kk * Pzz * Kk.transpose();
        
        // Update uOld
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
