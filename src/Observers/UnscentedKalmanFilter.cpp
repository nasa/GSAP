// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "Exceptions.h"
#include "GSAPConfigMap.h"
#include "Observers/UnscentedKalmanFilter.h"
#include "ThreadSafeLog.h"
#include "UData.h"

namespace PCOE {
    const static Log& log = Log::Instance();

    // Configuration Keys
    const std::string Q_KEY = "Observer.Q";
    const std::string R_KEY = "Observer.R";
    const std::string K_KEY = "Observer.kappa";
    const std::string A_KEY = "Obesrver.alpha";
    const std::string B_KEY = "Observer.beta";

    // Other string constants
    const std::string MODULE_NAME = "OBS-UKF";

    UnscentedKalmanFilter::UnscentedKalmanFilter(const Model* m) : Observer(m) {
        Expect(m != nullptr, "Invalid model");
        xEstimated = model->getStateVector();
        uPrev = model->getInputVector();
        zEstimated = model->getOutputVector();

        // Set up sigma point matrices and weights for x
        sigmaX.M.resize(model->getStateSize(), 2 * model->getStateSize() + 1);
        sigmaX.w.resize(2 * model->getStateSize() + 1);

        sigmaX.kappa = 3.0 - model->getStateSize();
        sigmaX.alpha = 1;
        sigmaX.beta = 0;
    }

    UnscentedKalmanFilter::UnscentedKalmanFilter(const Model* m, Matrix q, Matrix r)
        : UnscentedKalmanFilter(m) {
        Expect(q.rows() == q.cols(), "q is not square");
        Expect(q.rows() == model->getStateSize(), "Size of q does not match model state size");
        Expect(r.rows() == r.cols(), "q is not square");
        Expect(r.rows() == model->getOutputSize(), "Size of r does not match model output size");

        Q = std::move(q);
        R = std::move(r);
    }

    // GSAPConfigMap-based Constructor
    UnscentedKalmanFilter::UnscentedKalmanFilter(const Model* model, const GSAPConfigMap& config)
        : UnscentedKalmanFilter(model) {
        config.checkRequiredParams({Q_KEY, R_KEY});

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

        // Set kappa (optional)
        if (config.hasKey(K_KEY)) {
            setKappa(config.getDouble(K_KEY));
        }
        // Set alpha (optional)
        if (config.hasKey(K_KEY)) {
            setAlpha(config.getDouble(A_KEY));
        }
        // Set beta (optional)
        if (config.hasKey(K_KEY)) {
            setBeta(config.getDouble(B_KEY));
        }

        log.WriteLine(LOG_INFO, MODULE_NAME, "Created UKF");
    }

    void UnscentedKalmanFilter::initialize(const double t0,
                                           const Model::state_type& x0,
                                           const Model::input_type& u0) {
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initializing");

        // Initialize time, state, inputs
        lastTime = t0;
        xEstimated = x0;
        uPrev = u0;

        // Initialize P
        P = Q;

        // Compute sigma points for current state estimate
        computeSigmaPoints(xEstimated, P, sigmaX);

        // Compute corresponding output estimate
        std::vector<double> zeroNoiseZ(model->getOutputSize());
        zEstimated = model->outputEqn(lastTime, xEstimated, uPrev, zeroNoiseZ);

        // Set initialized flag
        initialized = true;
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initialize completed");
    }

