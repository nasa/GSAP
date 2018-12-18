/**  Event-Driven Prognoser Tests - Header
 *   @class     EventDrivenPrognoserTests EventDrivenPrognoserTests.h
 *
 *   @brief     Event-Driven Prognoser Test Class
 *
 *   Tests for event-driven prognoser class.
 *
 *   @author    Chris Teubert
 *   @version   1.2.0
 *
 *      Created: December 17, 2018
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */


#include <thread>

#include "Datum.h"
#include "ModelBasedEventDrivenPrognoserBuilder.h"
#include "EventDrivenPrognoser.h"
#include "EventDrivenPrognoserTests.h"
#include "MockClasses.h"
#include "Models/PrognosticsModelFactory.h"
#include "Observers/ObserverFactory.h"
#include "Predictors/PredictorFactory.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

const std::string SENSOR_SRC = "test_sensor_src";
const std::string TRAJ_SRC = "test_traj_src";

Datum<UData>::time_point addOneSecond(PCOE::Datum<UData>::ms_rep time) {
    auto a = std::chrono::milliseconds(time + PCOE::Datum<UData>::ms_rep(1000));
    return Datum<UData>::time_point(a);
}

void testEDPWithMockModel() {
    PrognosticsModelFactory::instance().Register<TestPrognosticsModel>("Mock");
    ObserverFactory::instance().Register<TestObserver>("Mock");
    PredictorFactory::instance().Register<TestPredictor>("Mock");
    ModelBasedEventDrivenPrognoserBuilder builder;
    builder.setModelName("Mock", true);
    builder.setObserverName("Mock");
    builder.setPredictorName("Mock");
    builder.setConfigParam("LoadEstimator.Loading", std::vector<std::string>({"1", "2"}));
    MessageBus bus;
    EventDrivenPrognoser prognoser = builder.build(bus, SENSOR_SRC, TRAJ_SRC);
    
    // Initialize
    std::map<MessageId, Datum<double> > data;
    data[MessageId::TestInput0] = Datum<double>(1);
    data[MessageId::TestInput1] = Datum<double>(2);
    data[MessageId::TestOutput0] = Datum<double>(3);
    prognoser.step(data);
    
    // TODO(CT): Test with Config map
    //ModelBasedPrognoser mbp(config);
    ConfigMap config;
    config.set("model", "Mock");
    config.set("observer", "Mock");
    config.set("predictor", "Mock");
    config.set("LoadEstimator.Loading", {"1", "2"});

    

    
    
}
