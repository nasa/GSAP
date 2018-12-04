/**  Model Unit Tests - Body
*   @ingroup   GPIC++
*   @ingroup   GSAP-Support
*
*   @brief     Unit tests for Model class and derived classes.
*
*   @author    Matthew Daigle
*   @version   1.1.0
*
*   @pre       N/A
*
*      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
*      Created: March 7, 2016
*
*   @copyright Copyright (c) 2018 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#include <iostream>

#include "Test.h"

#include "ModelTests.h"
#include "Model.h"
#include "Tank3.h"
#include "Battery.h"

using namespace PCOE;
using namespace PCOE::Test;

void testTankInitialize()
{
    // Create Tank3 model
    Tank3 TankModel = Tank3();

    // Initialize it
    std::vector<double> x(TankModel.getNumStates());
    std::vector<double> z(TankModel.getNumOutputs());
    std::vector<double> u(TankModel.getNumInputs());
    TankModel.initialize(x, u, z);

    // Check all states set to zero
    Assert::AreEqual(0, x[0], 1e-12);
    Assert::AreEqual(0, x[1], 1e-12);
    Assert::AreEqual(0, x[2], 1e-12);
}

void testTankStateEqn()
{
    // Create Tank3 model
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
    double nsValue = 0;
    ns.push_back(nsValue);
    ns.push_back(nsValue);
    ns.push_back(nsValue);

    // Step state equation
    double t = 0;
    TankModel.setDt(0.1);
    TankModel.Model::stateEqn(t, x, u, ns);

    // Check values of x
    Assert::AreEqual(0.1, x[0], 1e-12);
    Assert::AreEqual(0.1, x[1], 1e-12);
    Assert::AreEqual(0.1, x[2], 1e-12);

    // Check that dt was set properly
    Assert::AreEqual(0.1, TankModel.getDt(), 1e-12);
}

void testTankOutputEqn()
{
    // Create Tank3 model
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
    x.push_back(0.1);
    x.push_back(0.1);
    x.push_back(0.1);

    // Set up output noise
    std::vector<double> no;
    double noValue = 0;
    no.push_back(noValue);
    no.push_back(noValue);
    no.push_back(noValue);

    // Set up outputs
    std::vector<double> z;
    z.push_back(0);
    z.push_back(0);
    z.push_back(0);

    // Output equation
    double t = 0;
    TankModel.outputEqn(t, x, u, no, z);

    // Check values of z
    Assert::AreEqual(0.1, z[0], 1e-12);
    Assert::AreEqual(0.05, z[1], 1e-12);
    Assert::AreEqual(1.0 / 30.0, z[2], 1e-12);
}

void testBatterySetParameters()
{
    // Create battery model
    Battery battery = Battery();

    // Set parameters to defaults
    battery.setParameters(7500);

    // Check a few parameter values
    Assert::AreEqual(1250, battery.parameters.qpSMax, 1e-3);
    Assert::AreEqual(5000, battery.parameters.qpMin, 1e-12);
    Assert::AreEqual(12500, battery.parameters.qpMax, 1e-12);
}

void testBatteryInitialization()
{
    // Create battery model
    Battery battery = Battery();

    // Set up state vector
    std::vector<double> x(8);

    // Initialize
    std::vector<double> u0(1);
    std::vector<double> z0(2);
    u0[0] = 0.4;
    z0[0] = 20;
    z0[1] = 4.0;
    battery.initialize(x, u0, z0);

    // Check states
    Assert::AreEqual(293.15, x[battery.indices.states.Tb], 1e-12);
    Assert::AreEqual(battery.parameters.Ro*0.1, x[battery.indices.states.Vo], 1e-12);
    Assert::AreEqual(0, x[battery.indices.states.Vsn], 1e-12);
    Assert::AreEqual(0, x[battery.indices.states.Vsp], 1e-12);
    Assert::IsTrue(x[battery.indices.states.qnB] > 5.62e3 && x[battery.indices.states.qnB] < 5.63e3);
    Assert::IsTrue(x[battery.indices.states.qpB] > 5.771e3 && x[battery.indices.states.qpB] < 5.772e3);
    Assert::IsTrue(x[battery.indices.states.qnS] > 6.2535e2 && x[battery.indices.states.qnS] < 6.2536e2);
    Assert::IsTrue(x[battery.indices.states.qpS] > 6.413e2 && x[battery.indices.states.qpS] < 6.4132e2);
}

void testBatteryStateEqn()
{
    // Create battery model
    Battery battery = Battery();

    // Set up state vector
    std::vector<double> x(8);

    // Initialize
    std::vector<double> u0(1);
    std::vector<double> z0(2);
    u0[0] = 0.4;
    z0[0] = 20;
    z0[1] = 4.0;
    battery.initialize(x, u0, z0);

    // Set noise vector
    std::vector<double> zeroNoise(8);

    // Set input vector
    std::vector<double> u(1);
    u[0] = 1;

    // Compute next state
    battery.Model::stateEqn(0, x, u, zeroNoise);

    // Check states
    Assert::AreEqual(293.15, x[battery.indices.states.Tb], 1e-12);
    Assert::IsTrue(x[battery.indices.states.Vo] > 0.01461 && x[battery.indices.states.Vo] < 0.14611);
    Assert::IsTrue(x[battery.indices.states.Vsn] > 1.34338e-5 && x[battery.indices.states.Vsn] < 1.34339e-5);
    Assert::AreEqual(3.5303160541594348e-06, x[battery.indices.states.Vsp], 1e-11);
    Assert::IsTrue(x[battery.indices.states.qnB] > 5.62818e3 && x[battery.indices.states.qnB] < 5.62819e3);
    Assert::IsTrue(x[battery.indices.states.qnS] > 6.251e2 && x[battery.indices.states.qnS] < 6.2511e2);
    Assert::IsTrue(x[battery.indices.states.qpB] > 5.77181e3 && x[battery.indices.states.qpB] < 5.771821e3);
    Assert::IsTrue(x[battery.indices.states.qpS] > 6.41563e2 && x[battery.indices.states.qpS] < 6.4156335e2);
}

void testBatteryOutputEqn()
{
    // Create battery model
    Battery battery = Battery();

    // Set up state vector
    std::vector<double> x(8);

    // Set up output vector
    std::vector<double> z(2);

    // Initialize
    std::vector<double> u0(1);
    std::vector<double> z0(2);
    u0[0] = 0.4;
    z0[0] = 20;
    z0[1] = 4.0;
    battery.initialize(x, u0, z0);

    // Set noise vector
    std::vector<double> zeroNoise(2);

    // Set input vector
    std::vector<double> u(1);
    u[0] = 1;

    // Compute output
    battery.outputEqn(0, x, u, zeroNoise, z);

    // Check outputs
    Assert::IsTrue(z[battery.indices.outputs.Vm] > 3.999871 && z[battery.indices.outputs.Vm] < 3.9998711);
    Assert::AreEqual(20, z[battery.indices.outputs.Tbm], 1e-12);
}

void testBatteryThresholdEqn()
{
    // Create battery model
    Battery battery = Battery();

    // Set up state vector
    std::vector<double> x(8);

    // Set input vector
    std::vector<double> u(1);
    u[0] = 1;

    // Initialize
    std::vector<double> u0(1);
    std::vector<double> z0(2);
    u0[0] = 0.4;
    z0[0] = 20;
    z0[1] = 4.0;
    battery.initialize(x, u0, z0);

    // Check that not at threshold
    Assert::AreEqual(false, battery.thresholdEqn(0, x, u));

    // Re-initialize to lower voltage
    u0[0] = 0.3;
    z0[0] = 20;
    z0[1] = 3.0;
    battery.initialize(x, u0, z0);
    //battery.initialize(x,3.0,0.1,20);

    // Check that at threshold
    Assert::AreEqual(true, battery.thresholdEqn(0, x, u));
}

void testBatteryInputEqn()
{
    // Create battery model
    Battery battery = Battery();

    // Set input vector
    std::vector<double> u(1);
    u[0] = 0;

    // Set input parameters
    std::vector<double> inputParameters({ 1,2,3,4,5 });

    // Run inputEqn for different time points and check values
    battery.inputEqn(1, inputParameters, u);
    Assert::AreEqual(1, u[0], 1e-12);
}

void testBatteryPredictedOutputEqn()
{
    // Create battery model
    Battery battery = Battery();

    // Set up state vector
    std::vector<double> x(8);

    // Set input vector
    std::vector<double> u(1);
    u[0] = 1;

    // Initialize
    std::vector<double> u0(1);
    std::vector<double> z0(2);
    u0[0] = 0;
    z0[0] = 20;
    z0[1] = 4.2;
    battery.initialize(x, u0, z0);

    // Set up predicted outputs
    std::vector<double> z(battery.getNumPredictedOutputs());
    battery.predictedOutputEqn(0, x, u, z);

    // Check values
    Assert::AreEqual(1, z[0], 1e-5);
}
