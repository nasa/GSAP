/**  MonteCarloPredictor - Header
 *   @file      MonteCarloPredictor Class
 *   @ingroup   GPIC++
 *   @ingroup    Predictors
 *
 *   @brief     MonteCarloPredictor Class - Class defining the MonteCarloPredictor
 *
 *   @author    Matthew Daigle
 *   @version   1.1.0
 *
 *   @pre       N/A
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: March 22, 2016
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_MONTECARLOPREDICTOR_H
#define PCOE_MONTECARLOPREDICTOR_H

#include <string>
#include <vector>

#include "GSAPConfigMap.h"
#include "Model.h"
#include "Predictor.h"

namespace PCOE {
    class MonteCarloPredictor final : public Predictor {
    private:
        unsigned int numSamples; // number of samples used in prediction
        std::vector<double> processNoise; // variance vector (zero-mean assumed)

    public:
        /** @brief    Constructor for a MonteCarloPredictor based on a configMap
         *  @param  configMap Configuration map specifying predictor parameters
         **/
        explicit MonteCarloPredictor(const PrognosticsModel* m,
                                     LoadEstimator* le,
                                     GSAPConfigMap& configMap);

        /** @brief Set model pointer
         *  @param value given model pointer
         **/
        void setModel(PrognosticsModel* value);

        /** @brief    Predict function for a Predictor
         *  @param    tP Time of prediction
         *  @param    state state of system at time of prediction
         *  @param  data ProgData object, in which prediction results \re stored
         **/
        Prediction predict(const double tP, const std::vector<UData>& state, ProgData& data);
    };
}
#endif // PCOE_MONTECARLOPREDICTOR_H
