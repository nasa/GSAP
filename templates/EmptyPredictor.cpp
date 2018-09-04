/**  EmptyPredictor - Header
 *   @file      EmptyPredictor Class
 *   @ingroup   GPIC++
 *   @ingroup    Predictors
 *
 *   @copyright Copyright (c) 2017-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <random>
#include <string>
#include <vector>

#include "EmptyPredictor.h"
#include "Exceptions.h"
#include "Matrix.h"

namespace PCOE {
    const Log& log = Log::Instance();

    const std::string MODULE_NAME = "PRED-Empty";

    // ConfigMap-based Constructor
    EmptyPredictor::EmptyPredictor(const PrognosticsModel& m, LoadEstimator& le, TrajectoryService& ts, ConfigMap& config)
        : Predictor(m, le, ts, config) {
        // Setup based on configuration parameters
        // ...

        log.WriteLine(LOG_INFO, MODULE_NAME, "EmptyPredictor created");
    }

    // Predict function
    Prediction EmptyPredictor::predict(double t, const std::vector<UData>& state) {
        std::vector<ProgEvent> events;
        std::vector<DataPoint> systemTrajectories;

        // Predict events and trajectories

        return Prediction(std::move(events), std::move(systemTrajectories));
    }
}
