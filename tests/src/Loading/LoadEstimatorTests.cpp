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
    void registerProfileLoadEstimatorTests(TestContext& context);


    void registerTests(TestContext& context) {
        registerConstLoadEstimatorTests(context);
        registerGaussianLoadEstimatorTests(context);
        registerMovingAverageLoadEstimatorTests(context);
        registerProfileLoadEstimatorTests(context);
    }
}
