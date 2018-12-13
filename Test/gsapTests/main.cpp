// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <fstream>

#include "CommunicatorTests.h"
#include "ConfigMapTests.h"
#include "DPointTests.h"
#include "DPointsTests.h"
#include "DataStoreTests.h"
#include "DynamicArrayTests.h"
#include "FrameworkTests.h"
#include "GaussianVariableTests.h"
#include "Loading/LoadEstimatorTests.h"
#include "Messages/MessageWatcherTests.h"
#include "ModelBasedPrognoserTests.h"
#include "ModelTests.h"
#include "ObserverTests.h"
#include "Observers/EventDrivenObserverTests.h"
#include "ParticleFilterTests.h"
#include "PredictorTests.h"
#include "Predictors/EventDrivenPredictorTests.h"
#include "StatisticalToolsTests.h"
#include "TCPSocketTests.h"
#include "Test.h"
#include "ThreadTests.h"
#include "TrajectoryServiceTests.h"
#include "UDPSocketTests.h"
#include "UDataTests.h"

using namespace PCOE::Test;

namespace MatrixTests {
    void registerTests(TestContext& context);
}

namespace BatteryResultTests {
    void registerTests(TestContext& context);
}

namespace MessageBusTests {
    void registerTests(TestContext& context);
}

int main() {
    TestContext context;

    BatteryResultTests::registerTests(context);

    // Trajectory Service
    context.AddTest("Trajectory Service", PCOE::testTrajectoryService, "Trajectory Service");

    // Config Map Tests
    context.AddTest("Load Arguments", configMapLoadArgs, "Config Map");
    context.AddTest("Use", configMapUse, "Config Map");
    context.AddTest("Load", configMapLoad, "Config Map");
    context.AddTest("Load Nonexistent", configMapLoadNonexistent, "ConfigMap");
    context.AddTest("Add Bad Search Path", configMapAddBadSearchPath, "Config Map");
    context.AddTest("Trim", configMapTrim, "Config Map");
    context.AddTest("Require Keys", configMapRequireKeys, "Config Map");

    // UData Tests
    context.AddTest("construct_default", TestUData::construct_default, "UData");
    context.AddTest("construct_type", TestUData::construct_type, "UData");
    context.AddTest("construct_scalar", TestUData::construct_scalar, "UData");
    context.AddTest("construct_copy", TestUData::construct_copy, "UData");
    context.AddTest("construct_move", TestUData::construct_move, "UData");
    context.AddTest("operator_assign", TestUData::operator_assign, "UData");
    context.AddTest("operator_equal", TestUData::operator_equal, "UData");
    context.AddTest("operator_notequal", TestUData::operator_notequal, "UData");
    context.AddTest("operator_double", TestUData::operator_double, "UData");
    context.AddTest("dist", TestUData::dist, "UData");
    context.AddTest("npoints", TestUData::npoints, "UData");
    context.AddTest("size", TestUData::size, "UData");
    context.AddTest("uncertainty", TestUData::uncertainty, "UData");
    context.AddTest("updated", TestUData::updated, "UData");
    context.AddTest("valid", TestUData::valid, "UData");
    context.AddTest("operator_not", TestUData::operator_not, "UData");
    context.AddTest("iterators_forward", TestUData::iterators_forward, "UData");
    context.AddTest("iterators_reverse", TestUData::iterators_reverse, "UData");
    context.AddTest("access_double", TestUData::access_double, "UData");
    context.AddTest("access_pair", TestUData::access_pair, "UData");
    context.AddTest("access_vector", TestUData::access_vector, "UData");
    context.AddTest("point", TestUData::point, "UData");
    context.AddTest("meanSD", TestUData::meanSD, "UData");
    context.AddTest("meanCovar", TestUData::meanCovar, "UData");
    context.AddTest("percentiles", TestUData::percentiles, "UData");
    context.AddTest("samples", TestUData::samples, "UData");
    context.AddTest("wSamples", TestUData::wSamples, "UData");

    // DStore Tests
    context.AddTest("Init", DStoreInit, "DStore");
    context.AddTest("Use", DStoreUse, "DStore");

    // DPoints Tests
    context.AddTest("DPoint Init", testDPointsInit, "DPoints");
    context.AddTest("D Point Update", testDPointsUpdate, "DPoints");
    context.AddTest("D Points Includes", testDPointsIncludes, "DPoints");

    // DPoint Tests
    context.AddTest("Initialization", testDPointInit, "DPoint");
    context.AddTest("Update", testDPointUpdate, "DPoint");
	
	context.AddTest("Mock Model Test", testWithMockModel, "MBP");

    MatrixTests::registerTests(context);

    // Model Tests
    context.AddTest("Tank Initialization", testTankInitialize, "Model Tank");
    context.AddTest("Tank State Eqn", testTankStateEqn, "Model Tank");
    context.AddTest("Tank Output Eqn", testTankOutputEqn, "Model Tank");

    context.AddTest("Battery Set Parameters", testBatterySetParameters, "Model Battery");
    context.AddTest("Battery Initialization", testBatteryInitialization, "Model Battery");
    context.AddTest("Battery State Eqn", testBatteryStateEqn, "Model Battery");
    context.AddTest("Battery Output Eqn", testBatteryOutputEqn, "Model Battery");
    context.AddTest("Battery Threshold Eqn", testBatteryThresholdEqn, "Model Battery");
    context.AddTest("Battery Predicted Output Eqn", testBatteryPredictedOutputEqn, "Model Battery");

    // Observer Tests
    context.AddCategoryInitializer("Observer", observerTestsInit);
    // UKF Tank tests
    context.AddTest("UKF Initialize for Tank", testUKFTankInitialize, "Observer");
    context.AddTest("UKF Step for Tank", testUKFTankStep, "Observer");
    context.AddTest("UKF Tank Get Inputs", testUKFTankGetInputs, "Observer");

    // UKF Battery tests
    context.AddTest("UKF Battery Construction from ConfigMap",
                    testUKFBatteryFromConfig,
                    "Observer");
    context.AddTest("UKF Initialization for Battery", testUKFBatteryInitialize, "Observer");
    context.AddTest("UKF Step for Battery", testUKFBatteryStep, "Observer");

    // Thread Tests
    context.AddTest("treadctrl", tctrltests, "Thread");
    context.AddTest("Exception", exceptiontest, "Thread");
    context.AddTest("Move Constructor", moveCtor, "Thread");
    context.AddTest("Assignment Operator", assignmentOperator, "Thread");
    context.AddTest("Get ID", testGetID, "Thread");
    // context.AddTest("Destructor", testDestructor, "Thread");

    // Predictor Tests
    context.AddCategoryInitializer("Predictor", predictorTestInit);
    context.AddTest("Monte Carlo Predictor Configuration for Battery",
                    testMonteCarloBatteryConfig,
                    "Predictor");
    context.AddTest("Monte Carlo Prediction for Battery",
                    testMonteCarloBatteryPredict,
                    "Predictor");

    // Statistical Tools Tests
    context.AddTest("Calculate Mean", calculateMean, "Statistical Tools");
    context.AddTest("Calculate Standard Deviation", calculateStDv, "Statistical Tools");
    context.AddTest("Calculate CDF", calculateCDF, "Statistical Tools");

    // Gaussian Variable Tests
    context.AddTest("Constructor Specified", ctorSpecified, "Gaussian Variable");
    context.AddTest("Constructor Unspecified", ctorUnspecified, "Gaussian Variable");
    context.AddTest("Generate Samples Direct", generateSamplesDirect, "Gaussian Variable");
    context.AddTest("Generate Samples ICDUR", generateSamplesICDFUR, "Gaussian Variable");
    context.AddTest("Set Mean Std", setMeanStd, "Gaussian Variable");
    context.AddTest("Evaluate PDF", evaluatePDF, "Gaussian Variable");
    context.AddTest("Evaluate CDF", evaluateCDF, "Gaussian Variable");

    context.AddTest("Constructor", ctor, "Particle Filter");
    context.AddTest("Constructor with Nonempty Vectors",
                    ctorWithNonemptyVectors,
                    "Particle Filter");
    context.AddTest("ConfigMap Constructor", ConfigMapCtor, "Particle Filter");
    context.AddTest("Initialize", PFinitialize, "Particle Filter");
    context.AddTest("Step", step, "Particle Filter");
    context.AddTest("Get State Estimate", getStateEstimate, "Particle Filter");

    LoadEstimatorTests::registerTests(context);

    // TCPSocket Tests
    context.AddTest("TCPSocket Constructor", testTCPctor, "TCPSocket");
    context.AddTest("TCPServer Constructor", testTCPServerCtor, "TCPSocket");
    context.AddTest("TCPSocket Send and Receive", testTCPSendAndReceive, "TCPSocket");
    context.AddTest("TCPSocket Closers", testTCPClose, "TCPSocket");
    context.AddTest("TCPSocket NoDelay", testTCPNoDelay, "TCPSocket");
    context.AddTest("TCPSocket ReceiveBufferSize", testTCPReceiveBufferSize, "TCPSocket");
    context.AddTest("TCPSocket ReceiveTimeout", testTCPReceiveTimeout, "TCPSocket");
    context.AddTest("TCPSocket SendBufferSize", testTCPSendBufferSize, "TCPSocket");
    context.AddTest("TCPSocket SendTimeout", testTCPSendTimeout, "TCPSocket");
    context.AddTest("TCPSocket Exceptions", testTCPExceptions, "TCPSocket");

    // UDPSocket Tests
    context.AddTest("UDPSocket Constructor", testUDPCtor, "UDPSocket");
    context.AddTest("UDPSocket Send", testUDPSendandReceive, "UDPSocket");
    context.AddTest("UDPSocket Exception Handling", testExceptionHandling, "UDPSocket");

    context.AddTest("Prognoser Factory", PrognoserFactoryTest);

	// Communicator Tests
    context.AddTest("construct", TestCommunicator::construct, "Common Communicator");
    context.AddTest("enqueue", TestCommunicator::enqueue, "Common Communicator");
    context.AddTest("subscribe", TestCommunicator::subscribe, "Common Communicator");
    context.AddTest("stop", TestCommunicator::stop, "Common Communicator");

    MessageBusTests::registerTests(context);

    context.AddTest("Construct", MessageWatcherTests::constructor, "MessageWatcher");
    context.AddTest("Publish", MessageWatcherTests::publish, "MessageWatcher");
    context.AddTest("Message Count", MessageWatcherTests::messageCount, "MessageWatcher");

    context.AddTest("construct", EventDrivenObserverTests::constructor, "EventDrivenObserver");
    context.AddTest("processMessage",
                    EventDrivenObserverTests::processMessage,
                    "EventDrivenObserver");

    context.AddTest("construct", EventDrivenPredictorTests::constructor, "EventDrivenPredictor");
    context.AddTest("processMessage",
                    EventDrivenPredictorTests::processMessage,
                    "EventDrivenPredictor");
    context.AddTest("Full Config", EventDrivenPredictorTests::fullConfig, "EventDrivenPredictor");
    context.AddTest("Save Points", EventDrivenPredictorTests::savePts, "EventDrivenPredictor");

    DynamicArrayTests::registerTests(context);

    int result = context.Execute();
    std::ofstream junit("testresults/support.xml");
    context.WriteJUnit(junit);
    return result;
}
