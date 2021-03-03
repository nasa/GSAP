// Copyright (c) 2018-2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

#include "ConfigMap.h"
#include "Factory.h"
#include "Loading/ConstLoadEstimator.h"
#include "MockClasses.h"
#include "Models/BatteryModel.h"
#include "Models/PrognosticsModelFactory.h"
#include "Predictors/MonteCarloPredictor.h"
#include "Test.h"
#include "UData.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace PredictorTests {
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
        auto& toe = eod.getTOE();
        Assert::AreEqual(toe.size(), 10, "ToE must have 10 samples");
        Assert::IsTrue(toe[0] < 3500 && toe[0] > 2500, "ToE should be between 2500-3500");

        auto& eventState = eod.getState();
        Assert::AreEqual(eventState.size(), 1, "No SavePoints");
        Assert::AreEqual(eventState[0].size(), 10, "10 Samples");
        Assert::IsTrue(eventState[0][0] > 0.97, "event state should be ~1.0");

        auto& systemState = eod.getSystemState();
        Assert::AreEqual(systemState.size(), 1, "No SavePoints");
        Assert::AreEqual(systemState[0].size(), 8, "8 States");
        Assert::AreEqual(systemState[0][0].size(), 10, "10 Samples");

        for (unsigned int i = 0; i < toe.npoints(); i++) {
            meanEOD += toe[i] / toe.npoints();
        }
    }

    class SavePtProvider : public ISavePointProvider {
    public:
        std::set<Message::time_point> getSavePts() const override {
            return savePts;
        }


        bool hasChangedSinceSavePtsCall() const override {
            bool init = initialized;
            initialized = true;
            return !init;
        }

    private:
        mutable bool initialized = false;
        std::set<Message::time_point> savePts{
            Message::time_point(std::chrono::seconds(500)),
            Message::time_point(std::chrono::seconds(1000))
        };
    };

    void testMonteCarloBatteryPredictWithSavePts() {
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
        SavePtProvider provider;
        MCP.addSavePtProvider(&provider);

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
        auto& toe = eod.getTOE();
        Assert::AreEqual(toe.size(), 10, "ToE must have 10 samples");
        Assert::IsTrue(toe[0] < 3500 && toe[0] > 2500, "ToE should be between 2500-3500");

        auto& eventState = eod.getState();
        Assert::AreEqual(eventState.size(), 3, "2 SavePoints");
        Assert::AreEqual(eventState[0].size(), 10, "10 Samples");
        Assert::IsTrue(eventState[0][0] > 0.97, "event state should be ~1.0");

        auto& systemState = eod.getSystemState();
        Assert::AreEqual(systemState.size(), 3, "2 SavePoints");
        Assert::AreEqual(systemState[0].size(), 8, "8 States");
        Assert::AreEqual(systemState[0][0].size(), 10, "10 Samples");

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
    
    void registerTests(TestContext& context) {
        context.AddCategoryInitializer("Predictor", predictorTestInit);
        context.AddTest("Monte Carlo Predictor Configuration for Battery",
                        testMonteCarloBatteryConfig,
                        "Predictor");
        context.AddTest("Monte Carlo Prediction for Battery",
                        testMonteCarloBatteryPredict,
                        "Predictor");
        context.AddTest("Monte Carlo Prediction for Battery - with save pts",
                        testMonteCarloBatteryPredictWithSavePts,
                        "Predictor");
    }
}
