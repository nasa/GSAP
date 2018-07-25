//
//  BatteryPrognoserTests.cpp
//
//  Created by Matthew Daigle on 3/17/2016.
//  Copyright (c) 2016 NASA Diagnostics and Prognostics Group. All rights reserved.
//

#include <iostream>

#include "BatteryModel.h"
#include "BatteryPrognoserTests.h"
#include "CommunicatorFactory.h"
#include "GSAPConfigMap.h"
#include "ModelBasedPrognoser.h"
#include "ModelFactory.h"
#include "Observers/ObserverFactory.h"
#include "Observers/UnscentedKalmanFilter.h"
#include "PlaybackCommunicator.h"
#include "Predictors/MonteCarloPredictor.h"
#include "Predictors/PredictorFactory.h"
#include "PrognosticsModelFactory.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

void batteryPrognoserInit() {
    GSAPConfigMap paramMap;
    paramMap.set("Communicators", "../cfg/BatteryPlayback.cfg");
    // paramMap.set("commmanager.step_size", "1000");
}

PCOE::ModelBasedPrognoser* createBatteryPrognoser() {
    // A battery prognoser is constructed as a ModelBasedPrognoser with a Battery, a UKF, and a
    // MonteCarloPredictor
    GSAPConfigMap paramMap;
    paramMap.set("model", "Battery");
    paramMap.set("Battery.qMobile", "7600");
    paramMap.set("Battery.Ro", "0.117215");
    paramMap.set("Battery.VEOD", "3.2");

    // Observer parameters
    paramMap.set("observer", "UKF");
    // Build Q vector
    std::vector<std::string> qStrings;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (i == j) {
                qStrings.push_back("1e-10");
            }
            else {
                qStrings.push_back("0");
            }
        }
    }
    paramMap["Observer.Q"] = qStrings;
    // Build R string
    std::vector<std::string> rStrings;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            if (i == j) {
                rStrings.push_back("1e-2");
            }
            else {
                rStrings.push_back("0");
            }
        }
    }
    paramMap["Observer.R"] = rStrings;

    /*for (unsigned int i = 0; i < 8*8; i++) {
        std::cout << qStrings[i] << ", ";
    }
    std::cout << std::endl;
    for (unsigned int i = 0; i < 4; i++) {
        std::cout << rStrings[i] << ", ";
    }
    std::cout << std::endl;*/

    // Predictor parameters
    paramMap.set("predictor", "MC");
    paramMap.set("Predictor.numSamples", "10");
    paramMap.set("Predictor.horizon", "5000");
    paramMap.set("Model.event", "EOD");
    paramMap.set("Model.predictedOutputs", "SOC");
    std::vector<std::string> processNoise;
    for (unsigned int i = 0; i < 8; i++) {
        processNoise.push_back("1e-5");
    }
    paramMap["Model.processNoise"] = processNoise;
    // Create a constant loading scenario (single portion)
    std::vector<std::string> inputUncertainty;
    inputUncertainty.push_back("8"); // Mean of magnitude
    inputUncertainty.push_back("0.1"); // Std of mangnitude
    inputUncertainty.push_back("5000"); // Mean of duration
    inputUncertainty.push_back("1"); // Std of duration
    paramMap["Predictor.inputUncertainty"] = inputUncertainty;

    // Prognoser parameters
    paramMap.set("type", "mytype");
    paramMap.set("name", "myname");
    paramMap.set("id", "myid");
    std::vector<std::string> inTags({"voltage", "power", "temperature"});
    paramMap["inTags"] = inTags;
    paramMap.set("inputs", "power");
    std::vector<std::string> outputs({"voltage", "temperature"});
    paramMap["outputs"] = outputs;

    // Construct prognoser
    ModelBasedPrognoser* prognoser = NULL;
    try {
        prognoser = new ModelBasedPrognoser(paramMap);
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        Assert::Fail();
    }

    // Return pointer
    return prognoser;
}

void testBatteryPrognoserConstruction() {
    ModelBasedPrognoser* p = createBatteryPrognoser();
    Assert::IsNotNull(p);
    p->stop();
    p->join();
    delete p;
}

void testBatteryPrognoserStep() {
    // Create prognoser
    ModelBasedPrognoser* p = createBatteryPrognoser();

    // Do a step of the prognoser (will intialize)
    p->step();

    // Do another step (will run observer and predictor)
    p->step();

    // Check results (NOTE: These are lower than for PredictorTests since the data file is being run
    // very fast)
    // assert(meanEOD > 2200 && meanEOD < 2600);
    //    assert(meanSOCAt1 > 0.75 && meanSOCAt1 <= 0.85);
    //    assert(meanSOCAt500 > 0.60 && meanSOCAt500 < 0.70);
    p->stop();
    p->join();
    delete p;
}
