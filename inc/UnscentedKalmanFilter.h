/**  UnscentedKalmanFilter - Header
 *   @file       UnscentedKalmanFilter.h
 *   @ingroup    GPIC++
 *   @defgroup   Observer     Observer
 *
 *   @brief      Unscented Kalman filter class. Implements UKF state estimation algorithm for
 * nonlineary models. Uses the Model class.
 *
 *   @author     Matthew Daigle
 *   @version    0.1.0
 *
 *   @pre        N/A
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: March 5, 2016
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_UNSCENTEDKALMANFILTER_H
#define PCOE_UNSCENTEDKALMANFILTER_H

#include <cmath>
#include <vector>

#include "Matrix.h"
#include "Observer.h"

namespace PCOE {
    // Including class prototype to avoid including header
    class GSAPConfigMap;

    struct sigmaPoints {
        Matrix M; // data matrix
        std::vector<double> w; // weights
        double kappa = NAN; // tuning parameter
        double alpha = NAN; // scaling parameter
        double beta = NAN; // scaling parameter
    };

    class UnscentedKalmanFilter final : public Observer {
    private:
        Model::state_type m_xEstimated;
        Model::output_type m_zEstimated;
        Matrix m_Q;
        Matrix m_R;
        Matrix m_P;
        struct sigmaPoints m_sigmaX;

    public:
        /** @brief Constructor
         *   @param m Model pointer
         *   @param Q Process noise covariance matrix
         *   @param R Sensor noise covariance matrix
         **/
        UnscentedKalmanFilter(Model* m, const Matrix Q, const Matrix R);

        /** @brief Constructor given a ConfigMap
         *   @param configMap configuration map specifying parameters (Q, R)
         **/
        explicit UnscentedKalmanFilter(GSAPConfigMap& configMap);

        /** @brief Set model pointer
         *   @param model given model pointer
         **/
        void setModel(Model* model);

        /** @brief Set kappa parameter
         *   @param kappa value of kappa parameter, overrides default
         **/
        void setKappa(double kappa);

        /** @brief Set alpha parameter
         *   @param alpha value of alpha parameter, overrides default
         **/
        void setAlpha(double alpha);

        /** @brief Set beta parameter
         *   @param beta value of beta parameter, overrides default
         **/
        void setBeta(double beta);

        /** @brief Initialize UKF
         *   @param t0 Initial time
         *   @param x0 Initial state vector
         *   @param u0 Initial input vector
         **/
        void initialize(const double t0, const Model::state_type& x0, const Model::input_type& u0);

        /** @brief Estimation step. Updates xEstimated, zEsitmated, P, and sigmaX.
         *   @param newT Time value at new step
         *   @param u Input vector at current time
         *   @param z Output vector at current time
         **/
        void step(const double newT, const Model::input_type& u, const Model::output_type& z);

        /** @brief Compute sigma points given mean vector and covariance matrix.
         *          Implements symmetric unscented transform.
         *   @param mx Mean vector
         *   @param Pxx Covariance matrix
         *   @param kappa Tuning parameter
         *   @param alpha Scaling parameter
         *   @param X Sigma point data matrix
         *   @param w Sigma point weight vector
         **/
        void computeSigmaPoints(const Model::state_type& mx,
                                const Matrix& Pxx,
                                const double kappa,
                                const double alpha,
                                Matrix& X,
                                std::vector<double>& w);

        /** @brief Print some variables
         **/
        void print() const;

        // Accessors
        const Model::state_type& getStateMean() const override;
        const Model::output_type& getOutputMean() const override;
        const Matrix& getStateCovariance() const;
        std::vector<UData> getStateEstimate() const override;
    };
}

#endif // PCOE_UNSCENTEDKALMANFILTER_H
