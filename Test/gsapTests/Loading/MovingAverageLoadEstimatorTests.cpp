// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "ConfigMap.h"
#include "Loading/LoadEstimatorTests.h"
#include "Loading/MovingAverageLoadEstimator.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace MovingAverageLoadEstimatorTests {
    void contruct() {
        ConfigMap config;
        try {
            // Construct with missing config keys
            MovingAverageLoadEstimator le(config);
            Assert::Fail("No error on empty config");
        }
        catch (const std::range_error&) {
        }

        config.set("LoadEstimator.Loading", {"1.0", "1.0"});
        MovingAverageLoadEstimator le(config);
        // Construct without excpetion with required keys

        config.set("LoadEstimator.Loading", {"1.0", "1.0"});
        config.set("LoadEstimator.Window", 2);
        MovingAverageLoadEstimator le2(config);
        // Construct without excpetion with optional
    }

    void addLoad() {
        ConfigMap config;
        config.set("LoadEstimator.Loading", {"1.0", "1.0"});
        MovingAverageLoadEstimator le(config);

        Assert::IsTrue(le.canAddLoad(), "Can add load");

        le.addLoad({1.0, 1.0});
        // Add load without exception
    }

    void estimateLoad() {
        ConfigMap config;
        config.set("LoadEstimator.Loading", {"1.0", "1.0"});
        config.set("LoadEstimator.Window", 2);
        MovingAverageLoadEstimator le(config);

        auto estimate = le.estimateLoad(0.0);
        Assert::AreEqual(2, estimate.size(), "Estimate size");
        Assert::AreEqual(1.0, estimate[0], "First estimate value (0)");
        Assert::AreEqual(1.0, estimate[1], "Second estimate value (0)");

        le.addLoad({0.0, 0.0});
        estimate = le.estimateLoad(0.0);
        Assert::AreEqual(2, estimate.size(), "Estimate size");
        Assert::AreEqual(0.5, estimate[0], 1e-9, "First estimate value (1)");
        Assert::AreEqual(0.5, estimate[1], 1e-9, "Second estimate value (1)");

        le.addLoad({0.0, 0.0});
        estimate = le.estimateLoad(0.0);
        Assert::AreEqual(2, estimate.size(), "Estimate size");
        Assert::AreEqual(0.0, estimate[0], 1e-9, "First estimate value (1)");
        Assert::AreEqual(0.0, estimate[1], 1e-9, "Second estimate value (1)");
    }
}

namespace LoadEstimatorTests {
    void registerMovingAverageLoadEstimatorTests(TestContext& context) {
        context.AddTest("Construct", MovingAverageLoadEstimatorTests::contruct, "LoadEstimators");
        context.AddTest("Add Load", MovingAverageLoadEstimatorTests::addLoad, "LoadEstimators");
        context.AddTest("Estimate Load",
                        MovingAverageLoadEstimatorTests::estimateLoad,
                        "LoadEstimators");
    }
}
