// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <fstream>

#include "CommunicatorTests.h"
#include "ConfigMapTests.h"
#include "DPointTests.h"
#include "DPointsTests.h"
#include "DataStoreTests.h"
#include "FrameworkTests.h"
#include "GaussianVariableTests.h"
#include "LoadTests.hpp"
#include "MatrixTests.h"
#include "Messages/MessageBusTests.h"
#include "Messages/MessageWatcherTests.h"
#include "ModelTests.h"
#include "ObserverTests.h"
#include "Observers/EventDrivenObserverTests.h"
#include "PEventTests.h"
#include "ParticleFilterTests.h"
#include "PredictorTests.h"
#include "Predictors/EventDrivenPredictorTests.h"
#include "ProgManagerTests.h"
#include "StatisticalToolsTests.h"
#include "TCPSocketTests.h"
#include "Test.h"
#include "ThreadTests.h"
#include "UDPSocketTests.h"
#include "UDataTests.h"

using namespace PCOE::Test;

int main() {
    TestContext context;
    // Config Map Tests
    context.AddTest("Init", configMapInit, "Config Map");
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
    context.AddTest("construct_copy", TestUData::construct_copy, "UData");
    context.AddTest("construct_move", TestUData::construct_move, "UData");
    context.AddTest("operator_assign", TestUData::operator_assign, "UData");
    context.AddTest("operator_equal", TestUData::operator_equal, "UData");
    context.AddTest("operator_notequal", TestUData::operator_notequal, "UData");
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

    // Matrix Tests
    // Matrix Creation
    context.AddTest("construct_default", TestMatrix::construct_default, "Matrix");
    context.AddTest("construct_size", TestMatrix::construct_size, "Matrix");
    context.AddTest("construct_initialvalue", TestMatrix::construct_initialvalue, "Matrix");
    context.AddTest("construct_initializerlist", TestMatrix::construct_initializerlist, "Matrix");
    context.AddTest("construct_vector", TestMatrix::construct_vector, "Matrix");
    context.AddTest("construct_copy", TestMatrix::construct_copy, "Matrix");
    context.AddTest("construct_move", TestMatrix::construct_move, "Matrix");
    context.AddTest("operator_assign", TestMatrix::operator_assign, "Matrix");
    // Comparison operators
    context.AddTest("operator_equal", TestMatrix::operator_equal, "Matrix");
    context.AddTest("operator_notequal", TestMatrix::operator_notequal, "Matrix");
    context.AddTest("issquare", TestMatrix::issquare, "Matrix");
    // Basic operations
    context.AddTest("rows", TestMatrix::rows, "Matrix");
    context.AddTest("cols", TestMatrix::cols, "Matrix");
    context.AddTest("indexer", TestMatrix::indexer, "Matrix");
    context.AddTest("indexer_const", TestMatrix::indexer_const, "Matrix");
    context.AddTest("at", TestMatrix::at, "Matrix");
    context.AddTest("const_at", TestMatrix::const_at, "Matrix");
    context.AddTest("col_get", TestMatrix::col_get, "Matrix");
    context.AddTest("col_setmatrix", TestMatrix::col_setmatrix, "Matrix");
    context.AddTest("col_setvector", TestMatrix::col_setvector, "Matrix");
    context.AddTest("row_get", TestMatrix::row_get, "Matrix");
    context.AddTest("row_setmatrix", TestMatrix::row_setmatrix, "Matrix");
    context.AddTest("row_setvector", TestMatrix::row_setvector, "Matrix");
    context.AddTest("operator_vector", TestMatrix::operator_vector, "Matrix");
    context.AddTest("resize", TestMatrix::resize, "Matrix");
    // Arithmetic operations
    context.AddTest("add_matrix", TestMatrix::add_matrix, "Matrix");
    context.AddTest("add_scalar", TestMatrix::add_scalar, "Matrix");
    context.AddTest("subtract_matrix", TestMatrix::subtract_matrix, "Matrix");
    context.AddTest("subtract_salar", TestMatrix::subtract_salar, "Matrix");
    context.AddTest("multiply_matrix", TestMatrix::multiply_matrix, "Matrix");
    context.AddTest("multiply_scalar", TestMatrix::multiply_scalar, "Matrix");
    context.AddTest("divide_scalar", TestMatrix::divide_scalar, "Matrix");
    // Complex operations
    context.AddTest("adjoint", TestMatrix::adjoint, "Matrix");
    context.AddTest("cofactors", TestMatrix::cofactors, "Matrix");
    context.AddTest("determinant", TestMatrix::determinant, "Matrix");
    context.AddTest("laplace determinant", TestMatrix::laplaceDet, "Matrix");
    context.AddTest("diagonal", TestMatrix::diagonal, "Matrix");
    context.AddTest("inverse", TestMatrix::inverse, "Matrix");
    context.AddTest("minors", TestMatrix::minors, "Matrix");
    context.AddTest("submatrix", TestMatrix::submatrix, "Matrix");
    context.AddTest("transpose", TestMatrix::transpose, "Matrix");
    context.AddTest("identity", TestMatrix::identity, "Matrix");
    // Special operations
    context.AddTest("cholesky", TestMatrix::cholesky, "Matrix");
    context.AddTest("weightedmean", TestMatrix::weightedmean, "Matrix");
    context.AddTest("weightedcovariance", TestMatrix::weightedcovariance, "Matrix");
    // Stream insertion
    context.AddTest("stream insertion operator", TestMatrix::streamInsertionOperator, "Matrix");

    // Model Tests
    context.AddTest("Tank Initialization", testTankInitialize, "Model Tank");
    context.AddTest("Tank State Eqn", testTankStateEqn, "Model Tank");
    context.AddTest("Tank Output Eqn", testTankOutputEqn, "Model Tank");

    context.AddTest("Battery Set Parameters", testBatterySetParameters, "Model Battery");
    context.AddTest("Battery Initialization", testBatteryInitialization, "Model Battery");
    context.AddTest("Battery State Eqn", testBatteryStateEqn, "Model Battery");
    context.AddTest("Battery Output Eqn", testBatteryOutputEqn, "Model Battery");
    context.AddTest("Battery Threshold Eqn", testBatteryThresholdEqn, "Model Battery");
    context.AddTest("Battery Input Eqn", testBatteryInputEqn, "Model Battery");
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

    // PEvent Tests
    context.AddTest("Initialization", testPEventInit, "PEvent");
    context.AddTest("Update", testPEventUpdate, "PEvent");

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
    context.AddTest("GSAPConfigMap Constructor", GSAPConfigMapCtor, "Particle Filter");
    context.AddTest("Initialize", PFinitialize, "Particle Filter");
    context.AddTest("Step", step, "Particle Filter");
    context.AddTest("Get State Estimate", getStateEstimate, "Particle Filter");

    context.AddCategoryInitializer("LoadEstimator", PCOE::LoadTestInit);
    context.AddTest("ConstLoadEst", PCOE::testConstLoad, "LoadEstimator");
    context.AddTest("ConstLoadFact", PCOE::testFactory, "LoadEstimator");
    context.AddTest("MovingAverageLoadEst", PCOE::testMovingAverage, "LoadEstimator");
    context.AddTest("ConstLoadUcert", PCOE::testConstLoadWithUncert, "LoadEstimator");

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

    // ProgManager
    context.AddTest("construct_default", TestProgManager::construct_default, "ProgManager");
    context.AddTest("construct_path", TestProgManager::construct_path, "ProgManager");
    context.AddTest("construct_config", TestProgManager::construct_config, "ProgManager");
    context.AddTest("setConfig_path", TestProgManager::setConfig_path, "ProgManager");
    context.AddTest("setConfig_config", TestProgManager::setConfig_config, "ProgManager");

    context.AddTest("construct", TestCommunicator::construct, "Common Communicator");
    context.AddTest("enqueue", TestCommunicator::enqueue, "Common Communicator");
    context.AddTest("subscribe", TestCommunicator::subscribe, "Common Communicator");
    context.AddTest("stop", TestCommunicator::stop, "Common Communicator");

    context.AddTest("construct", MessageBusTests::constructor, "MessageBus");
    context.AddTest("publish", MessageBusTests::publish, "MessageBus");
    context.AddTest("subscribeAll", MessageBusTests::subscribeAll, "MessageBus");
    context.AddTest("subscribeMultiSource", MessageBusTests::subscribeMultiSource, "MessageBus");
    context.AddTest("subscribe", MessageBusTests::subscribe, "MessageBus");
    context.AddTest("unsubscribe", MessageBusTests::unsubscribe, "MessageBus");
    context.AddTest("unsubscribePartial", MessageBusTests::unsubscribePartial, "MessageBus");

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

    int result = context.Execute();
    std::ofstream junit("testresults/support.xml");
    context.WriteJUnit(junit);
    return result;
}
