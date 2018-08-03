//
//  ConstLoadTests.cpp
//  GSAP
//
//  Created by Teubert, Christopher (ARC-TI) on 2/14/18.
//
//

#include "LoadTests.hpp"
#include "ConstLoadEstimator.h"
#include "ConfigMap.h"
#include "LoadEstimatorFactory.h"
#include "MovingAverageLoadEstimator.h"
#include "Test.h"

using namespace PCOE::Test;

namespace PCOE {
    void LoadTestInit() {}

    void testConstLoad() {
        ConfigMap configMap;

        try {
            ConstLoadEstimator c = ConstLoadEstimator(configMap);
            Assert::Fail("Accepted missiong loading key");
        }
        catch (std::range_error) {
        }

        LoadEstimate test = {1, 2, 3};
        std::vector<std::string> testStr;
        for (auto&& testElement : test) {
            testStr.push_back(std::to_string(testElement));
        }
        configMap.set(ConstLoadEstimator::LOADING_KEY, testStr);

        ConstLoadEstimator c = ConstLoadEstimator(configMap);

        Assert::IsFalse(c.usesHistoricalLoading());
        Assert::IsTrue(c.isSampleBased());

        try {
            c.addLoad({10});
            Assert::Fail();
        }
        catch (std::runtime_error) {
        }

        LoadEstimate test2 = c.estimateLoad(NAN, 0);

        Assert::AreEqual(test, test2, "Sampling not correct");

        configMap.set(ConstLoadEstimator::LOADING_KEY, {});

        ConstLoadEstimator c2 = ConstLoadEstimator(configMap);

        LoadEstimate test3 = c2.estimateLoad(NAN, 0);
        Assert::IsTrue(test3.empty(), "Empty loading vector sampling doesn't return empty sample");
    }

    void testConstLoadWithUncert() {
        ConfigMap configMap;

        LoadEstimate test = {1, 2, 3};
        LoadEstimate std = {0.1, 0.1, 0.1};
        std::vector<std::string> testStr, stdStr;
        for (auto&& testElement : test) {
            testStr.push_back(std::to_string(testElement));
        }
        for (auto&& stdElement : std) {
            stdStr.push_back(std::to_string(stdElement));
        }
        configMap.set(ConstLoadEstimator::LOADING_KEY, testStr);
        configMap.set(ConstLoadEstimator::STDDEV_KEY, stdStr);
        ConstLoadEstimator c = ConstLoadEstimator(configMap);
        Assert::AreEqual(c.getUncertaintyMode(), ConstLoadEstimator::GAUSSIAN);

        //      TODO(CT): test uncertainty sampling in some meaningful way
    }

    void testMovingAverage() {
        ConfigMap configMap;

        MovingAverageLoadEstimator c2 = MovingAverageLoadEstimator(configMap);
        Assert::IsTrue(c2.usesHistoricalLoading());
        Assert::IsFalse(c2.isSampleBased());

        configMap.set(MovingAverageLoadEstimator::WINDOW_SIZE_KEY,
                      std::vector<std::string>({"2"})); // Set window size key;
        MovingAverageLoadEstimator c = MovingAverageLoadEstimator(configMap);

        configMap.set(MovingAverageLoadEstimator::WINDOW_SIZE_KEY,
                      std::vector<std::string>({"-1"})); // Set window size key;
        MovingAverageLoadEstimator c3 = MovingAverageLoadEstimator(configMap);

        LoadEstimate test2 = c.estimateLoad(NAN, 0);
        Assert::IsTrue(test2.empty(), "Estimate not empty with no data yet provided");

        try {
            c.setNSamples(10);
            Assert::Fail();
        }
        catch (std::runtime_error er) {
        }

        LoadEstimate exampleLoad = LoadEstimate({5.0, 1e10, -5e10});
        c.addLoad(exampleLoad);
        test2 = c.estimateLoad(NAN, 0);
        Assert::IsFalse(test2.empty(), "Estimate empty with data provided");
        Assert::AreEqual(test2.size(), exampleLoad.size(), "Load estimate wrong size");
        for (size_t i = 0; i < test2.size(); i++) {
            Assert::AreEqual(exampleLoad[i],
                             test2[i],
                             std::numeric_limits<double>::epsilon(),
                             "single sample test");
        }

        LoadEstimate exampleLoad2 = LoadEstimate({4.5, 5e9, -4e10});
        c.addLoad(exampleLoad2);
        test2 = c.estimateLoad(NAN, 0);
        Assert::IsFalse(test2.empty(), "Estimate empty with data provided");
        Assert::AreEqual(test2.size(), exampleLoad.size(), "Load estimate wrong size");
        for (size_t i = 0; i < test2.size(); i++) {
            Assert::AreEqual((exampleLoad[i] + exampleLoad2[i]) / 2.0,
                             test2[i],
                             2 * std::numeric_limits<double>::epsilon(),
                             "Two sample test");
        }

        c.addLoad(exampleLoad2);
        test2 = c.estimateLoad(NAN, 0);
        Assert::IsFalse(test2.empty(), "Estimate empty with data provided");
        Assert::AreEqual(test2.size(), exampleLoad.size(), "Load estimate wrong size");
        for (size_t i = 0; i < test2.size(); i++) {
            Assert::AreEqual(exampleLoad2[i],
                             test2[i],
                             std::numeric_limits<double>::epsilon(),
                             "Full buffer test");
        }
    }

    void testFactory() {
        ConfigMap configMap;
        LoadEstimate test = {1, 2, 3};
        std::vector<std::string> testStr;
        for (auto&& testElement : test) {
            testStr.push_back(std::to_string(testElement));
        }
        configMap.set(ConstLoadEstimator::LOADING_KEY, testStr);

        LoadEstimatorFactory& f = LoadEstimatorFactory::instance();
        std::unique_ptr<LoadEstimator> c =
            std::unique_ptr<LoadEstimator>(f.Create("const", configMap));
    }
}
