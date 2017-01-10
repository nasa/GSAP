/**  EmptyPredictor - Header
 *   @file      EmptyPredictor Class
 *   @ingroup   GPIC++
 *   @ingroup    Predictors
 *
 *   @brief     EmptyPredictor Class - Class defining the EmptyPredictor
 *
 *   @author    Matthew Daigle
 *   @version   0.1.0
 *
 *   @pre       N/A
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: January 10, 2017
 *
 *   @copyright Copyright (c) 2017 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_EmptyPredictor_H
#define PCOE_EmptyPredictor_H

#include <vector>
#include <string>

#include "Model.h"
#include "Predictor.h"
#include "GSAPConfigMap.h"

namespace PCOE {
    class EmptyPredictor final : public Predictor {

    public:
        /** @brief    Constructor for a EmptyPredictor based on a configMap
        *   @param  configMap Configuration map specifying predictor parameters
        **/
        explicit EmptyPredictor(GSAPConfigMap & configMap);
        /** @brief Set model pointer
        *   @param model given model pointer
        **/
        void setModel(PrognosticsModel * model);

        /** @brief    Predict function for a Predictor
        *   @param    tP Time of prediction
        *    @param    state state of system at time of prediction
        *   @param  data ProgData object, in which prediction results are stored
        **/
        void predict(const double tP, const std::vector<UData> & state, ProgData & data);
    };
}

#endif  // PCOE_EmptyPredictor_H
