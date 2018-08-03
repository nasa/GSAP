// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <thread>

#include "MockClasses.h"
#include "Observers/EventDrivenObserver.h"
#include "Predictors/EventDrivenPredictor.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace EventDrivenPredictorTests {
    static const auto PUBLISH_DELAY = std::chrono::milliseconds(5);

    void constructor() {
        MessageBus bus;
        TestPrognosticsModel tpm;
        TestLoadEstimator tle;
        const std::string src = "test";

        EventDrivenPredictor edPred(bus,
                                    std::unique_ptr<Predictor>(
                                        new TestPredictor(&tpm, &tle, ConfigMap())),
                                    src);
        // Constructed without exception
    }

    void processMessage() {
        MessageBus bus;
        TestPrognosticsModel tpm;
        TestLoadEstimator tle;
        const std::string src = "test";

        MessageCounter listener(bus, src, MessageId::TestEvent0);
        EventDrivenObserver edObs(bus, std::unique_ptr<Observer>(new TestObserver(&tpm)), src);
        EventDrivenPredictor edPred(bus,
                                    std::unique_ptr<Predictor>(
                                        new TestPredictor(&tpm, &tle, ConfigMap())),
                                    src);
        Assert::AreEqual(0,
                         listener.getCount(),
                         "Predictor produced prediction during construction");

        // First set of messages initializes observer
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput0, src, 0.0)));
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput1, src, 0.0)));
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestOutput0, src, 0.0)));
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
        Assert::AreEqual(0,
                         listener.getCount(),
                         "Predictor produced prediction after one set of data");

        // Second set of messages should trigger a state estimate and prediction
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput0, src, 0.0)));
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput1, src, 0.0)));
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestOutput0, src, 0.0)));
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
        Assert::AreEqual(1, listener.getCount(), "Predictor didn't produce prediction");
    }
}
