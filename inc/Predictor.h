/**  Predictor - Header
 *   @file      Predictor Interface Class
 *   @ingroup   GPIC++
 *   @defgroup  Predictor     Predictor
 *
 *   @brief     Predictor Interface Class - Class defining the Predictor interface (model-based
 *                                          predictor)
 *
 *   @author    Matthew Daigle
 *   @version   1.1.0
 *
 *   @pre       N/A
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: March 16, 2016
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 **/

#ifndef PCOE_PREDICTOR_H
#define PCOE_PREDICTOR_H

#include <functional>
#include <string>
#include <vector>

#include "Model.h"
#include "MovingAverageLoadEstimator.h"
#include "ProgData.h"
#include "PrognosticsModel.h"
#include "ThreadSafeLog.h"

namespace PCOE {
    struct Prediction {
        std::vector<ProgEvent> events;
        std::vector<DataPoint> sysTrajectories;
    };

    class Predictor {
    protected:
        typedef std::function<LoadEstimate(const double, const unsigned int)> LoadEstFcn;
        LoadEstFcn loadEstFcn;
        MovingAverageLoadEstimator defaultLoadEst;
        PrognosticsModel* model;
        double horizon;            // time span of prediction
        Log &log;  ///> Logger (Defined in ThreadSafeLog.h)
        
     public:
        /** Constructor
         *  @param  configMap   Map of configuration parameters
         **/
        Predictor(GSAPConfigMap& configMap) : model(nullptr), log(Log::Instance()) {
            (void)configMap; // Suppress unused parameter warning. Future versions may need the map
            using std::placeholders::_1;
            using std::placeholders::_2;
            loadEstFcn = std::bind(&LoadEstimator::estimateLoad, defaultLoadEst, _1, _2);
        }

        /** Destructor
         **/
        virtual ~Predictor() = default;

        /** @brief Set Load Estimation Function
         *  @param  fcn  load estimation function
         **/
        virtual void setLoadEst(LoadEstFcn fcn) {
            loadEstFcn = fcn;
        }

        /** @brief Set model pointer
         *  @param model given model pointer
         **/
        virtual void setModel(PrognosticsModel* model) {
            this->model = model;
        }

        /** @brief    Predict future events and values of system variables
         *  @param    tP Time of prediction
         *  @param    state state of system at time of prediction
         *  @param  data ProgData object, in which prediction results are stored
         **/
        virtual Prediction predict(const double tP, const std::vector<UData>& state, ProgData& data) = 0;
    };
}

#endif // PCOE_PREDICTOR_H
