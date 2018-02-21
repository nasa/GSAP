/**  Predictor - Header
 *   @file      Predictor Interface Class
 *   @ingroup   GPIC++
 *   @defgroup  Predictor     Predictor
 *
 *   @brief     Predictor Interface Class - Class defining the Predictor interface (model-based predictor)
 *
 *   @author    Matthew Daigle
 *   @version   0.1.0
 *
 *   @pre       N/A
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: March 16, 2016
 *
 *   @copyright Copyright (c) 2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
**/

#ifndef PCOE_PREDICTOR_H
#define PCOE_PREDICTOR_H

#include <vector>
#include <string>

#include "Model.h"
#include "LoadEstimator.h"
#include "LoadEstimatorFactory.h"
#include "PrognosticsModel.h"
#include "ProgData.h"
#include "ThreadSafeLog.h"

namespace PCOE {
    
    const std::string LOAD_ESTIMATOR_KEY = "Predictor.loadEstimator";
    
    class Predictor {
    public:
        Predictor(GSAPConfigMap & configMap) : pModel(NULL), log(Log::Instance()) {
            configMap.checkRequiredParams({LOAD_ESTIMATOR_KEY});
            LoadEstimatorFactory & loadEstFact = LoadEstimatorFactory::instance();
            
            loadEstimator = std::unique_ptr<LoadEstimator>(loadEstFact.Create(configMap[LOAD_ESTIMATOR_KEY][0], configMap));
        }

        virtual ~Predictor() = default;

        /** @brief Set model pointer
         *  @param model given model pointer
         **/
        virtual void setModel(PrognosticsModel *model) {
            pModel = model;
            loadEstimator->setModel(model);
        }
        
        /** @brief    Predict future events and values of system variables
         *  @param    tP Time of prediction
         *  @param    state state of system at time of prediction
         *  @param  data ProgData object, in which prediction results are stored
         **/
        virtual void predict(const double tP, const std::vector<UData> & state, ProgData & data) = 0;

    protected:
        PrognosticsModel * pModel;  // model used for prediction
        std::unique_ptr<LoadEstimator> loadEstimator;
        double horizon;            // time span of prediction
        std::vector<std::string> predictedOutputs;  // list of variables for which to compute future values of
        Log &log;  ///> Logger (Defined in ThreadSafeLog.h)
    };
}

#endif  // PCOE_PREDICTOR_H
