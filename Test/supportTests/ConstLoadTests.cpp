//
//  ConstLoadTests.cpp
//  GSAP
//
//  Created by Teubert, Christopher (ARC-TI) on 2/14/18.
//
//

#include "ConstLoadTests.hpp"
#include "Test.h"
#include "ConstLoadEstimator.h"
#include "GSAPConfigMap.h"

using namespace PCOE::Test;

namespace PCOE {
    void testConstLoad() {
        GSAPConfigMap configMap;
        
        try {
            ConstLoadEstimator c = ConstLoadEstimator(configMap);
            Assert::Fail("Accepted missiong loading key");
        } catch (...) {
        }
        
        LoadEstimate test = {1, 2, 3};
        std::vector<std::string> testStr;
        for (auto && testElement : test) {
            testStr.push_back(std::to_string(testElement));
        }
        configMap.insert(std::pair<std::string, std::vector<std::string> >(ConstLoadEstimator::LOADING_KEY, testStr));
        
        ConstLoadEstimator c = ConstLoadEstimator(configMap);
        
        LoadEstimate test2 = c.estimateLoad(NAN);
        
        Assert::AreEqual(test, test2, "Sampling not correct");
        
        configMap[ConstLoadEstimator::LOADING_KEY] = {};
        
        ConstLoadEstimator c2 = ConstLoadEstimator(configMap);
        
        LoadEstimate test3 = c2.estimateLoad(NAN);
        Assert::IsTrue(test3.empty(), "Empty loading vector sampling doesn't return empty sample");
    }
}
