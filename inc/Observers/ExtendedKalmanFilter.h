// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#ifndef PCOE_EXTENDEDKALMANFILTER_H
#define PCOE_EXTENDEDKALMANFILTER_H


#endif


#include <cmath>
#include <vector>

#include "Matrix.h"
#include "Observers/Observer.h"

namespace PCOE {
    class ConfigMap;
    
    /**
     * Implements EKF state estimation algorithm for non-linear models.
     *
     **
     *** Denotes a note to myself that must be dealt with
     **
     *
     * @author Molly O'Connor
     * Cannibalized from UnscentedKalmanFilter.h by
     * @author Matthew Daigle
     * @author Jason Watkins
     **/
    class ExtendedKalmanFilter final : public Observer {
    private:
        /**
         * Constructs a new @{code ExtendedKalmanFilter} instance and
         * initializes the model. This constructor is only intended to be used
         * by other constructors in EKF to set up model-related parameters.
         *
         * @param m A valid pointer to a model on which state estimation will be
         *          performed. The EKF does not take ownership of the model.
         **/
        explicit ExtendedKalmanFilter(const Model& m);
        
    public:
        /**
         * Constructs a new @{code ExtendedKalmanFilter} instance with the
         * given model and covariance matrices. Checks that noise covariance 
         * matrices are square matrices with the same dimensions as model state 
         * and output vectors.
         *
         * @param m A valid pointer to a model on which state estimation will be
         *          performed. The EKF does not take ownership of the model.
         * @param Q Process noise covariance matrix
         * @param R Sensor noise covariance matrix
         **/
        ExtendedKalmanFilter(const Model& m, const Matrix Q, const Matrix R);
        
        /**
         * Constructs a new @{code ExtendedKalmanFilter} instance with the
         * given model and with covariance matrices read from the provided
         * config.
         *
         * @param m      A valid pointer to a model on which state estimation
         *               will be performed. The EKF does not take ownership of
         *               the model.
         * @param config A configuration from which to read covariance matrices.
         **/
        ExtendedKalmanFilter(const Model& m, const ConfigMap& config);
        
        /**
         * Sets the initial model state.
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
         * @param t The time at which to make a prediction. Must have advanced from previous time.
         * @param u The model input vector at time @{code t}.
         * @param z The model output vector at time @{code t}.
         **/
        void step(double t, const Model::input_type& u, Model::output_type& z) override;
        
        /**
         *
         * Returns the current state estimate of the observer, including
         * uncertainty.
         *
         * @return The last calculated state estimate calcualted by the
         *         observer.
         **/
        std::vector<UData> getStateEstimate() const override;
        
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
    };
}

#endif
