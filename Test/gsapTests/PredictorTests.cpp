/**  PredictorTests - Body
 *   @file      Unit tests for Predictor classes
 *   @ingroup   GPIC++
 *   @ingroup	Predictors
 *
 *   @brief     Unit tests for Predictor classes
 *
 *   @author    Matthew Daigle
 *   @version   1.1.0
 *
 *   @pre       N/A
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: March 25, 2016
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <iostream>
#include <memory>
#include <vector>

#include "BatteryModel.h"
#include "ConstLoadEstimator.h"
#include "Factory.h"
#include "GSAPConfigMap.h"
#include "PredictorTests.h"
#include "Predictors/MonteCarloPredictor.h"
#include "PrognosticsModelFactory.h"
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
    GSAPConfigMap configMap;
    configMap.set("Predictor.SampleCount", "10");
    configMap.set("Predictor.Horizon", "5000");
    std::vector<std::string> processNoise;
    for (unsigned int i = 0; i < 8; i++) {
        processNoise.push_back("1e-5");
    }
    configMap["Model.ProcessNoise"] = processNoise;
    configMap["Predictor.LoadEstimator"] = std::vector<std::string>({"const"});
    configMap["LoadEstimator.loading"] = std::vector<std::string>({"8"});
    configMap["Predictor.Events"] = std::vector<std::string>({"EOD"});

    PrognosticsModelFactory& pProgModelFactory = PrognosticsModelFactory::instance();
    std::unique_ptr<PrognosticsModel> model =
        std::unique_ptr<PrognosticsModel>(pProgModelFactory.Create("Battery", configMap));
    auto u0 = Model::input_type({0});
    auto z0 = Model::output_type({20, 4.2});
    auto x = model->initialize(u0, z0);

    ConstLoadEstimator le(configMap);

    // Create MonteCarloPredictor for battery
    MonteCarloPredictor MCP(model.get(), &le, configMap);

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
    double meanSOCAt1 = 0;
    double meanSOCAt2500 = 0;
    auto& eod = prediction.events[0];
    for (unsigned int i = 0; i < eod.getTOE().npoints(); i++) {
        meanEOD += eod.getTOE()[i] / eod.getTOE().npoints();
    }

    auto& soc = prediction.sysTrajectories[0];
    for (unsigned int i = 0; i < soc.getNPoints(); i++) {
        meanSOCAt1 += soc[1][i] / soc.getNPoints();
        meanSOCAt2500 += soc[2500][i] / soc.getNPoints();
    }
}

// Test error cases with config parameters
void testMonteCarloBatteryConfig() {
    // Set up configMap
    GSAPConfigMap configMap;
    configMap.set("Predictor.SampleCount", "100");
    configMap.set("Predictor.Horizon", "5000");
    std::vector<std::string> processNoise;
    for (unsigned int i = 0; i < 8; i++) {
        processNoise.push_back("1e-5");
    }
    configMap["Model.ProcessNoise"] = processNoise;
    configMap["Predictor.LoadEstimator"] = std::vector<std::string>({"const"});
    configMap["LoadEstimator.loading"] = std::vector<std::string>({"8"});
    configMap["Predictor.Events"] = std::vector<std::string>({"EOD"});

    BatteryModel battery;

    ConstLoadEstimator le(configMap);

    // Create MonteCarloPredictor for battery
    MonteCarloPredictor MCP(&battery, &le, configMap);
}
