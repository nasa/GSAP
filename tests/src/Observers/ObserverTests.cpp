// Copyright (c) 2018-2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <iostream>
#include <sstream>

#include "Test.h"

#include "Matrix.h"
#include "Models/BatteryModel.h"
#include "Observers/ParticleFilter.h"
#include "Observers/UnscentedKalmanFilter.h"
#include "Tank3.h"
#include "ThreadSafeLog.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace ObserverTests {
    void observerTestsInit() {
        // Set up the log
        Log& log = Log::Instance("ObserverTests.log");
        log.Initialize("ObserverTests", "1.0", "No comments.");
    }

    void testUKFTankInitialize() {
        // Create Tank model
        Tank3 TankModel = Tank3();

        // Set parameter values
        TankModel.parameters.K1 = 1;
        TankModel.parameters.K2 = 2;
        TankModel.parameters.K3 = 3;
        TankModel.parameters.R1 = 1;
        TankModel.parameters.R2 = 2;
        TankModel.parameters.R3 = 3;
        TankModel.parameters.R1c2 = 1;
        TankModel.parameters.R2c3 = 2;

        // Set up u (input flows)
        auto u = TankModel.getInputVector();
        u[0] = 1;
        u[1] = 1;
        u[2] = 1;

        // Set up x (masses)
        auto x = TankModel.getStateVector();
        x[0] = 0;
        x[1] = 0;
        x[2] = 0;

        // Set up Q
        Matrix Q(TankModel.getStateSize(), TankModel.getStateSize());
        for (unsigned int i = 0; i < TankModel.getStateSize(); i++) {
            // Fill in diagonal
            Q[i][i] = 1e-5;
        }

        // Set up R
        Matrix R(TankModel.getOutputSize(), TankModel.getOutputSize());
        for (unsigned int i = 0; i < TankModel.getOutputSize(); i++) {
            // Fill in diagonal
            R[i][i] = 1e-2;
        }

        // Create a UKF
        UnscentedKalmanFilter UKF = UnscentedKalmanFilter(TankModel, Q, R);
        double t = 0;

        // Make sure that can't step without initializing first
        try {
            auto z = TankModel.getOutputVector();
            UKF.step(t, u, z);
            Assert::Fail();
        }
        catch (...) {
        }

        // Initialize UKF
        UKF.initialize(t, x, u);
    }

    void testUKFTankStep() {
        // Create Tank model
        Tank3 TankModel = Tank3();

        // Set parameter values
        TankModel.parameters.K1 = 1;
        TankModel.parameters.K2 = 2;
        TankModel.parameters.K3 = 3;
        TankModel.parameters.R1 = 1;
        TankModel.parameters.R2 = 2;
        TankModel.parameters.R3 = 3;
        TankModel.parameters.R1c2 = 1;
        TankModel.parameters.R2c3 = 2;

        // Set up u (input flows)
        auto u = TankModel.getInputVector();
        u[0] = 1;
        u[1] = 1;
        u[2] = 1;

        // Set up x (masses)
        auto x = TankModel.getStateVector();
        x[0] = 0;
        x[1] = 0;
        x[2] = 0;

        auto z = TankModel.getOutputVector();

        // Set up state noise
        std::vector<double> ns;
        double nsValue = 0.001;
        ns.push_back(nsValue);
        ns.push_back(nsValue);
        ns.push_back(nsValue);

        // Set up output noise
        std::vector<double> no;
        double noValue = 0.01;
        no.push_back(noValue);
        no.push_back(noValue);
        no.push_back(noValue);

        // Set up Q
        Matrix Q(TankModel.getStateSize(), TankModel.getStateSize());
        for (unsigned int i = 0; i < TankModel.getStateSize(); i++) {
            // Fill in diagonal
            Q[i][i] = 1e-5;
        }

        // Set up R
        Matrix R(TankModel.getOutputSize(), TankModel.getOutputSize());
        for (unsigned int i = 0; i < TankModel.getOutputSize(); i++) {
            // Fill in diagonal
            R[i][i] = 1e-2;
        }

        // Create a UKF
        UnscentedKalmanFilter UKF = UnscentedKalmanFilter(TankModel, Q, R);

        // Initialize UKF
        double t = 0;
        double dt = 0.1;
        UKF.initialize(t, x, u);

        // Make sure can't step without incrementing time
        try {
            UKF.step(t, u, z);
            Assert::Fail("Step without incrementing time");
        }
        catch (...) {
        }

        // Simulate to get outputs for time t
        t += dt;
        x = TankModel.stateEqn(t, x, u, ns, dt);
        z = TankModel.outputEqn(t, x, no);

        // Step UKF for time t
        UKF.step(t, u, z);
    }

    void testUKFTankGetInputs() {
        // Create Tank model
        Tank3 TankModel = Tank3();

        // Set parameter values
        TankModel.parameters.K1 = 1;
        TankModel.parameters.K2 = 2;
        TankModel.parameters.K3 = 3;
        TankModel.parameters.R1 = 1;
        TankModel.parameters.R2 = 2;
        TankModel.parameters.R3 = 3;
        TankModel.parameters.R1c2 = 1;
        TankModel.parameters.R2c3 = 2;

        // Set up u (input flows)
        auto u = TankModel.getInputVector();
        u[0] = 1;
        u[1] = 1;
        u[2] = 1;

        // Set up x (masses)
        auto x = TankModel.getStateVector();
        x[0] = 0;
        x[1] = 0;
        x[2] = 0;

        // Set up state noise
        std::vector<double> ns;
        double nsValue = 0.001;
        ns.push_back(nsValue);
        ns.push_back(nsValue);
        ns.push_back(nsValue);

        // Set up output noise
        std::vector<double> no;
        double noValue = 0.01;
        no.push_back(noValue);
        no.push_back(noValue);
        no.push_back(noValue);

        // Set up outputs
        auto z = TankModel.getOutputVector();

        // Set up Q
        Matrix Q(TankModel.getStateSize(), TankModel.getStateSize());
        for (unsigned int i = 0; i < TankModel.getStateSize(); i++) {
            // Fill in diagonal
            Q[i][i] = 1e-5;
        }

        // Set up R
        Matrix R(TankModel.getOutputSize(), TankModel.getOutputSize());
        for (unsigned int i = 0; i < TankModel.getOutputSize(); i++) {
            // Fill in diagonal
            R[i][i] = 1e-2;
        }

        // Create a UKF
        UnscentedKalmanFilter UKF = UnscentedKalmanFilter(TankModel, Q, R);

        // Initialize UKF
        double t = 0;
        double dt = 0.1;
        UKF.initialize(t, x, u);

        // Simulate to get outputs for time t
        t += dt;
        x = TankModel.stateEqn(t, x, u, ns, dt);
        z = TankModel.outputEqn(t, x, no);

        // Step UKF for time t
        u[0] = 1;
        u[1] = 2;
        u[2] = 3;
        UKF.step(t, u, z);
    }

    void testUKFBatteryInitialize() {
        // Create battery model
        BatteryModel battery = BatteryModel();

        // Initialize
        auto u0 = BatteryModel::input_type({0});
        auto z0 = BatteryModel::output_type({20, 4.2});
        auto x = battery.initialize(u0, z0);

        // Set up inputs
        auto u = battery.getInputVector();

        // Set up Q
        Matrix Q(battery.getStateSize(), battery.getStateSize());
        for (unsigned int i = 0; i < battery.getStateSize(); i++) {
            // Fill in diagonal
            Q[i][i] = 1e-10;
        }

        // Set up R
        Matrix R(battery.getOutputSize(), battery.getOutputSize());
        for (unsigned int i = 0; i < battery.getOutputSize(); i++) {
            // Fill in diagonal
            R[i][i] = 1e-2;
        }

        // Create a UKF
        UnscentedKalmanFilter UKF = UnscentedKalmanFilter(battery, Q, R);

        // Initialize UKF
        // double dt = 0.1;
        double t = 0;
        UKF.initialize(t, x, u);

        // MORE TESTS
    }

    void testUKFBatteryStep() {
        // Create battery model
        BatteryModel battery = BatteryModel();

        // Initialize
        auto u0 = BatteryModel::input_type({0});
        auto z0 = BatteryModel::output_type({20, 4.2});
        auto x = battery.initialize(u0, z0);

        // Set up inputs
        auto u = battery.getInputVector();

        // Set up Q
        Matrix Q(battery.getStateSize(), battery.getStateSize());
        for (unsigned int i = 0; i < battery.getStateSize(); i++) {
            // Fill in diagonal
            Q[i][i] = 1e-10;
        }

        // Set up R
        Matrix R(battery.getOutputSize(), battery.getOutputSize());
        for (unsigned int i = 0; i < battery.getOutputSize(); i++) {
            // Fill in diagonal
            R[i][i] = 1e-2;
        }

        // Create a UKF
        UnscentedKalmanFilter UKF = UnscentedKalmanFilter(battery, Q, R);

        // Set up zNoise
        std::vector<double> zNoise(battery.getOutputSize());
        zNoise[0] = 0.01;
        zNoise[1] = 0.01;

        // Set up xNoise
        std::vector<double> xNoise(battery.getStateSize());

        // Initialize UKF
        double dt = 1;
        double t = 0;
        UKF.initialize(t, x, u);

        // Simulate to get outputs for time t
        t += dt;
        u[0] = 1;
        x = battery.stateEqn(t, x, u, xNoise, dt);
        auto z = battery.outputEqn(t, x, zNoise);

        // Step UKF for time t
        UKF.step(t, u, z);
    }

    void testUKFBatteryFromConfig() {
        ConfigMap paramMap;

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
        paramMap.set("Observer.Q", qStrings);
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
        paramMap.set("Observer.R", rStrings);

        BatteryModel battery;

        // Construct a UKF from the config map
        UnscentedKalmanFilter ukf(battery, paramMap);

        // NOTE: These may not be relevant anymore, because it only does these checks now within
        // initialize...

        // Create a UKF with bad R and ensure throws error
        rStrings.pop_back();
        paramMap.set("Observer.R", rStrings);
        try {
            UnscentedKalmanFilter ukf2(battery, paramMap);
            Assert::Fail();
        }
        catch (...) {
        }

        // Create a UKF with bad Q and ensure throws error
        // Note that it checks Q first, so it is okay that R is also bad
        qStrings.pop_back();
        paramMap.set("Observer.Q", qStrings);
        try {
            UnscentedKalmanFilter ukf3(battery, paramMap);
            Assert::Fail();
        }
        catch (...) {
        }
    }

    void testPFBatteryFromConfig() {
        ConfigMap configMap;

        // Observer parameters
        configMap.set("observer", "ParticleFilter");

        // Build process noise variance vector
        std::vector<std::string> pnStrings;
        for (int i = 0; i < 8; i++) {
            pnStrings.push_back("1e-10");
        }
        configMap.set("Observer.processNoise", pnStrings);

        // Build sensor noise variance vector
        std::vector<std::string> snStrings;
        for (int i = 0; i < 2; i++) {
            snStrings.push_back("1e-3");
        }
        configMap.set("Observer.sensorNoise", snStrings);

        // Set number of particles
        configMap.set("Observer.N", "100");

        BatteryModel battery;

        // Construct a PF from the config map
        ParticleFilter pf(battery, configMap);

        // Create a UKF with bad sn and ensure throws error
        snStrings.pop_back();
        configMap.set("Observer.sensorNoise", snStrings);
        try {
            ParticleFilter pf2(battery, configMap);
            Assert::Fail();
        }
        catch (...) {
        }

        // Create a UKF with bad sn and ensure throws error
        // Note that it checks pn first, so it is okay that sn is also bad
        pnStrings.pop_back();
        configMap.set("Observer.processNoise", pnStrings);
        try {
            ParticleFilter pf3(battery, configMap);
            Assert::Fail();
        }
        catch (...) {
        }
    }

    void testPFBatteryInitialize() {
        // Create battery model
        BatteryModel battery = BatteryModel();

        // Initialize
        auto u0 = BatteryModel::input_type({0});
        auto z0 = BatteryModel::output_type({20, 4.2});
        auto x = battery.initialize(u0, z0);

        // Set up inputs
        auto u = battery.getInputVector();

        // Set up process noise
        std::vector<double> pn(battery.getStateSize());
        for (unsigned int i = 0; i < battery.getStateSize(); i++) {
            pn[i] = 1e-10;
        }

        // Set up process noise
        std::vector<double> sn(battery.getOutputSize());
        for (unsigned int i = 0; i < battery.getOutputSize(); i++) {
            pn[i] = 1e-3;
        }

        // Create a PF
        size_t N = 100;
        ParticleFilter PF = ParticleFilter(battery, N, pn, sn);

        // Initialize PF
        double t = 0;
        PF.initialize(t, x, u);
    }

    void testPFBatteryStep() {
        // Create battery model
        BatteryModel battery = BatteryModel();

        // Initialize
        auto u0 = BatteryModel::input_type({0});
        auto z0 = BatteryModel::output_type({20, 4.2});
        auto x = battery.initialize(u0, z0);

        // Set up inputs
        auto u = battery.getInputVector();

        // Set up process noise
        std::vector<double> pn(battery.getStateSize());
        for (unsigned int i = 0; i < battery.getStateSize(); i++) {
            pn[i] = 1e-10;
        }

        // Set up process noise
        std::vector<double> sn(battery.getOutputSize());
        for (unsigned int i = 0; i < battery.getOutputSize(); i++) {
            sn[i] = 1e-3;
        }

        // Create a PF
        size_t N = 100;
        ParticleFilter PF = ParticleFilter(battery, N, pn, sn);

        // Initialize PF
        double t = 0;
        double dt = 1;
        PF.initialize(t, x, u);

        // Set up zNoise
        std::vector<double> zNoise(battery.getOutputSize());
        zNoise[0] = 0.01;
        zNoise[1] = 0.01;

        // Set up xNoise
        std::vector<double> xNoise(battery.getStateSize());

        // Simulate to get outputs for time t
        t += dt;
        u[0] = 1;
        x = battery.stateEqn(t, x, u, xNoise, dt);
        auto z = battery.outputEqn(t, x, zNoise);

        // Step UKF for time t
        PF.step(t, u, z);
    }

    void registerTests(TestContext& context) {
        context.AddCategoryInitializer("Observer", observerTestsInit);
        // UKF Tank tests
        context.AddTest("UKF Initialize for Tank", testUKFTankInitialize, "Observer");
        context.AddTest("UKF Step for Tank", testUKFTankStep, "Observer");
        context.AddTest("UKF Tank Get Inputs", testUKFTankGetInputs, "Observer");
        
        // UKF Battery tests
        context.AddTest("UKF Battery Construction from ConfigMap",
                        testUKFBatteryFromConfig,
                        "Observer");
        context.AddTest("UKF Initialization for Battery", testUKFBatteryInitialize, "Observer");
        context.AddTest("UKF Step for Battery", testUKFBatteryStep, "Observer");
    }
}
