//
//  ParticleFilterTests.h
//  UnitTestAll
//
//  Created by Julian Vu on 2/28/17.

#include <Exceptions.h>
#include "Tank3.h"
#include "GSAPConfigMap.h"
#include "ParticleFilter.h"
#include "ParticleFilterTests.h"
#include "Model.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

void ctor() {
    // Create Tank3 model
    Tank3 test = Tank3();

    // Initialize its
    std::vector<double> x(test.getNumStates());
    std::vector<double> z(test.getNumOutputs());
    std::vector<double> u(test.getNumInputs());
    test.initialize(x, u, z);

    size_t N = 0;
    std::vector<double> processNoise;
    std::vector<double> sensorNoise;
    try {
        ParticleFilter pf = ParticleFilter(&test, N, processNoise, sensorNoise);
        Assert::Fail("Constructor did not catch empty processNoise/sensorNoise vectors");
    }
    catch (std::range_error) {}
}

void ctorWithNonemptyVectors() {
    // Create Tank3 model
    Tank3 test = Tank3();

    // Initialize its
    std::vector<double> x(test.getNumStates());
    std::vector<double> z(test.getNumOutputs());
    std::vector<double> u(test.getNumInputs());
    test.initialize(x, u, z);

    size_t N = 0;
    std::vector<double> processNoise;
    std::vector<double> sensorNoise;

    processNoise.push_back(0.0);
    processNoise.push_back(1.0);
    processNoise.push_back(2.0);
    sensorNoise.push_back(0.0);
    sensorNoise.push_back(1.0);
    sensorNoise.push_back(2.0);

    ParticleFilter pf = ParticleFilter(&test, N, processNoise, sensorNoise);

    Assert::AreEqual(0, pf.getNumParticles());
    Assert::AreEqual(3, pf.getProcessNoiseVariance().size());
    Assert::AreEqual(3, pf.getSensorNoiseVariance().size());
    Assert::AreEqual(3, pf.getOutputMean().size());

    std::vector<double> emptySensorNoise;

    try {
        ParticleFilter pf2 = ParticleFilter(&test, N, processNoise, emptySensorNoise);
        Assert::Fail("Constructor did not catch empty sensorNoise vector");
    }
    catch (std::range_error) {}
}

void GSAPConfigMapCtor() {
    GSAPConfigMap theMap;
    theMap.set("Observer.N", "200");
    theMap.set("Observer.processNoise", "20.0");
    theMap.set("Observer.sensorNoise", "20.0");
    theMap.set("Observer.MinNEffective", "100");
    ParticleFilter pf = ParticleFilter(theMap);
}

void PFinitialize() {
    // Create Tank3 model
    Tank3 test = Tank3();

    // Initialize its
    std::vector<double> x(test.getNumStates());
    std::vector<double> z(test.getNumOutputs());
    std::vector<double> u(test.getNumInputs());
    test.initialize(x, u, z);

    size_t N = 200;
    std::vector<double> processNoise;
    std::vector<double> sensorNoise;

    processNoise.push_back(0.0);
    processNoise.push_back(1.0);
    processNoise.push_back(2.0);
    sensorNoise.push_back(0.0);
    sensorNoise.push_back(1.0);
    sensorNoise.push_back(2.0);

    ParticleFilter pf = ParticleFilter(&test, N, processNoise, sensorNoise);
    const double t0 = 0;

    pf.initialize(t0, x, u);

    Assert::AreEqual(0, pf.getTime(), "Time is not 0");
    Assert::AreEqual(3, pf.getStateMean().size(), "Number of states is not 3");
    Assert::AreEqual(3, pf.getInputs().size(), "Number of inputs is not 3");
    Assert::AreEqual(200, pf.getNumParticles(), "Number of particles is not 200");

    GSAPConfigMap theMap;
    theMap.set("Observer.N", "200");
    theMap.set("Observer.processNoise", "20.0");
    theMap.set("Observer.sensorNoise", "20.0");
    theMap.set("Observer.MinNEffective", "100");
    ParticleFilter pf2 = ParticleFilter(theMap);

    try {
        pf2.initialize(t0, x, u);
        Assert::Fail("initialize() didn't catch null model.");
    }
    catch (std::runtime_error) {}
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
    std::vector<double> x(test.getNumStates());
    std::vector<double> z(test.getNumOutputs());
    std::vector<double> u(test.getNumInputs());
    test.initialize(x, u, z);

    size_t N = 20;
    std::vector<double> processNoise;
    std::vector<double> sensorNoise;

    processNoise.push_back(1.0);
    processNoise.push_back(1.0);
    processNoise.push_back(2.0);
    sensorNoise.push_back(1.0);
    sensorNoise.push_back(1.0);
    sensorNoise.push_back(2.0);

    ParticleFilter pf = ParticleFilter(&test, N, processNoise, sensorNoise);
    const double t0 = 0;
    const double t1 = 1;

    // Test step() with uninitialized Particle Filter
    try {
        pf.step(t1, u, z);
        Assert::Fail("step() did not catch uninitialized ParticleFilter.");
    }
    catch (std::domain_error) {}

    pf.initialize(t0, x, u);

    // Test step() with no change in time
    try {
        pf.step(t0, u, z);
        Assert::Fail("step() did not catch unchanged time.");
    }
    catch (std::domain_error) {}

    pf.setMinNEffective(2000);
    Assert::AreEqual(2000, pf.getMinNEffective());
    pf.step(t1, u, z);
}

void getStateEstimate() {
    // Create Tank3 model
    Tank3 test = Tank3();

    // Initialize its
    std::vector<double> x(test.getNumStates());
    std::vector<double> z(test.getNumOutputs());
    std::vector<double> u(test.getNumInputs());
    test.initialize(x, u, z);

    size_t N = 2000;
    std::vector<double> processNoise;
    std::vector<double> sensorNoise;

    processNoise.push_back(1.0);
    processNoise.push_back(1.0);
    processNoise.push_back(2.0);
    sensorNoise.push_back(1.0);
    sensorNoise.push_back(1.0);
    sensorNoise.push_back(2.0);

    ParticleFilter pf = ParticleFilter(&test, N, processNoise, sensorNoise);
    std::vector<UData> stateEstimate = pf.getStateEstimate();
    Assert::AreEqual(3, stateEstimate.size());
}
