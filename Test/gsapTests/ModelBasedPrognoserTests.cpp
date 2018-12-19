/**  Model-based Prognoser Tests - Body
 *   @class     ModelBasedPrognoserTests ModelBasedPrognoserTests.h
 *
 *   @brief     Model-based Prognoser Test Class
 *
 *   Tests for general model-based prognoser class.
 *
 *   @author    Chris Teubert
 *   @version   1.2.0
 *
 *      Created: December 13, 2018
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */


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

namespace MBProgTestSupport {
    Datum<UData>::time_point addOneSecond(PCOE::Datum<UData>::ms_rep time) {
        auto a = std::chrono::milliseconds(time + PCOE::Datum<UData>::ms_rep(1000));
        return Datum<UData>::time_point(a);
    }
}
using namespace MBProgTestSupport;

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
    
    // First Step
    auto newTime = addOneSecond(data[MessageId::TestInput0].getTime());
    data[MessageId::TestInput0].setTime(newTime);
    data[MessageId::TestInput1].setTime(newTime);
    data[MessageId::TestOutput0].setTime(newTime);
    Prediction result = mbp.step(data);
    
    Assert::AreEqual(result.getEvents().size(), 1);
    Assert::AreEqual(result.getEvents()[0].getState()[0].get(), 1, 1e-6);
    Assert::AreEqual(result.getEvents()[0].getStartTime().get(), 1.5, 1e-6);
    Assert::AreEqual(result.getObservables().size(), 0);

    
    // No time passed
    Prediction result2 = mbp.step(data);
    Assert::AreEqual(result2.getEvents().size(), 0);
    Assert::AreEqual(result2.getObservables().size(), 0);
    
    TestPrognosticsModel model(config);
    TestObserver obs(model, config);
    TestLoadEstimator loadEst(config);
    TestPredictor pred(model, loadEst, TrajectoryService(), config);
    ModelBasedPrognoser mbp2(config);
    
    // Initialize
    mbp2.step(data);
    
    // First Step
    newTime = addOneSecond(data[MessageId::TestInput0].getTime());
    data[MessageId::TestInput0].setTime(newTime);
    data[MessageId::TestInput1].setTime(newTime);
    data[MessageId::TestOutput0].setTime(newTime);
    result = mbp2.step(data);
    
    Assert::AreEqual(result.getEvents().size(), 1);
    Assert::AreEqual(result.getEvents()[0].getState()[0].get(), 1, 1e-6);
    Assert::AreEqual(result.getEvents()[0].getStartTime().get(), 1.5, 1e-6);
    Assert::AreEqual(result.getObservables().size(), 0);
    
    // No time passed
    result2 = mbp2.step(data);
    Assert::AreEqual(result2.getEvents().size(), 0);
    Assert::AreEqual(result2.getObservables().size(), 0);
}
