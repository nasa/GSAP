// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Loading/LoadEstimatorTests.h"

using namespace PCOE::Test;

namespace LoadEstimatorTests {
    void registerConstLoadEstimatorTests(TestContext& context);
    void registerGaussianLoadEstimatorTests(TestContext& context);
    void registerMovingAverageLoadEstimatorTests(TestContext& context);

    void registerTests(TestContext& context) {
        registerConstLoadEstimatorTests(context);
        registerGaussianLoadEstimatorTests(context);
        registerMovingAverageLoadEstimatorTests(context);
    }
}
