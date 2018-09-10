/**  EmptyPredictor - Header
 *   @file      EmptyPredictor Class
 *   @ingroup   GPIC++
 *   @ingroup    Predictors
 *
 *   @brief     EmptyPredictor Class - Class defining the EmptyPredictor
 *
 *   @author    Matthew Daigle
 *   @version   1.1.0
 *
 *   @pre       N/A
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: January 10, 2017
 *
 *   @copyright Copyright (c) 2017-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_EmptyPredictor_H
#define PCOE_EmptyPredictor_H

#include <string>
#include <vector>

#include "ConfigMap.h"
#include "Models/Model.h"
#include "Predictors/Predictor.h"

namespace PCOE {
    class EmptyPredictor final : public Predictor {
    public:
        /**
         * Initializes a new @{code EmptyPredictor}.
         *
         * @param m      The model used by the predictor.
         * @param le     The load estimator used by the predictor.
         * @param ts     The trajectory service to be used by the predictor
         * @param config Configuration map specifying predictor parameters.
         **/
        explicit EmptyPredictor(const PrognosticsModel& m,
                                LoadEstimator& le,
                                TrajectoryService& ts,
                                ConfigMap& config);

        /**
         * Predict future events and values of system variables
         *
         * @param t     Time of prediction
         * @param state State of system at time of prediction
         **/
        Prediction predict(double t, const std::vector<UData>& state) override;
    };
}

#endif // PCOE_EmptyPredictor_H
