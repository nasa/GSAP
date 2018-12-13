//
//  ModelBasedPrognoserTests.cpp
//  testLib
//
//  Created by Teubert, Christopher (ARC-TI) on 12/13/18.
//

#include "Contracts.h"
#include "MockClasses.h"
#include "ModelBasedPrognoser.h"
#include "ModelBasedPrognoserTests.h"
#include "Models/PrognosticsModelFactory.h"
#include "Observers/ObserverFactory.h"
#include "Predictors/PredictorFactory.h"

using namespace PCOE;
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
	
	std::map<MessageId, Datum<double> > data;
	data[MessageId::TestInput0] = Datum<double>(1);
	data[MessageId::TestInput1] = Datum<double>(2);
	data[MessageId::TestOutput0] = Datum<double>(3);
	Prediction result = mbp.step(data);
}
