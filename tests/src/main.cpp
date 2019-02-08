// Copyright (c) 2016-2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <fstream>

#include "Test.h"

using namespace PCOE::Test;

namespace AsyncIntegrationTests {
    void registerTests(TestContext& context);
}

namespace SyncIntegrationTests {
    void registerTests(TestContext& context);
}

namespace BatteryResultTests {
    void registerTests(TestContext& context);
}

namespace ConfigMapTests {
    void registerTests(TestContext& context);
}

namespace DataPointTests {
    void registerTests(TestContext& context);
}

namespace DataPointsTests {
    void registerTests(TestContext& context);
}

namespace DataStoreTests {
    void registerTests(TestContext& context);
}

namespace DynamicArrayTests {
    void registerTests(TestContext& context);
}

namespace AsyncObserverTests {
    void registerTests(TestContext& context);
}

namespace AsyncPredictorTests {
    void registerTests(TestContext& context);
}

namespace AsyncPrognoserTests {
    void registerTests(TestContext& context);
}

namespace GaussianVariableTests {
    void registerTests(TestContext& context);
}

namespace LoadEstimatorTests {
    void registerTests(TestContext& context);
}

namespace MatrixTests {
    void registerTests(TestContext& context);
}

namespace MessageBusTests {
    void registerTests(TestContext& context);
}

namespace MessageWatcherTests {
    void registerTests(TestContext& context);
}

namespace ModelBasedPrognoserTests {
    void registerTests(TestContext& context);
}

namespace ModelTests {
    void registerTests(TestContext& context);
}

namespace ObserverTests {
    void registerTests(TestContext& context);
}

namespace ParticleFilterTests {
    void registerTests(TestContext& context);
}

namespace PredictorTests {
    void registerTests(TestContext& context);
}

namespace StatisticalToolsTests {
    void registerTests(TestContext& context);
}

namespace TrajectoryServiceTests {
    void registerTests(TestContext& context);
}

namespace UDataTests {
    void registerTests(TestContext& context);
}

int main() {
    TestContext context;
    
    BatteryResultTests::registerTests(context);
    ConfigMapTests::registerTests(context);
    DataPointTests::registerTests(context);
    DataPointsTests::registerTests(context);
    DataStoreTests::registerTests(context);
    DynamicArrayTests::registerTests(context);
    AsyncObserverTests::registerTests(context);
    AsyncPredictorTests::registerTests(context);
    AsyncPrognoserTests::registerTests(context);
    GaussianVariableTests::registerTests(context);
    LoadEstimatorTests::registerTests(context);
    MatrixTests::registerTests(context);
    MessageBusTests::registerTests(context);
    MessageWatcherTests::registerTests(context);
    ModelBasedPrognoserTests::registerTests(context);
    ModelTests::registerTests(context);
    ObserverTests::registerTests(context);
    ParticleFilterTests::registerTests(context);
    StatisticalToolsTests::registerTests(context);
    TrajectoryServiceTests::registerTests(context);
    UDataTests::registerTests(context);
    
    // Integration Tests
    SyncIntegrationTests::registerTests(context);
    AsyncIntegrationTests::registerTests(context);

    int result = context.Execute();
    std::ofstream junit("testresults/support.xml");
    context.WriteJUnit(junit);
    return result;
}
