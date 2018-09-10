//
//  ParticleFilterTests.h
//  UnitTestAll
//
//  Created by Julian Vu on 2/28/17.

#include "ParticleFilterTests.h"
#include "ConfigMap.h"
#include "Exceptions.h"
#include "Models/BatteryModel.h"
#include "Models/Model.h"
#include "Observers/ParticleFilter.h"
#include "Tank3.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

void ctor() {
    // Create Tank3 model
    Tank3 test = Tank3();

    // Initialize its
    auto u = test.getInputVector();
    auto z = test.getOutputVector();
    auto x = test.initialize(u, z);

    size_t N = 0;
    std::vector<double> processNoise;
    std::vector<double> sensorNoise;
    try {
        ParticleFilter pf = ParticleFilter(test, N, processNoise, sensorNoise);
        Assert::Fail("Constructor did not catch empty processNoise/sensorNoise vectors");
    }
    catch (AssertException&) {
    }
}

void ctorWithNonemptyVectors() {
    // Create Tank3 model
    Tank3 test = Tank3();

    // Initialize its
    auto u = test.getInputVector();
    auto z = test.getOutputVector();
    auto x = test.initialize(u, z);

    size_t N = 0;
    std::vector<double> processNoise;
    std::vector<double> sensorNoise;

    processNoise.push_back(0.0);
    processNoise.push_back(1.0);
    processNoise.push_back(2.0);
    sensorNoise.push_back(0.0);
    sensorNoise.push_back(1.0);
    sensorNoise.push_back(2.0);

    ParticleFilter pf = ParticleFilter(test, N, processNoise, sensorNoise);

    Assert::AreEqual(0, pf.getParticleCount());
    Assert::AreEqual(3, pf.getProcessNoiseVariance().size());
    Assert::AreEqual(3, pf.getSensorNoiseVariance().size());

    std::vector<double> emptySensorNoise;

    try {
        ParticleFilter pf2 = ParticleFilter(test, N, processNoise, emptySensorNoise);
        Assert::Fail("Constructor did not catch empty sensorNoise vector");
    }
    catch (AssertException&) {
    }
}

void ConfigMapCtor() {
    ConfigMap theMap;
    theMap.set("Observer.ParticleCount", "200");
    theMap.set("Observer.ProcessNoise", {"1", "1", "1", "1", "1", "1", "1", "1"});
    theMap.set("Observer.SensorNoise", {"1", "1"});
    theMap.set("Observer.MinEffective", "100");

    BatteryModel battery;

    ParticleFilter pf = ParticleFilter(battery, theMap);
}

void PFinitialize() {
    // Create Tank3 model
    Tank3 test = Tank3();

    // Initialize its
    auto u = test.getInputVector();
    auto z = test.getOutputVector();
    auto x = test.initialize(u, z);

    size_t N = 200;
    std::vector<double> processNoise;
    std::vector<double> sensorNoise;

    processNoise.push_back(0.0);
    processNoise.push_back(1.0);
    processNoise.push_back(2.0);
    sensorNoise.push_back(0.0);
    sensorNoise.push_back(1.0);
    sensorNoise.push_back(2.0);

    ParticleFilter pf = ParticleFilter(test, N, processNoise, sensorNoise);
    const double t0 = 0;

    pf.initialize(t0, x, u);

    Assert::AreEqual(200, pf.getParticleCount(), "Number of particles is not 200");
}

void step() {
    // Create Tank3 model
    Tank3 test = Tank3();

    // Set parameter values
    test.parameters.K1 = 1;
    test.parameters.K2 = 2;
    test.parameters.K3 = 3;
    test.parameters.R1 = 1;
    test.parameters.R2 = 2;
    test.parameters.R3 = 3;
    test.parameters.R1c2 = 1;
    test.parameters.R2c3 = 2;

    // Initialize its
    auto u = test.getInputVector();
    auto z = test.getOutputVector();
    auto x = test.initialize(u, z);

    size_t N = 20;
    std::vector<double> processNoise;
    std::vector<double> sensorNoise;

    processNoise.push_back(1.0);
    processNoise.push_back(1.0);
    processNoise.push_back(2.0);
    sensorNoise.push_back(1.0);
    sensorNoise.push_back(1.0);
    sensorNoise.push_back(2.0);

    ParticleFilter pf = ParticleFilter(test, N, processNoise, sensorNoise);
    const double t0 = 0;
    const double t1 = 1;

    // Test step() with uninitialized Particle Filter
    try {
        pf.step(t1, u, z);
        Assert::Fail("step() did not catch uninitialized ParticleFilter.");
    }
    catch (AssertException&) {
    }

    pf.initialize(t0, x, u);

    // Test step() with no change in time
    try {
        pf.step(t0, u, z);
        Assert::Fail("step() did not catch unchanged time.");
    }
    catch (AssertException&) {
    }

    pf.setMinEffective(2000);
    Assert::AreEqual(2000, pf.getMinEffective());
    pf.step(t1, u, z);
}

void getStateEstimate() {
    // Create Tank3 model
    Tank3 test = Tank3();

    // Initialize its
    auto u = test.getInputVector();
    auto z = test.getOutputVector();
    auto x = test.initialize(u, z);

    size_t N = 2000;
    std::vector<double> processNoise;
    std::vector<double> sensorNoise;

    processNoise.push_back(1.0);
    processNoise.push_back(1.0);
    processNoise.push_back(2.0);
    sensorNoise.push_back(1.0);
    sensorNoise.push_back(1.0);
    sensorNoise.push_back(2.0);

    ParticleFilter pf = ParticleFilter(test, N, processNoise, sensorNoise);
    std::vector<UData> stateEstimate = pf.getStateEstimate();
    Assert::AreEqual(3, stateEstimate.size());
}
