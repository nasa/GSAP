// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_UNSCENTEDKALMANFILTER_H
#define PCOE_UNSCENTEDKALMANFILTER_H

#include <cmath>
#include <vector>

#include "Matrix.h"
#include "Observers/Observer.h"

namespace PCOE {
    class ConfigMap;

    struct SigmaPoints {
        Matrix M; // data matrix
        std::vector<double> w; // weights
        double kappa; // tuning parameter
        double alpha; // scaling parameter
        double beta; // scaling parameter
    };

    /**
     * Implements UKF state estimation algorithm for non-linear models.
     *
     * @author Mathew Daigle
     * @author Jason Watkins
     **/
    class UnscentedKalmanFilter final : public Observer {
    private:
        /**
         * Constructs a new @{code UnscentedKalmanFilter} instance and
         * initializes the model. This constructor is only intended to be used
         * by other constructors in UKF to set up model-related parameters.
         *
         * @param m A valid pointer to a model on which state estimation will be
         *          performed. The UKF does not take ownership of the model.
         **/
        explicit UnscentedKalmanFilter(const Model& m);

    public:
        /**
         * Constructs a new @{code UnscentedKalmanFilter} instance with the
         * given model and covariance matrices.
         *
         * @param m A valid pointer to a model on which state estimation will be
         *          performed. The UKF does not take ownership of the model.
         * @param Q Process noise covariance matrix
         * @param R Sensor noise covariance matrix
         **/
        UnscentedKalmanFilter(const Model& m, const Matrix Q, const Matrix R);

        /**
         * Constructs a new @{code UnscentedKalmanFilter} instance with the
         * given model and with covariance matrices read from the provided
         * config.
         *
         * @param m      A valid pointer to a model on which state estimation
         *               will be performed. The UKF does not take ownership of
         *               the model.
         * @param config A configuration from which to read covariance matrices.
         **/
        UnscentedKalmanFilter(const Model& m, const ConfigMap& config);

        /**
         * Sets the initial model state and computes initial sigma points.
         *
         * @param t0 Initial time
         * @param x0 Initial model state
         * @param u0 Initial model input
         **/
        void initialize(double t0,
                        const Model::state_type& x0,
                        const Model::input_type& u0) override;

        /**
         * Performs a single state estimation with the given model inputs and
         * outputs.
         *
         * @param t The time at which to make a prediction.
         * @param u The model input vector at time @{code t}.
         * @param z The model output vector at time @{code t}.
         **/
        void step(double t, const Model::input_type& u, const Model::output_type& z) override;

        /**
         * Compute sigma points given mean vector and covariance matrix.
         * Implements symmetric unscented transform.
         *
         * @param mx    Mean vector
         * @param Pxx   Covariance matrix
         * @param sigma Sigma points
         **/
        void computeSigmaPoints(const Model::state_type& mx, const Matrix& Pxx, SigmaPoints& sigma);

        /**
         * Sets the kappa tuning parameter.
         **/
        inline void setKappa(double value) {
            sigmaX.kappa = value;
        }

        /**
         * Sets the alpha scaling parameter.
         **/
        void setAlpha(double value) {
            sigmaX.alpha = value;
        }

        /**
         * Sets the beta scaling parameter.
         **/
        void setBeta(double value) {
            sigmaX.beta = value;
        }

        /**
         * Returns the current state estimate of the observer, including
         * uncertainty.
         *
         * @return The last calculated state estimate calcualted by the
         *         observer.
         **/
        std::vector<UData> getStateEstimate() const;

        /**
         * Gets the state covariance matrix.
         **/
        const Matrix& getStateCovariance() const {
            return P;
        }

    private:
        Model::state_type xEstimated;
        Model::output_type zEstimated;
        Matrix Q;
        Matrix R;
        Matrix P;
        struct SigmaPoints sigmaX;
    };
}

#endif
