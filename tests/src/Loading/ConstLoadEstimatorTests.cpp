// Copyright (c) 2018-2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "ConfigMap.h"
#include "Loading/ConstLoadEstimator.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace ConstLoadEstimatorTests {
    void contruct() {
        ConfigMap config;
        try {
            // Construct with missing config keys
            ConstLoadEstimator le(config);
            Assert::Fail("No error on empty config");
        }
        catch (const std::range_error&) {
        }

        config.set("LoadEstimator.Loading", {"1.0", "1.0"});
        ConstLoadEstimator le(config);
        // Construct without excpetion with correct config
    }

    void estimateLoad() {
        ConfigMap config;
        config.set("LoadEstimator.Loading", {"1.0", "1.0"});
        ConstLoadEstimator le(config);

        auto estimate = le.estimateLoad(0.0);
        Assert::AreEqual(2, estimate.size(), "Estimate size");
        Assert::AreEqual(1.0, estimate[0], 1e-15, "First estimate value");
        Assert::AreEqual(1.0, estimate[1], 1e-15, "Second estimate value");
    }

    void addLoad() {
        ConfigMap config;
        config.set("LoadEstimator.Loading", {"1.0", "1.0"});
        ConstLoadEstimator le(config);

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
    void registerConstLoadEstimatorTests(TestContext& context) {
        context.AddTest("Construct", ConstLoadEstimatorTests::contruct, "LoadEstimators");
        context.AddTest("Estimate Load", ConstLoadEstimatorTests::estimateLoad, "LoadEstimators");
        context.AddTest("Add Load", ConstLoadEstimatorTests::addLoad, "LoadEstimators");
    }
}
