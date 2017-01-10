/**  ObserverTests - Body
*   @file      Unit tests for Observer classes
*   @ingroup   GPIC++
*   @ingroup   Observers
*
*   @brief     Unit tests for Observer classes
*
*   @author    Matthew Daigle
*   @version   0.1.0
*
*   @pre       N/A
*
*      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
*      Created: March 17, 2016
*
*   @copyright Copyright (c) 2016 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#include <iostream>

#include "Test.h"

#include "ObserverTests.h"
#include "Tank3.h"
#include "Battery.h"
#include "Matrix.h"
#include "UnscentedKalmanFilter.h"
#include "ThreadSafeLog.h"
#include "ModelFactory.h"

using namespace PCOE;
using namespace PCOE::Test;

void observerTestsInit() {
    // Set up the log
    Log & log = Log::Instance("ObserverTests.log");
    log.Initialize("ObserverTests", "1.0", "No comments.");

    // Create the needed factories
    ModelFactory & pModelFactory = ModelFactory::instance();

    // Register battery model
    pModelFactory.Register("Battery", ModelFactory::Create<Battery>);
}

void testUKFTankInitialize()
{
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
    std::vector<double> u;
    u.push_back(1);
    u.push_back(1);
    u.push_back(1);

    // Set up x (masses)
    std::vector<double> x;
    x.push_back(0);
    x.push_back(0);
    x.push_back(0);

    // Set up Q
    Matrix Q(TankModel.getNumStates(), TankModel.getNumStates());
    for (unsigned int i = 0; i < TankModel.getNumStates(); i++) {
        // Fill in diagonal
        Q[i][i] = 1e-5;
    }

    // Set up R
    Matrix R(TankModel.getNumOutputs(), TankModel.getNumOutputs());
    for (unsigned int i = 0; i < TankModel.getNumOutputs(); i++) {
        // Fill in diagonal
        R[i][i] = 1e-2;
    }

    // Create a UKF
    UnscentedKalmanFilter UKF = UnscentedKalmanFilter(&TankModel, Q, R);
    double t = 0;

    // Make sure that can't step without initializing first
    try {
        std::vector<double> z;
        UKF.step(t, u, z);
        Assert::Fail();
    }
    catch (...) { }

    // Initialize UKF
    UKF.initialize(t, x, u);

    // Check t
    Assert::AreEqual(t, UKF.getTime(), 1e-12);

    // Check x, z, P
    std::vector<double> xMean = UKF.getStateMean();
    std::vector<double> zMean = UKF.getOutputMean();
    Matrix xCov = UKF.getStateCovariance();
    Assert::AreEqual(0, xMean[0], 1e-12);
    Assert::AreEqual(0, xMean[1], 1e-12);
    Assert::AreEqual(0, xMean[2], 1e-12);
    Assert::AreEqual(0, zMean[0], 1e-12);
    Assert::AreEqual(0, zMean[1], 1e-12);
    Assert::AreEqual(0, zMean[2], 1e-12);
    Assert::AreEqual(Q, xCov);
}

void testUKFTankStep()
{
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
    std::vector<double> u;
    u.push_back(1);
    u.push_back(1);
    u.push_back(1);

    // Set up x (masses)
    std::vector<double> x;
    x.push_back(0);
    x.push_back(0);
    x.push_back(0);

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
    std::vector<double> z;
    z.push_back(0);
    z.push_back(0);
    z.push_back(0);

    // Set up Q
    Matrix Q(TankModel.getNumStates(), TankModel.getNumStates());
    for (unsigned int i = 0; i < TankModel.getNumStates(); i++) {
        // Fill in diagonal
        Q[i][i] = 1e-5;
    }

    // Set up R
    Matrix R(TankModel.getNumOutputs(), TankModel.getNumOutputs());
    for (unsigned int i = 0; i < TankModel.getNumOutputs(); i++) {
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
    catch (...) { }

    // Simulate to get outputs for time t
    t += dt;
    TankModel.stateEqn(t, x, u, ns, dt);
    TankModel.outputEqn(t, x, u, no, z);

    // Step UKF for time t
    UKF.step(t, u, z);

    // Check x
    std::vector<double> xMean = UKF.getStateMean();

    Assert::IsTrue(xMean[0] > 0.100007 && xMean[0] < 0.1000072);
    Assert::IsTrue(xMean[1] > 0.1000055 && xMean[1] < 0.100005512);
    Assert::IsTrue(xMean[2] > 0.10000336 && xMean[2] < 0.100003371);

    // Check z
    std::vector<double> zMean = UKF.getOutputMean();
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

void testUKFTankGetInputs()
{
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
    std::vector<double> u;
    u.push_back(1);
    u.push_back(1);
    u.push_back(1);

    // Set up x (masses)
    std::vector<double> x;
    x.push_back(0);
    x.push_back(0);
    x.push_back(0);

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
    std::vector<double> z;
    z.push_back(0);
    z.push_back(0);
    z.push_back(0);

    // Set up Q
    Matrix Q(TankModel.getNumStates(), TankModel.getNumStates());
    for (unsigned int i = 0; i < TankModel.getNumStates(); i++) {
        // Fill in diagonal
        Q[i][i] = 1e-5;
    }

    // Set up R
    Matrix R(TankModel.getNumOutputs(), TankModel.getNumOutputs());
    for (unsigned int i = 0; i < TankModel.getNumOutputs(); i++) {
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
    TankModel.stateEqn(t, x, u, ns, dt);
    TankModel.outputEqn(t, x, u, no, z);

    // Step UKF for time t
    u[0] = 1;
    u[1] = 2;
    u[2] = 3;
    UKF.step(t, u, z);

    // Check that it remembers the inputs
    std::vector<double> uOld = UKF.getInputs();
    Assert::AreEqual(1, uOld[0], 1e-12);
    Assert::AreEqual(2, uOld[1], 1e-12);
    Assert::AreEqual(3, uOld[2], 1e-12);

}

void testUKFBatteryInitialize()
{
    // Create battery model
    Battery battery = Battery();

    // Set up state vector
    std::vector<double> x(8);

    // Initialize
    std::vector<double> u0(1);
    std::vector<double> z0(2);
    u0[0] = 0;
    z0[0] = 20;
    z0[1] = 4.2;
    battery.initialize(x, u0, z0);

    // Set up inputs
    std::vector<double> u(1);

    // Set up Q
    Matrix Q(battery.getNumStates(), battery.getNumStates());
    for (unsigned int i = 0; i < battery.getNumStates(); i++) {
        // Fill in diagonal
        Q[i][i] = 1e-10;
    }

    // Set up R
    Matrix R(battery.getNumOutputs(), battery.getNumOutputs());
    for (unsigned int i = 0; i < battery.getNumOutputs(); i++) {
        // Fill in diagonal
        R[i][i] = 1e-2;
    }

    // Create a UKF
    UnscentedKalmanFilter UKF = UnscentedKalmanFilter(&battery, Q, R);

    // Initialize UKF
    //double dt = 0.1;
    double t = 0;
    UKF.initialize(t, x, u);

    // Check x
    std::vector<double> xMean = UKF.getStateMean();
    Assert::AreEqual(x, xMean);

    // Check z
    std::vector<double> zMean = UKF.getOutputMean();
    Assert::IsTrue(zMean[1] > 4.191423 && zMean[1] < 4.1914237);
    Assert::AreEqual(20, zMean[0], 1e-12);

    // Check P
    Matrix xCov = UKF.getStateCovariance();
    Assert::AreEqual(Q, xCov);
}

void testUKFBatteryStep()
{
    // Create battery model
    Battery battery = Battery();

    // Set up state vector
    std::vector<double> x(8);

    // Initialize
    std::vector<double> u0(1);
    std::vector<double> z0(2);
    u0[0] = 0;
    z0[0] = 20;
    z0[1] = 4.2;
    battery.initialize(x, u0, z0);

    // Set up inputs
    std::vector<double> u(1);

    // Set up Q
    Matrix Q(battery.getNumStates(), battery.getNumStates());
    for (unsigned int i = 0; i < battery.getNumStates(); i++) {
        // Fill in diagonal
        Q[i][i] = 1e-10;
    }

    // Set up R
    Matrix R(battery.getNumOutputs(), battery.getNumOutputs());
    for (unsigned int i = 0; i < battery.getNumOutputs(); i++) {
        // Fill in diagonal
        R[i][i] = 1e-2;
    }

    // Create a UKF
    UnscentedKalmanFilter UKF = UnscentedKalmanFilter(&battery, Q, R);

    // Set up z
    std::vector<double> z(battery.getNumOutputs());

    // Set up zNoise
    std::vector<double> zNoise(battery.getNumOutputs());
    zNoise[0] = 0.01;
    zNoise[1] = 0.01;

    // Set up xNoise
    std::vector<double> xNoise(battery.getNumStates());

    // Initialize UKF
    double dt = 1;
    double t = 0;
    UKF.initialize(t, x, u);

    // Simulate to get outputs for time t
    t += dt;
    u[0] = 1;
    battery.stateEqn(t, x, u, xNoise, dt);
    battery.outputEqn(t, x, u, zNoise, z);

    // Step UKF for time t
    UKF.step(t, u, z);

    // Check x
    std::vector<double> xMean = UKF.getStateMean();
	Assert::AreEqual(-3.515545e-11, xMean[1], 1e-17, "xMean[1]");
    Assert::AreEqual(760, xMean[5], 1e-12, "xMean[5]");

    // Check z
    std::vector<double> zMean = UKF.getOutputMean();
    Assert::AreEqual(20, zMean[0], 1e-6, "zMean[0]");
    Assert::AreEqual(4.191423, zMean[1], 1e-6, "zMean[1]");

    // Check P
    Matrix xCov = UKF.getStateCovariance();
    Assert::AreEqual(2e-10, xCov[0][0], 1e-16, "xCov[0][0]");
    Assert::AreEqual(1.654e-24, xCov[4][6], 1e-23, "xCov[4][6]");
}

void testUKFBatteryFromConfig()
{
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

    // Construct a UKF from the config map
    UnscentedKalmanFilter ukf(paramMap);

    // NOTE: These may not be relevant anymore, because it only does these checks now within initialize...

    // Create a UKF with bad R and ensure throws error
    rStrings.pop_back();
    paramMap["Observer.R"] = rStrings;
    try {
        UnscentedKalmanFilter ukf2(paramMap);
        Assert::Fail();
    }
    catch (...) { }

    // Create a UKF with bad Q and ensure throws error
    // Note that it checks Q first, so it is okay that R is also bad
    qStrings.pop_back();
    paramMap["Observer.Q"] = qStrings;
    try {
        UnscentedKalmanFilter ukf3(paramMap);
        Assert::Fail();
    }
    catch (...) { }
}
