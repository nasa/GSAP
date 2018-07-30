// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "MockClasses.h"
#include "Observers/EventDrivenObserver.h"
#include "Predictors/EventDrivenPredictor.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace EventDrivenPredictorTests {
    class PredictorListener final : public IMessageProcessor {
    public:
        PredictorListener(MessageBus& bus, std::string src) : source(std::move(src)) {
            bus.subscribe(this, source, MessageId::BatteryEod);
        }

        void processMessage(const std::shared_ptr<Message>& message) override {
            ++count;
        }

        inline std::size_t getCount() const {
            return count;
        }

    private:
        std::string source;
        std::size_t count = 0;
    };

    void constructor() {
        MessageBus bus;
        TestPrognosticsModel tpm;
        TestLoadEstimator tle;
        const std::string src = "test";

        EventDrivenPredictor edPred(bus,
                                    std::unique_ptr<Predictor>(
                                        new TestPredictor(&tpm, &tle, GSAPConfigMap())),
                                    src);
        // Constructed without exception
    }

    void processMessage() {
        MessageBus bus;
        TestPrognosticsModel tpm;
        TestLoadEstimator tle;
        const std::string src = "test";

        PredictorListener listener(bus, src);
        EventDrivenObserver edObs(bus, std::unique_ptr<Observer>(new TestObserver(&tpm)), src);
        EventDrivenPredictor edPred(bus,
                                    std::unique_ptr<Predictor>(
                                        new TestPredictor(&tpm, &tle, GSAPConfigMap())),
                                    src);
        Assert::AreEqual(0,
                         listener.getCount(),
                         "Predictor produced prediction during construction");

        // First set of messages initializes observer
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput0, src, 0.0)));
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput1, src, 0.0)));
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestOutput0, src, 0.0)));
        Assert::AreEqual(0,
                         listener.getCount(),
                         "Predictor produced prediction after one set of data");

        // Second set of messages should trigger a state estimate and prediction
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput0, src, 0.0)));
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput1, src, 0.0)));
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestOutput0, src, 0.0)));
        Assert::AreEqual(1, listener.getCount(), "Predictor didn't produce prediction");
    }
}