    void UnscentedKalmanFilter::step(const double timestamp,
                                     const Model::input_type& u,
                                     const Model::output_type& z) {
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Starting step");
        Expect(isInitialized(), "Not initialized");
        Expect(timestamp - lastTime > 0, "Time has not advanced");

        // Update time
        double dt = timestamp - lastTime;
        lastTime = timestamp;

        std::size_t sigmaPointCount = sigmaX.M.cols();

        // 1. Predict
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Starting step - predict");

        // Compute sigma points for current state estimate
        computeSigmaPoints(xEstimated, Q, sigmaX);

        // Propagate sigma points through state equation
        Matrix Xkk1(model->getStateSize(), sigmaPointCount);
        std::vector<double> zeroNoise(model->getStateSize());
        for (unsigned int i = 0; i < sigmaPointCount; i++) {
            auto x = Model::state_type(static_cast<std::vector<double>>(sigmaX.M.col(i)));
            x = model->stateEqn(timestamp, x, uPrev, zeroNoise, dt);
            Xkk1.col(i, x.vec());
        }

        // Recombine weighted sigma points to produce predicted state and covariance
        std::vector<double> xkk1 =
            static_cast<std::vector<double>>(Xkk1.weightedMean(Matrix(sigmaX.w)));
        Matrix Pkk1 = Xkk1.weightedCovariance(Matrix(sigmaX.w), sigmaX.alpha, sigmaX.beta) + Q;

        // Propagate sigma points through output equation
        Matrix Zkk1(model->getOutputSize(), sigmaPointCount);
        for (unsigned int i = 0; i < sigmaPointCount; i++) {
            auto zkk1 =
                model->outputEqn(timestamp,
                                 Model::state_type(static_cast<std::vector<double>>(Xkk1.col(i))),
                                 u,
                                 zeroNoise);
            Zkk1.col(i, zkk1.vec());
        }

        // Recombine weighted sigma points to produce predicted measurement and covariance
        std::vector<double> zkk1 =
            static_cast<std::vector<double>>(Zkk1.weightedMean(Matrix(sigmaX.w)));
        Matrix Pzz = Zkk1.weightedCovariance(Matrix(sigmaX.w), sigmaX.alpha, sigmaX.beta) + R;

        // 2. Update
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Starting step - update");

        // Compute state-output cross-covariance matrix
        Matrix Pxz(model->getStateSize(), model->getOutputSize());
        for (unsigned int i = 0; i < sigmaPointCount; i++) {
            Matrix columnx(model->getStateSize(), 1);
            Matrix columnz(model->getOutputSize(), 1);
            columnx.col(0, Xkk1.col(i));
            columnz.col(0, Zkk1.col(i));

            Matrix xkk1m(model->getStateSize(), 1);
            Matrix zkk1m(model->getOutputSize(), 1);
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
        Matrix xkk1m(model->getStateSize(), 1);
        Matrix zkk1m(model->getOutputSize(), 1);
        Matrix zm(model->getOutputSize(), 1);
        xkk1m.col(0, xkk1);
        zkk1m.col(0, zkk1);
        zm.col(0, z.vec());
        Matrix xk1m = xkk1m + Kk * (zm - zkk1m);
        xEstimated = Model::state_type(static_cast<std::vector<double>>(xk1m.col(0)));

        // Compute output estimate
        std::vector<double> zeroNoiseZ(model->getOutputSize());
        zEstimated = model->outputEqn(timestamp, xEstimated, u, zeroNoiseZ);

        // Compute covariance
        P = Pkk1 - Kk * Pzz * Kk.transpose();

        // Update uOld
        uPrev = u;
    }

    void UnscentedKalmanFilter::computeSigmaPoints(const Model::state_type& mx,
                                                   const Matrix& Pxx,
                                                   SigmaPoints& sigma) {
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Computing sigma points");

        // Assumes that sigma points have been set up correctly within the constructor
        auto stateSize = mx.size();
        auto sigmaPointCount = sigma.M.cols();

        // First sigma point is the mean
        for (unsigned int i = 0; i < stateSize; i++) {
            sigma.M[i][0] = mx[i];
        }

        // Compute a matrix square root using Cholesky decomposition
        Matrix nkPxx = Pxx;
        for (unsigned int i = 0; i < nkPxx.rows(); i++) {
            for (unsigned int j = 0; j < nkPxx.cols(); j++) {
                nkPxx[i][j] *= (stateSize + sigma.kappa);
            }
        }
        Matrix matrixSq = nkPxx.chol();

        // For sigma points 2 to n+1, it is mx + ith column of matrix square root
        for (unsigned int i = 0; i < stateSize; i++) {
            // Set column
            for (unsigned int j = 0; j < stateSize; j++) {
                sigma.M[i][j + 1] = mx[i] + matrixSq[i][j];
            }
        }

        // For sigma points n+2 to 2n+1, it is mx - ith column of matrix square root
        for (unsigned int i = 0; i < stateSize; i++) {
            // Set column
            for (unsigned int j = 0; j < stateSize; j++) {
                sigma.M[i][j + stateSize + 1] = mx[i] - matrixSq[i][j];
            }
        }

        // w(1) is kappa/(n+kappa)
        sigma.w[0] = sigma.kappa / (stateSize + sigma.kappa);

        // Rest of w are 0.5/(n+kappa)
        for (unsigned int i = 1; i < sigmaPointCount; i++) {
            sigma.w[i] = 0.5 / (stateSize + sigma.kappa);
        }

        // Scale the sigma points
        // 1. Xi' = X0 + alpha*(Xi-X0)
        Matrix X0 = sigma.M.col(0);
        for (unsigned int i = 1; i < sigmaPointCount; i++) {
            sigma.M.col(i, X0 + sigma.alpha * (sigma.M.col(i) - X0));
        }

        // 2. W0' = W0/alpha^2 + (1/alpha^2-1)
        //    Wi' = Wi/alpha^2
        sigma.w[0] = sigma.w[0] / sigma.alpha / sigma.alpha + (1 / sigma.alpha / sigma.alpha - 1);
        for (unsigned int i = 1; i < sigmaPointCount; i++) {
            sigma.w[i] = sigma.w[i] / sigma.alpha / sigma.alpha;
        }
    }

    std::vector<UData> UnscentedKalmanFilter::getStateEstimate() const {
        std::vector<UData> state(model->getStateSize());
        for (unsigned int i = 0; i < model->getStateSize(); i++) {
            state[i].uncertainty(UType::MeanCovar);
            state[i].npoints(model->getStateSize());
            state[i][MEAN] = xEstimated[i];
            state[i][COVAR()] = static_cast<std::vector<double>>(P.row(i));
        }
        return state;
    }
}
