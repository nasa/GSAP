//
//  ParticleFilterTests.h
//  UnitTestAll
//
//  Created by Julian Vu on 2/28/17.

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
    catch (...) {}
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

    std::vector<double> emptySensorNoise;

    try {
        ParticleFilter pf2 = ParticleFilter(&test, N, processNoise, emptySensorNoise);
        Assert::Fail("Constructor did not catch empty sensorNoise vector");
    }
    catch (...) {}
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
    catch (...) {}
}