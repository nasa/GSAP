/**  PredictorTests - Body
*   @file      Unit tests for Predictor classes
*   @ingroup   GPIC++
*   @ingroup	Predictors
*
*   @brief     Unit tests for Predictor classes
*
*   @author    Matthew Daigle
*   @version   0.1.0
*
*   @pre       N/A
*
*      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
*      Created: March 25, 2016
*
*   @copyright Copyright (c) 2016 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#include <iostream>
#include <vector>
#include <memory>

#include "GSAPConfigMap.h"
#include "MonteCarloPredictor.h"
#include "UData.h"
#include "Battery.h"
#include "BatteryEOL.h"
#include "PredictorTests.h"

#include "Test.h"
#include "PrognosticsModelFactory.h"
#include "Factory.h"

using namespace PCOE;
using namespace PCOE::Test;

void predictorTestInit() {
    // Set up the log
    Log & log = Log::Instance("PredictorTests.log");
    log.Initialize("PredictorTests", "1.0", "No comments.");

    // Create the model factory and register battery models
    PrognosticsModelFactory& modelFactory = PrognosticsModelFactory::instance();
    modelFactory.Register("Battery", PrognosticsModelFactory::Create<Battery>);
    modelFactory.Register("BatteryEOL", PrognosticsModelFactory::Create<BatteryEOL>);
}

void testMonteCarloBatteryPredict()
{
    // Set up configMap
    GSAPConfigMap configMap;
    configMap.set("Predictor.numSamples", "10");
    configMap.set("Predictor.horizon", "5000");
    configMap.set("Model.event", "EOD");
    configMap.set("Model.predictedOutputs", "SOC");
    std::vector<std::string> processNoise;
    for (unsigned int i = 0; i < 8; i++) {
        processNoise.push_back("1e-5");
    }
    configMap["Model.processNoise"] = processNoise;
    // Create a constant loading scenario (single portion)
    std::vector<std::string> inputUncertainty;
    inputUncertainty.push_back("8");		// Mean of magnitude
    inputUncertainty.push_back("0.1");		// Std of mangnitude
    configMap["Predictor.inputUncertainty"] = inputUncertainty;
   
    // Create a battery model (to help set up inputs for predict)
    Battery battery = Battery();
    std::vector<double> x(8);
    std::vector<double> u0(1);
    std::vector<double> z0(2);
    u0[0] = 0;
    z0[0] = 20;
    z0[1] = 4.2;
    battery.initialize(x, u0, z0);
    
    // Create MonteCarloPredictor for battery
    MonteCarloPredictor MCP(configMap);
    
    // Create and set the model
    PrognosticsModelFactory & pProgModelFactory = PrognosticsModelFactory::instance();
    std::unique_ptr<PrognosticsModel> model = std::unique_ptr<PrognosticsModel>(pProgModelFactory.Create("Battery", configMap));
    MCP.setModel(model.get());

    // Set up inputs for predict function
    double t = 0;
    std::vector<UData> state(battery.getNumStates());
    for (unsigned int i = 0; i < battery.getNumStates(); i++)
    {
        // Set uncertainty type and size
        state[i].uncertainty(UType::MeanCovar);
        state[i].npoints(battery.getNumStates());
        // Set mean
        state[i][MEAN] = x[i];
        // Set covariance
        std::vector<double> covariance(battery.getNumStates());
        for (unsigned int j = 0; j < battery.getNumStates(); j++) {
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
    data.setUncertainty(UType::Samples);
    data.addEvent("EOD");
    data.addSystemTrajectory("SOC");
    data.sysTrajectories.setNSamples(10);			// numSamples @todo(MD): Is this right? No basic setup function?
    data.setPredictions(1, 5000);						// interval, number of predictions
    data.setupOccurrence(10);							// numSamples
    data.events["EOD"].timeOfEvent.npoints(10);			// numSamples

    // Run predict function
    MCP.predict(t, state, data);

    // Compute mean of timeOfEvent and SOC at different time points
    double meanEOD = 0;
    double meanSOCAt1 = 0;
    double meanSOCAt2500 = 0;
    for (unsigned int i = 0; i < data.events["EOD"].timeOfEvent.npoints(); i++) {
        meanEOD += data.events["EOD"].timeOfEvent[i] / data.events["EOD"].timeOfEvent.npoints();
    }
    for (unsigned int i = 0; i < data.sysTrajectories["SOC"].getNPoints(); i++) {
        meanSOCAt1 += data.sysTrajectories["SOC"][1][i] / data.sysTrajectories["SOC"].getNPoints();
        meanSOCAt2500 += data.sysTrajectories["SOC"][2500][i] / data.sysTrajectories["SOC"].getNPoints();
    }

    // Check results
    Assert::AreEqual(3050, meanEOD, 250);
    Assert::AreEqual(0.995, meanSOCAt1, 0.05);
    Assert::AreEqual(0.275, meanSOCAt2500, 0.025);
}

// Test error cases with config parameters
void testMonteCarloBatteryConfig()
{
    // Set up configMap
    GSAPConfigMap configMap;
    configMap.set("Predictor.numSamples", "100");
    configMap.set("Predictor.horizon", "5000");
    configMap.set("Model.event", "EOD");
    configMap.set("Model.predictedOutputs", "SOC");
    std::vector<std::string> processNoise;
    for (unsigned int i = 0; i < 8; i++) {
        processNoise.push_back("1e-5");
    }
    configMap["Model.processNoise"] = processNoise;
    // Create a constant loading scenario (single portion)
    std::vector<std::string> inputUncertainty;
    inputUncertainty.push_back("8");		// Mean of magnitude
    inputUncertainty.push_back("0.1");		// Std of magnitude
    configMap["Predictor.inputUncertainty"] = inputUncertainty;
    
    // Create MonteCarloPredictor for battery
    MonteCarloPredictor MCP(configMap);

    // Note: the following may not do as intended anymore because these checks are only made in the predict function

    //// Remove a predicted output and ensure error thrown
    //configMap["predictedOutputs"].pop_back();
    //try {
    //    MonteCarloPredictor MCP2(configMap);
    //    Assert::Fail();
    //}
    //catch (std::runtime_error e) {
    //    Assert::AreEqual(1, 1);
    //}

    //// Remove an input uncertainty value and ensure error thrown
    //configMap["inputUncertainty"].pop_back();
    //try {
    //    MonteCarloPredictor MCP3(configMap);
    //    Assert::Fail();
    //}
    //catch (std::runtime_error e) {
    //    Assert::AreEqual(1, 1);
    //}

    //// Remove a process noise value and ensure error thrown
    //configMap["processNoise"].pop_back();
    //try {
    //    MonteCarloPredictor MCP4(configMap);
    //    Assert::Fail();
    //}
    //catch (std::runtime_error e) {
    //    Assert::AreEqual(1, 1);
    //}
}

void testMonteCarloBatteryEOLPredict()
{
    // Set up configMap
    GSAPConfigMap configMap;
    configMap.set("Predictor.numSamples", "10");
    configMap.set("Predictor.horizon", "50");  // reference discharge cycles
    configMap.set("Model.event", "EOL");
    configMap.set("Model.predictedOutputs", "capacity");
    std::vector<std::string> processNoise = {"1e-10", "1e-15", "1e-20"};
    configMap["Model.processNoise"] = processNoise;
    // Create a constant loading scenario (single portion)
    std::vector<std::string> inputUncertainty;
    inputUncertainty.push_back("8");		// Mean of magnitude (average discharge current)
    inputUncertainty.push_back("0.5");		// Std of magnitude
    configMap["Predictor.inputUncertainty"] = inputUncertainty;
    
    // Create a battery model (to help set up inputs for predict)
    Battery batteryEOD = Battery();
    // Create battery EOL model based on that discharge model
    BatteryEOL batteryEOL = BatteryEOL();
    batteryEOL.setDischargeModel(&batteryEOD);
    std::vector<double> x(3);
    std::vector<double> empty;
    batteryEOL.initialize(x, empty, empty);
    
    // Create MonteCarloPredictor for batteryEOL model
    MonteCarloPredictor MCP(configMap);
    
    // Create and set the model with aggressive damage progression
    batteryEOL.parameters.wQMobile = -10;
    MCP.setModel(&batteryEOL);
    
    // Set up inputs for predict function
    double t = 0;
    std::vector<UData> state(batteryEOL.getNumStates());
    for (unsigned int i = 0; i < batteryEOL.getNumStates(); i++)
    {
        // Set uncertainty type and size
        state[i].uncertainty(UType::MeanCovar);
        state[i].npoints(batteryEOL.getNumStates());
        // Set mean
        state[i][MEAN] = x[i];
        // Set covariance
        std::vector<double> covariance(batteryEOL.getNumStates());
        for (unsigned int j = 0; j < batteryEOL.getNumStates(); j++) {
            if (i == j) {
                covariance[j] = std::stod(processNoise[i]);
            }
            else {
                covariance[j] = 0e-30;
            }
        }
        state[i].setVec(COVAR(0), covariance);
    }

    // Create progdata
    ProgData data;
    data.setUncertainty(UType::Samples);
    data.addEvent("EOL");
    data.addSystemTrajectory("capacity");
    data.sysTrajectories.setNSamples(10);           // numSamples
    data.setPredictions(1, 50);                     // interval, number of predictions
    data.setupOccurrence(10);						// numSamples
    data.events["EOL"].timeOfEvent.npoints(10);		// numSamples
    
    // Run predict function
    MCP.predict(t, state, data);
    
    // Compute mean of timeOfEvent and SOC at different time points
    double meanEOL = 0;
    double meanCapacityAt1 = 0;
    double meanCapacityAt25 = 0;
    for (unsigned int i = 0; i < data.events["EOL"].timeOfEvent.npoints(); i++) {
        meanEOL += data.events["EOL"].timeOfEvent[i] / data.events["EOL"].timeOfEvent.npoints();
    }
    for (unsigned int i = 0; i < data.sysTrajectories["capacity"].getNPoints(); i++) {
        meanCapacityAt1 += data.sysTrajectories["capacity"][0][i] / data.sysTrajectories["capacity"].getNPoints();
        meanCapacityAt25 += data.sysTrajectories["capacity"][25][i] / data.sysTrajectories["capacity"].getNPoints();
    }
    
    std::cout << meanEOL << ", " << meanCapacityAt1 << ", " << meanCapacityAt25 << std::endl;

    // Check results
    Assert::AreEqual(39.5, meanEOL, 1);
    Assert::AreEqual(1.93, meanCapacityAt1, 0.02);
    Assert::AreEqual(1.39, meanCapacityAt25, 0.1);
}
