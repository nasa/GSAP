// Copyright (c) 2018-2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "ConfigMap.h"
#include "Exceptions.h"
#include "Loading/ProfileLoadEstimator.h"
#include "Test.h"
using namespace PCOE::Test;
using namespace PCOE;

namespace LoadEstimatorTests {
    void registerConstLoadEstimatorTests(TestContext& context);
    void registerGaussianLoadEstimatorTests(TestContext& context);
    void registerMovingAverageLoadEstimatorTests(TestContext& context);
    void profileTest() {
        ConfigMap c;
        c.set("LoadEstimator.LoadingProfileLength", 3);
        c.set("LoadEstimator.Element[0].Duration", 10);
        c.set("LoadEstimator.Element[0].Loads", {"2.5", "10"});
        c.set("LoadEstimator.Element[1].Duration", 15);
        c.set("LoadEstimator.Element[1].Loads", {"4.5", "6"});
        c.set("LoadEstimator.Element[2].Duration", 10);
        c.set("LoadEstimator.Element[2].Loads", {"-5", "5"});
        ProfileLoadEstimator le(c);

        auto a = le.estimateLoad(100);
        using namespace Test;
        Assert::AreEqual(a[0], 2.5, 1e-4);
        Assert::AreEqual(a[1], 10, 1e-4);

        a = le.estimateLoad(109.5);
        Assert::AreEqual(a[0], 2.5, 1e-4);
        Assert::AreEqual(a[1], 10, 1e-4);

        a = le.estimateLoad(110.5);
        Assert::AreEqual(a[0], 4.5, 1e-4);
        Assert::AreEqual(a[1], 6, 1e-4);

        a = le.estimateLoad(124.5);
        Assert::AreEqual(a[0], 4.5, 1e-4);
        Assert::AreEqual(a[1], 6, 1e-4);

        a = le.estimateLoad(125.5);
        Assert::AreEqual(a[0], -5, 1e-4);
        Assert::AreEqual(a[1], 5, 1e-4);

        a = le.estimateLoad(134.5);
        Assert::AreEqual(a[0], -5, 1e-4);
        Assert::AreEqual(a[1], 5, 1e-4);

        try {
            le.estimateLoad(135.5);
            Assert::Fail();
        } catch (OutOfRangeError e) {
        }
    }

    void registerTests(TestContext& context) {
        registerConstLoadEstimatorTests(context);
        registerGaussianLoadEstimatorTests(context);
        registerMovingAverageLoadEstimatorTests(context);
        context.AddTest("Profile Load Estimator", profileTest, "LoadEstimators");
    }
}
