/**  ObserverTests - Body
 *   @file      Unit tests for Observer classes
 *   @ingroup   GPIC++
 *   @ingroup   Observers
 *
 *   @brief     Unit tests for Observer classes
 *
 *   @author    Matthew Daigle
 *   @version   1.1.0
 *
 *   @pre       N/A
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: March 17, 2016
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <iostream>

#include "Test.h"

#include "BatteryModel.h"
#include "Matrix.h"
#include "ModelFactory.h"
#include "ObserverTests.h"
#include "Observers/ParticleFilter.h"
#include "Observers/UnscentedKalmanFilter.h"
#include "Tank3.h"
#include "ThreadSafeLog.h"

using namespace PCOE;
using namespace PCOE::Test;

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
    UnscentedKalmanFilter UKF = UnscentedKalmanFilter(&TankModel, Q, R);
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

    // Check x, z, P
    Model::state_type xMean = UKF.getStateMean();
    Model::output_type zMean = UKF.getOutputMean();
    Matrix xCov = UKF.getStateCovariance();
    Assert::AreEqual(0, xMean[0], 1e-12);
    Assert::AreEqual(0, xMean[1], 1e-12);
    Assert::AreEqual(0, xMean[2], 1e-12);
    Assert::AreEqual(0, zMean[0], 1e-12);
    Assert::AreEqual(0, zMean[1], 1e-12);
    Assert::AreEqual(0, zMean[2], 1e-12);
    Assert::AreEqual(Q, xCov);
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
    UnscentedKalmanFilter UKF = UnscentedKalmanFilter(&TankModel, Q, R);

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
    z = TankModel.outputEqn(t, x, u, no);

    // Step UKF for time t
    UKF.step(t, u, z);

    // Check x
    auto xMean = UKF.getStateMean();

    Assert::IsTrue(xMean[0] > 0.100007 && xMean[0] < 0.1000072);
    Assert::IsTrue(xMean[1] > 0.1000055 && xMean[1] < 0.100005512);
    Assert::IsTrue(xMean[2] > 0.10000336 && xMean[2] < 0.100003371);

    // Check z
    auto zMean = UKF.getOutputMean();
    Assert::IsTrue(zMean[0] > 0.100007 && zMean[0] < 0.1000072);
    Assert::IsTrue(zMean[1] > 0.0500027 && zMean[1] < 0.0500028);
    Assert::IsTrue(zMean[2] > 0.0333344 && zMean[2] < 0.333345);

    // Check P (a few values)
    Matrix xCov = UKF.getStateCovariance();
    Assert::IsTrue(xCov[0][0] > 0.1642e-4 && xCov[0][0] < 0.16421e-4);
    Assert::IsTrue(xCov[1][2] > 0.003869e-4 && xCov[1][2] < 0.386916e-4);
    Assert::IsTrue(xCov[2][1] > 0.003869e-4 && xCov[2][1] < 0.386916e-4);
    Assert::IsTrue(xCov[2][2] > 0.194574e-4 && xCov[2][2] < 0.1945742e-4);
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
    UnscentedKalmanFilter UKF = UnscentedKalmanFilter(&TankModel, Q, R);

    // Initialize UKF
    double t = 0;
    double dt = 0.1;
    UKF.initialize(t, x, u);

    // Simulate to get outputs for time t
    t += dt;
    x = TankModel.stateEqn(t, x, u, ns, dt);
    z = TankModel.outputEqn(t, x, u, no);

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
    auto u0 = Model::input_type({0});
    auto z0 = Model::output_type({20, 4.2});
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
    UnscentedKalmanFilter UKF = UnscentedKalmanFilter(&battery, Q, R);

    // Initialize UKF
    // double dt = 0.1;
    double t = 0;
    UKF.initialize(t, x, u);

    // Check x
    auto xMean = UKF.getStateMean();
    Assert::AreEqual(x, xMean);

    // Check z
    auto zMean = UKF.getOutputMean();
    Assert::IsTrue(zMean[1] > 4.191423 && zMean[1] < 4.1914237);
    Assert::AreEqual(20, zMean[0], 1e-12);

    // Check P
    Matrix xCov = UKF.getStateCovariance();
    Assert::AreEqual(Q, xCov);
}

void testUKFBatteryStep() {
    // Create battery model
    BatteryModel battery = BatteryModel();

    // Initialize
    auto u0 = Model::input_type({0});
    auto z0 = Model::output_type({20, 4.2});
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
    UnscentedKalmanFilter UKF = UnscentedKalmanFilter(&battery, Q, R);

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
    auto z = battery.outputEqn(t, x, u, zNoise);

    // Step UKF for time t
    UKF.step(t, u, z);

    // Check x
    auto xMean = UKF.getStateMean();
    // Note (JW): Delta for this test was originally 1e-17, which worked up to
    //            now. After seeing the test fail on macOS (and only macOS) on
    //            an unrelated change, I'm bumping it up to 1e-16
    Assert::AreEqual(-3.515545e-11, xMean[1], 1e-15, "xMean[1]");
    Assert::AreEqual(760, xMean[5], 1e-12, "xMean[5]");

    // Check z
    auto zMean = UKF.getOutputMean();
    Assert::AreEqual(20, zMean[0], 1e-6, "zMean[0]");
    Assert::AreEqual(4.191423, zMean[1], 1e-6, "zMean[1]");

    // Check P
    Matrix xCov = UKF.getStateCovariance();
    Assert::AreEqual(2e-10, xCov[0][0], 1e-16, "xCov[0][0]");
    Assert::AreEqual(1.654e-24, xCov[4][6], 1e-23, "xCov[4][6]");
}

void testUKFBatteryFromConfig() {
    GSAPConfigMap paramMap;

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

    BatteryModel battery;

    // Construct a UKF from the config map
    UnscentedKalmanFilter ukf(&battery, paramMap);

    // NOTE: These may not be relevant anymore, because it only does these checks now within
    // initialize...

    // Create a UKF with bad R and ensure throws error
    rStrings.pop_back();
    paramMap["Observer.R"] = rStrings;
    try {
        UnscentedKalmanFilter ukf2(&battery, paramMap);
        Assert::Fail();
    }
    catch (...) {
    }

    // Create a UKF with bad Q and ensure throws error
    // Note that it checks Q first, so it is okay that R is also bad
    qStrings.pop_back();
    paramMap["Observer.Q"] = qStrings;
    try {
        UnscentedKalmanFilter ukf3(&battery, paramMap);
        Assert::Fail();
    }
    catch (...) {
    }
}

void testPFBatteryFromConfig() {
    GSAPConfigMap configMap;

    // Observer parameters
    configMap.set("observer", "ParticleFilter");

    // Build process noise variance vector
    std::vector<std::string> pnStrings;
    for (int i = 0; i < 8; i++) {
        pnStrings.push_back("1e-10");
    }
    configMap["Observer.processNoise"] = pnStrings;

    // Build sensor noise variance vector
    std::vector<std::string> snStrings;
    for (int i = 0; i < 2; i++) {
        snStrings.push_back("1e-3");
    }
    configMap["Observer.sensorNoise"] = snStrings;

    // Set number of particles
    configMap.set("Observer.N", "100");

    BatteryModel battery;

    // Construct a PF from the config map
    ParticleFilter pf(&battery, configMap);

    // Create a UKF with bad sn and ensure throws error
    snStrings.pop_back();
    configMap["Observer.sensorNoise"] = snStrings;
    try {
        ParticleFilter pf2(&battery, configMap);
        Assert::Fail();
    }
    catch (...) {
    }

    // Create a UKF with bad sn and ensure throws error
    // Note that it checks pn first, so it is okay that sn is also bad
    pnStrings.pop_back();
    configMap["Observer.processNoise"] = pnStrings;
    try {
        ParticleFilter pf3(&battery, configMap);
        Assert::Fail();
    }
    catch (...) {
    }
}

void testPFBatteryInitialize() {
    // Create battery model
    BatteryModel battery = BatteryModel();

    // Initialize
    auto u0 = Model::input_type({0});
    auto z0 = Model::output_type({20, 4.2});
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
    ParticleFilter PF = ParticleFilter(&battery, N, pn, sn);

    // Initialize PF
    double t = 0;
    PF.initialize(t, x, u);

    // Check x
    auto xMean = PF.getStateMean();
    Assert::AreEqual(x, xMean);

    // Check z
    auto zMean = PF.getOutputMean();
    Assert::IsTrue(zMean[1] > 4.191423 && zMean[1] < 4.1914237);
    Assert::AreEqual(20, zMean[0], 1e-12);
}

void testPFBatteryStep() {
    // Create battery model
    BatteryModel battery = BatteryModel();

    // Initialize
    auto u0 = Model::input_type({0});
    auto z0 = Model::output_type({20, 4.2});
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
    ParticleFilter PF = ParticleFilter(&battery, N, pn, sn);

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
    auto z = battery.outputEqn(t, x, u, zNoise);

    // Step UKF for time t
    PF.step(t, u, z);

    // Check x
    auto xMean = PF.getStateMean();
    Assert::AreEqual(0, xMean[1], 1e-3, "xMean[1]");
    Assert::AreEqual(760, xMean[5], 1e-1, "xMean[5]");

    // Check z
    auto zMean = PF.getOutputMean();
    Assert::AreEqual(20, zMean[0], 1e-6, "zMean[0]");
    Assert::AreEqual(4.191423, zMean[1], 1e-6, "zMean[1]");
}
