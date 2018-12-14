/**  Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <iostream>
#include <memory>
#include <vector>

#include "ConfigMap.h"
#include "Factory.h"
#include "Loading/ConstLoadEstimator.h"
#include "MockClasses.h"
#include "Models/BatteryModel.h"
#include "Models/PrognosticsModelFactory.h"
#include "PredictorTests.h"
#include "Predictors/MonteCarloPredictor.h"
#include "Test.h"
#include "UData.h"

using namespace PCOE;
using namespace PCOE::Test;
void predictorTestInit() {
    // Set up the log
    Log& log = Log::Instance("PredictorTests.log");
    log.Initialize("PredictorTests", "1.0", "No comments.");
}

void testMonteCarloBatteryPredict() {
    // Set up configMap
    ConfigMap configMap;
    configMap.set("Predictor.SampleCount", "10");
    configMap.set("Predictor.Horizon", "5000");
    std::vector<std::string> processNoise;
    for (unsigned int i = 0; i < 8; i++) {
        processNoise.push_back("1e-5");
    }
    configMap.set("Model.ProcessNoise", processNoise);
    configMap.set("Predictor.LoadEstimator", std::vector<std::string>({"const"}));
    configMap.set("LoadEstimator.Loading", std::vector<std::string>({"8"}));

    PrognosticsModelFactory& pProgModelFactory = PrognosticsModelFactory::instance();
    std::unique_ptr<PrognosticsModel> model =
        std::unique_ptr<PrognosticsModel>(pProgModelFactory.Create("Battery", configMap));
    auto u0 = BatteryModel::input_type({0});
    auto z0 = BatteryModel::output_type({20, 4.2});
    auto x = model->initialize(u0, z0);

    TestLoadEstimator le(configMap);
    TrajectoryService ts;

    // Create MonteCarloPredictor for battery
    MonteCarloPredictor MCP(*model, le, ts, configMap);

    // Set up inputs for predict function
    double t = 0;
    std::vector<UData> state(model->getStateSize());
    for (unsigned int i = 0; i < model->getStateSize(); i++) {
        // Set uncertainty type and size
        state[i].uncertainty(UType::MeanCovar);
        state[i].npoints(model->getStateSize());
        // Set mean
        state[i][MEAN] = x[i];
        // Set covariance
        std::vector<double> covariance(model->getStateSize());
        for (unsigned int j = 0; j < model->getStateSize(); j++) {
            if (i == j) {
                covariance[j] = 1e-5;
            }
            else {
                covariance[j] = 1e-10;
            }
        }
        state[i].setVec(COVAR(0), covariance);
    }

    // Run predict function
    Prediction prediction = MCP.predict(t, state);

    // Compute mean of timeOfEvent and SOC at different time points
    double meanEOD = 0;
    auto& eod = prediction.getEvents()[0];
    auto& toe = eod.getStartTime();
    for (unsigned int i = 0; i < toe.npoints(); i++) {
        meanEOD += toe[i] / toe.npoints();
    }
}

// Test error cases with config parameters
void testMonteCarloBatteryConfig() {
    // Set up configMap
    ConfigMap configMap;
    configMap.set("Predictor.SampleCount", "100");
    configMap.set("Predictor.Horizon", "5000");
    std::vector<std::string> processNoise;
    for (unsigned int i = 0; i < 8; i++) {
        processNoise.push_back("1e-5");
    }
    configMap.set("Model.ProcessNoise", processNoise);
    configMap.set("Predictor.LoadEstimator", std::vector<std::string>({"const"}));
    configMap.set("LoadEstimator.Loading", std::vector<std::string>({"8"}));
    configMap.set("Predictor.Events", std::vector<std::string>({"EOD"}));

    BatteryModel battery;

    ConstLoadEstimator le(configMap);
    TrajectoryService ts;

    // Create MonteCarloPredictor for battery
    MonteCarloPredictor MCP(battery, le, ts, configMap);
}
