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
#include "GSAPConfigMap.h"
#include "MonteCarloPredictor.h"
#include "PredictorTests.h"
#include "UData.h"

#include "Factory.h"
#include "PrognosticsModelFactory.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

void predictorTestInit() {
    // Set up the log
    Log& log = Log::Instance("PredictorTests.log");
    log.Initialize("PredictorTests", "1.0", "No comments.");

    // Create the model factory and register battery model
    PrognosticsModelFactory& modelFactory = PrognosticsModelFactory::instance();
    modelFactory.Register("Battery", PrognosticsModelFactory::Create<BatteryModel>);
}

void testMonteCarloBatteryPredict() {
    // Set up configMap
    GSAPConfigMap configMap;
    configMap.set("Predictor.numSamples", "10");
    configMap.set("Predictor.horizon", "5000");
    std::vector<std::string> processNoise;
    for (unsigned int i = 0; i < 8; i++) {
        processNoise.push_back("1e-5");
    }
    configMap["Model.processNoise"] = processNoise;
    configMap["Predictor.loadEstimator"] = std::vector<std::string>({"const"});
    configMap["LoadEstimator.loading"] = std::vector<std::string>({"8"});

    // Create a battery model (to help set up inputs for predict)
    BatteryModel battery = BatteryModel();
    auto u0 = Model::input_type({0});
    auto z0 = Model::output_type({20, 4.2});
    auto x = battery.initialize(u0, z0);

    // Create MonteCarloPredictor for battery
    MonteCarloPredictor MCP(configMap);

    // Create and set the model
    PrognosticsModelFactory& pProgModelFactory = PrognosticsModelFactory::instance();
    std::unique_ptr<PrognosticsModel> model =
        std::unique_ptr<PrognosticsModel>(pProgModelFactory.Create("Battery", configMap));
    MCP.setModel(model.get());

    // Set up inputs for predict function
    double t = 0;
    std::vector<UData> state(battery.getStateSize());
    for (unsigned int i = 0; i < battery.getStateSize(); i++) {
        // Set uncertainty type and size
        state[i].uncertainty(UType::MeanCovar);
        state[i].npoints(battery.getStateSize());
        // Set mean
        state[i][MEAN] = x[i];
        // Set covariance
        std::vector<double> covariance(battery.getStateSize());
        for (unsigned int j = 0; j < battery.getStateSize(); j++) {
            if (i == j) {
                covariance[j] = 1e-5;
            }
            else {
                covariance[j] = 1e-10;
            }
        }
        state[i].setVec(COVAR(0), covariance);
    }

    // Create progdata
    ProgData data;
    data.addEvent("EOD");
    data.addSystemTrajectory("SOC");
    data.setUncertainty(UType::Samples);
    data.sysTrajectories.setNSamples(
        10); // numSamples @todo(MD): Is this right? No basic setup function?
    data.setPredictions(1, 5000); // interval, number of predictions
    data.events["EOD"].getTOE().npoints(10); // numSamples

    // Run predict function
    MCP.predict(t, state, data);

    // Compute mean of timeOfEvent and SOC at different time points
    double meanEOD = 0;
    double meanSOCAt1 = 0;
    double meanSOCAt2500 = 0;
    for (unsigned int i = 0; i < data.events["EOD"].getTOE().npoints(); i++) {
        meanEOD += data.events["EOD"].getTOE()[i] / data.events["EOD"].getTOE().npoints();
    }
    for (unsigned int i = 0; i < data.sysTrajectories["SOC"].getNPoints(); i++) {
        meanSOCAt1 += data.sysTrajectories["SOC"][1][i] / data.sysTrajectories["SOC"].getNPoints();
        meanSOCAt2500 +=
            data.sysTrajectories["SOC"][2500][i] / data.sysTrajectories["SOC"].getNPoints();
    }
}

// Test error cases with config parameters
void testMonteCarloBatteryConfig() {
    // Set up configMap
    GSAPConfigMap configMap;
    configMap.set("Predictor.numSamples", "100");
    configMap.set("Predictor.horizon", "5000");
    std::vector<std::string> processNoise;
    for (unsigned int i = 0; i < 8; i++) {
        processNoise.push_back("1e-5");
    }
    configMap["Model.processNoise"] = processNoise;
    configMap["Predictor.loadEstimator"] = std::vector<std::string>({"const"});
    configMap["LoadEstimator.loading"] = std::vector<std::string>({"8"});

    // Create MonteCarloPredictor for battery
    MonteCarloPredictor MCP(configMap);
}
