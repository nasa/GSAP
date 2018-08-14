// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Loading/GaussianLoadEstimator.h"
#include "Loading/LoadEstimatorTests.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace GaussianLoadEstimatorTests {
    void contruct() {
        ConfigMap config;
        try {
            // Construct with missing config keys
            GaussianLoadEstimator le(config);
            Assert::Fail("No error on empty config");
        }
        catch (const std::range_error&) {
        }

        config.set("LoadEstimator.Loading", {"1.0", "1.0"});
        try {
            // Construct with missing std deviation key
            GaussianLoadEstimator le(config);
            Assert::Fail("No error on empty config");
        }
        catch (const std::range_error&) {
        }

        config.set("LoadEstimator.StdDev", 10.0);
        GaussianLoadEstimator le2(config);
        // Construct without excpetion with single std deviation value

        config.set("LoadEstimator.StdDev", {"10.0", "2.0"});
        GaussianLoadEstimator le3(config);
    }

    void estimateLoad() {
        ConfigMap config;
        config.set("LoadEstimator.Loading", {"1.0", "1.0"});
        config.set("LoadEstimator.StdDev", 10.0);
        GaussianLoadEstimator le(config);

        auto estimate = le.estimateLoad(0.0);
        Assert::AreEqual(2, estimate.size(), "Estimate size");
        Assert::AreNotEqual(1.0, estimate[0], "First estimate value");
        Assert::AreNotEqual(1.0, estimate[1], "Second estimate value");
    }

    void addLoad() {
        ConfigMap config;
        config.set("LoadEstimator.Loading", {"1.0", "1.0"});
        config.set("LoadEstimator.StdDev", 10.0);
        GaussianLoadEstimator le(config);

        Assert::IsFalse(le.canAddLoad(), "Can add load");

        try {
            le.addLoad({1.0, 1.0});
            Assert::Fail("No exception when adding load");
        }
        catch (const std::runtime_error&) {
        }
    }
}

namespace LoadEstimatorTests {
    void registerGaussianLoadEstimatorTests(TestContext& context) {
        context.AddTest("Construct", GaussianLoadEstimatorTests::contruct, "LoadEstimators");
        context.AddTest("Estimate Load",
                        GaussianLoadEstimatorTests::estimateLoad,
                        "LoadEstimators");
        context.AddTest("Add Load", GaussianLoadEstimatorTests::addLoad, "LoadEstimators");
    }
}
