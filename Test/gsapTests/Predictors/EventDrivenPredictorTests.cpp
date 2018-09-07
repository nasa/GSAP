// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <thread>

#include "MockClasses.h"
#include "Observers/EventDrivenObserver.h"
#include "Predictors/EventDrivenPredictor.h"
#include "Test.h"

#include "ConfigMap.h"
#include "Contracts.h"
#include "Loading/LoadEstimatorFactory.h"
#include "Messages/Message.h"
#include "Messages/MessageId.h"
#include "Messages/ScalarMessage.h"
#include "Models/PrognosticsModelFactory.h"
#include "Observers/Observer.h"
#include "Observers/ObserverFactory.h"
#include "Predictors/PredictorFactory.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace EventDrivenPredictorTests {
    class PowerConverter final : public IMessageProcessor {
    public:
        PowerConverter(MessageBus& bus, const ConfigMap&, const std::string& source) : bus(bus) {
            bus.subscribe(this, source, MessageId::Volts);
            bus.subscribe(this, source, MessageId::Watts);
        }

        void processMessage(const std::shared_ptr<Message>& message) override {
            lock_guard guard(m);
            switch (message->getMessageId()) {
            case MessageId::Volts: {
                DoubleMessage* vmsg = dynamic_cast<DoubleMessage*>(message.get());
                volts = vmsg->getValue();
                break;
            }
            case MessageId::Watts: {
                DoubleMessage* wmsg = dynamic_cast<DoubleMessage*>(message.get());
                double current = wmsg->getValue() / volts;
                auto imsg = std::shared_ptr<Message>(new DoubleMessage(MessageId::Amperes,
                                                                       message->getSource(),
                                                                       message->getTimestamp(),
                                                                       current));
                bus.publish(imsg);
                break;
            }
            default:
                Unreachable("Unexpected message id");
            }
        }

    private:
        using mutex = std::mutex;
        using lock_guard = std::lock_guard<mutex>;

        mutable mutex m;
        MessageBus& bus;
        double volts;
    };

    void constructor() {
        MessageBus bus;
        TestPrognosticsModel tpm;
        TestLoadEstimator tle;
        const std::string src = "test";

        TrajectoryService trajService;

        EventDrivenPredictor edPred(bus,
                                    std::unique_ptr<Predictor>(
                                        new TestPredictor(tpm, tle, trajService, ConfigMap())),
                                    src);
        // Constructed without exception
    }

    void processMessage() {
        MessageBus bus;

        TestPrognosticsModel tpm;
        TestLoadEstimator tle;
        TrajectoryService ts;
        const std::string src = "test";

        TrajectoryService trajService;

        MessageCounter listener(bus, src, MessageId::TestEvent0);
        EventDrivenObserver edObs(bus, std::unique_ptr<Observer>(new TestObserver(tpm)), src);
        EventDrivenPredictor edPred(bus,
                                    std::unique_ptr<Predictor>(
                                        new TestPredictor(tpm, tle, trajService, ConfigMap())),
                                    src);
        Assert::AreEqual(0,
                         listener.getCount(),
                         "Predictor produced prediction during construction");

        // First set of messages initializes observer
        auto timestamp = MessageClock::time_point(MessageClock::duration(1000));
        bus.publish(std::shared_ptr<Message>(
            new DoubleMessage(MessageId::TestInput0, src, timestamp, 0.0)));
        bus.publish(std::shared_ptr<Message>(
            new DoubleMessage(MessageId::TestInput1, src, timestamp, 0.0)));
        bus.publish(std::shared_ptr<Message>(
            new DoubleMessage(MessageId::TestOutput0, src, timestamp, 0.0)));
        bus.processAll();
        Assert::AreEqual(0,
                         listener.getCount(),
                         "Predictor produced prediction after one set of data");

        // Second set of messages should trigger a state estimate and prediction
        bus.publish(std::shared_ptr<Message>(
            new DoubleMessage(MessageId::TestInput0, src, timestamp, 0.0)));
        bus.publish(std::shared_ptr<Message>(
            new DoubleMessage(MessageId::TestInput1, src, timestamp, 0.0)));
        bus.publish(std::shared_ptr<Message>(
            new DoubleMessage(MessageId::TestOutput0, src, timestamp, 0.0)));
        bus.processAll();
        Assert::AreEqual(1, listener.getCount(), "Predictor didn't produce prediction");
        auto msg = listener.getLastMessage();
        auto predictionTimestamp = msg->getTimestamp();
        Assert::AreEqual(timestamp, predictionTimestamp, "Unexpected prediction timestamp");

        timestamp = MessageClock::now();
        bus.publish(std::shared_ptr<Message>(
            new DoubleMessage(MessageId::TestInput0, src, timestamp, 0.0)));
        bus.publish(std::shared_ptr<Message>(
            new DoubleMessage(MessageId::TestInput1, src, timestamp, 0.0)));
        bus.publish(std::shared_ptr<Message>(
            new DoubleMessage(MessageId::TestOutput0, src, timestamp, 0.0)));
        bus.processAll();
        Assert::AreEqual(2, listener.getCount(), "Predictor didn't produce 2nd prediction");
        msg = listener.getLastMessage();
        predictionTimestamp = msg->getTimestamp();
        Assert::AreEqual(timestamp, predictionTimestamp, "Unexpected 2nd prediction timestamp");
    }

    void fullConfig() {
        static Log& log = Log::Instance();
        log.SetVerbosity(LOG_TRACE);
        std::string src = "3701";
        MessageBus bus(std::launch::deferred);
        MessageCounter listener(bus, src, MessageId::BatteryEod);
        ConfigMap config;
        config.set("model", "Battery");
        config.set("Model.ProcessNoise",
                   {"1e-5", "1e-5", "1e-5", "1e-5", "1e-5", "1e-5", "1e-5", "1e-5"});
        config.set("observer", "UKF");
        config.set("Observer.Q",
                   {"1e-10",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "1e-10",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "1e-10",
                    "0",
                    "0",
                    "0",
                    "0",
                    ""
                    "0",
                    "0",
                    "0",
                    "0",
                    "1e-10",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "1e-10",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    ""
                    "1e-10",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "1e-10",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "1e-10"});
        config.set("Observer.R", {"1e-2", "0", "0", "1e-2"});
        config.set("predictor", "MC");
        config.set("Predictor.SampleCount", "100");
        config.set("Predictor.Horizon", "100");
        config.set("LoadEstimator.Loading", "8");

        std::unique_ptr<PrognosticsModel> model;
        std::unique_ptr<LoadEstimator> loadEstimator;
        std::unique_ptr<Observer> observer;
        std::unique_ptr<Predictor> predictor;
        std::vector<std::unique_ptr<IMessageProcessor>> eventListeners;
        TrajectoryService trajService;

        auto& mfactory = PrognosticsModelFactory::instance();
        model = mfactory.Create("Battery", config);
        eventListeners.push_back(
            std::unique_ptr<IMessageProcessor>(new PowerConverter(bus, config, src)));

        auto& lefactory = LoadEstimatorFactory::instance();
        loadEstimator = lefactory.Create("Const", config);

        auto& ofactory = ObserverFactory::instance();
        observer = ofactory.Create("UKF", *model, config);

        auto& pfactory = PredictorFactory::instance();
        predictor = pfactory.Create("MC", *model, *loadEstimator, trajService, config);

        auto edObs = observer ? std::unique_ptr<EventDrivenObserver>(
                                    new EventDrivenObserver(bus, std::move(observer), src))
                              : nullptr;

        auto edPred = predictor ? std::unique_ptr<EventDrivenPredictor>(
                                      new EventDrivenPredictor(bus, std::move(predictor), src))
                                : nullptr;

        DoubleMessage* dm0 =
            new DoubleMessage(MessageId::Volts,
                              src,
                              MessageClock::time_point(MessageClock::duration(1535391267115000)),
                              12.2);
        DoubleMessage* dm1 =
            new DoubleMessage(MessageId::Watts,
                              src,
                              MessageClock::time_point(MessageClock::duration(1535391267115000)),
                              2);
        DoubleMessage* dm2 =
            new DoubleMessage(MessageId::Centigrade,
                              src,
                              MessageClock::time_point(MessageClock::duration(1535391267115000)),
                              20.0);

        DoubleMessage* dm3 =
            new DoubleMessage(MessageId::Volts,
                              src,
                              MessageClock::time_point(MessageClock::duration(1535391268115000)),
                              12.2);
        DoubleMessage* dm4 =
            new DoubleMessage(MessageId::Watts,
                              src,
                              MessageClock::time_point(MessageClock::duration(1535391268115000)),
                              2);
        DoubleMessage* dm5 =
            new DoubleMessage(MessageId::Centigrade,
                              src,
                              MessageClock::time_point(MessageClock::duration(1535391268115000)),
                              20.0);

        bus.publish(std::shared_ptr<Message>(dm0));
        bus.processAll();
        bus.publish(std::shared_ptr<Message>(dm1));
        bus.processAll();
        bus.publish(std::shared_ptr<Message>(dm2));
        bus.processAll();
        bus.publish(std::shared_ptr<Message>(dm3));
        bus.processAll();
        bus.publish(std::shared_ptr<Message>(dm4));
        bus.processAll();
        bus.publish(std::shared_ptr<Message>(dm5));
        bus.processAll();
        Assert::AreEqual(1, listener.getCount(), "Predictor didn't produce prediction");
    }
}
