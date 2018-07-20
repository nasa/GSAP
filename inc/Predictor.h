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

#include "Assert.h"
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
        LoadEstimator* loadEstimator;
        const PrognosticsModel* model;
        double horizon; // time span of prediction
        Log& log; ///> Logger (Defined in ThreadSafeLog.h)
        std::vector<std::string> events;

    public:
        /** Constructor
         *  @param  configMap   Map of configuration parameters
         **/
        Predictor(const PrognosticsModel* m, LoadEstimator* le, GSAPConfigMap& configMap)
            : model(m), loadEstimator(le), log(Log::Instance()) {
            Expect(m != nullptr, "Model is null");
            Expect(le != nullptr, "Load Estimator is null");
            (void)configMap; // Suppress unused parameter warning. Future versions may need the map
        }

        /** Destructor
         **/
        virtual ~Predictor() = default;

        /** @brief    Predict future events and values of system variables
         *  @param    tP Time of prediction
         *  @param    state state of system at time of prediction
         *  @param  data ProgData object, in which prediction results are stored
         **/
        virtual Prediction predict(const double tP,
                                   const std::vector<UData>& state,
                                   ProgData& data) = 0;

        const std::vector<std::string>& getEvents() {
            return events;
        }
    };
}

#endif // PCOE_PREDICTOR_H
