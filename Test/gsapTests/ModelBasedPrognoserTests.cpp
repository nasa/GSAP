//
//  ModelBasedPrognoserTests.cpp
//  testLib
//
//  Created by Teubert, Christopher (ARC-TI) on 12/13/18.
//

#include <thread>

#include "MockClasses.h"
#include "ModelBasedPrognoser.h"
#include "ModelBasedPrognoserTests.h"
#include "Models/PrognosticsModelFactory.h"
#include "Observers/ObserverFactory.h"
#include "Predictors/PredictorFactory.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

void testWithMockModel() {
	PrognosticsModelFactory::instance().Register<TestPrognosticsModel>("Mock");
	ObserverFactory::instance().Register<TestObserver>("Mock");
	PredictorFactory::instance().Register<TestPredictor>("Mock");
	ConfigMap config;
	config.set("model", "Mock");
	config.set("observer", "Mock");
	config.set("predictor", "Mock");
	config.set("LoadEstimator.Loading", {"1", "2"});
	ModelBasedPrognoser mbp(config);
	
	// Initialize
	std::map<MessageId, Datum<double> > data;
	data[MessageId::TestInput0] = Datum<double>(1);
	data[MessageId::TestInput1] = Datum<double>(2);
	data[MessageId::TestOutput0] = Datum<double>(3);
	mbp.step(data);
	
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	// First Step
	data[MessageId::TestInput0] = Datum<double>(1);
	data[MessageId::TestInput1] = Datum<double>(2);
	data[MessageId::TestOutput0] = Datum<double>(3);
	Prediction result = mbp.step(data);
	
	Assert::AreEqual(result.getEvents().size(), 1);
	Assert::AreEqual((double) result.getEvents()[0].getState()[0].get(), 1, 1e-6);
	Assert::AreEqual((double) result.getEvents()[0].getStartTime().get(), 1.5, 1e-6);
	Assert::AreEqual(result.getSystemTrajectories().size(), 0);
	
	// No time passed
	Prediction result2 = mbp.step(data);
	Assert::AreEqual(result2.getEvents().size(), 0);
	Assert::AreEqual(result2.getSystemTrajectories().size(), 0);
	
	TestPrognosticsModel model(config);
	TestObserver obs(model, config);
	TestLoadEstimator loadEst(config);
	TestPredictor pred(model, loadEst, TrajectoryService(), config);
	ModelBasedPrognoser mbp2(config);
	
	// Initialize
	data[MessageId::TestInput0] = Datum<double>(1);
	data[MessageId::TestInput1] = Datum<double>(2);
	data[MessageId::TestOutput0] = Datum<double>(3);
	mbp.step(data);
	
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	// First Step
	data[MessageId::TestInput0] = Datum<double>(1);
	data[MessageId::TestInput1] = Datum<double>(2);
	data[MessageId::TestOutput0] = Datum<double>(3);
	result = mbp.step(data);
	
	Assert::AreEqual(result.getEvents().size(), 1);
	Assert::AreEqual((double) result.getEvents()[0].getState()[0].get(), 1, 1e-6);
	Assert::AreEqual((double) result.getEvents()[0].getStartTime().get(), 1.5, 1e-6);
	Assert::AreEqual(result.getSystemTrajectories().size(), 0);
	
	// No time passed
	result2 = mbp.step(data);
	Assert::AreEqual(result2.getEvents().size(), 0);
	Assert::AreEqual(result2.getSystemTrajectories().size(), 0);
}
