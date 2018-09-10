/**  EmptyObserver - Header
 *   @file       EmptyObserver.h
 *   @ingroup    GPIC++
 *   @defgroup   Observer     Observer
 *
 *   @brief      EmptyObserver class
 *
 *   @author     Matthew Daigle
 *   @version    1.1.0
 *
 *   @pre        N/A
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: January 10, 2017
 *
 *   @copyright Copyright (c) 2017-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_EmptyObserver_H
#define PCOE_EmptyObserver_H

#include "Observers/Observer.h"
#include <cmath>
#include <vector>

namespace PCOE {
    class EmptyObserver final : public Observer {
    public:
        /**
         * Constructs a new @{code EmptyObserver} instance with the
         * given model and covariance matrices.
         *
         * @param m A valid pointer to a model on which state estimation will be
         *          performed. The EmptyObserver does not take ownership of the model.
         * @param Q Process noise covariance matrix
         * @param R Sensor noise covariance matrix
         **/
        EmptyObserver(const Model& m, const Matrix Q, const Matrix R);

        /** @brief Initialize UKF
         *   @param t0 Initial time
         *   @param x0 Initial state vector
         *   @param u0 Initial input vector
         **/
        void initialize(const double t0,
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

        std::vector<UData> getStateEstimate() const override;
    };
}

#endif // PCOE_EmptyObserver_H
