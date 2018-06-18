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

#include <vector>
#include <cmath>
#include "Observer.h"

namespace PCOE {
    class EmptyObserver final : public Observer {

    public:
        /** @brief Set model pointer
        *   @param model given model pointer
        **/
        void setModel(Model *model);

        /** @brief Initialize UKF
        *   @param t0 Initial time
        *   @param x0 Initial state vector
        *   @param u0 Initial input vector
        **/
        void initialize(const double t0, const std::vector<double> & x0,
            const std::vector<double> & u0);

        /** @brief Estimation step. Updates xEstimated, zEsitmated, P, and sigmaX.
        *   @param newT Time value at new step
        *   @param u Input vector at current time
        *   @param z Output vector at current time
        **/
        void step(const double newT, const std::vector<double> & u,
            const std::vector<double> & z);

        // Accessors
        const std::vector<double> & getStateMean() const;
        const std::vector<double> & getOutputMean() const;
        std::vector<UData> getStateEstimate() const;
    };
}

#endif // PCOE_EmptyObserver_H
