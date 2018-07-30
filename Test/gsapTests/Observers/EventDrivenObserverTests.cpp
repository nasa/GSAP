// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <utility>

#include "MockClasses.h"
#include "Observers/EventDrivenObserver.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace EventDrivenObserverTests {
    class ObserverListener final : public IMessageProcessor {
    public:
        ObserverListener(MessageBus& bus, std::string src) : source(std::move(src)) {
            bus.subscribe(this, source, MessageId::ModelStateEstimate);
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
        TestModel tm;
        const std::string src = "test";

        EventDrivenObserver edObs(bus, std::unique_ptr<Observer>(new TestObserver(&tm)), src);
        // Constructed without exception
    }

    void processMessage() {
        MessageBus bus;
        TestModel tm;
        const std::string src = "test";

        ObserverListener listener(bus, src);
        EventDrivenObserver edObs(bus, std::unique_ptr<Observer>(new TestObserver(&tm)), src);

        Assert::AreEqual(0, listener.getCount(), "obs produced state estimate on construction");
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput0, src, 0.0)));
        Assert::AreEqual(0, listener.getCount(), "obs produced state estimate before init (1)");
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput1, src, 0.0)));
        Assert::AreEqual(0, listener.getCount(), "obs produced state estimate before init (2)");
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestOutput0, src, 0.0)));
        Assert::AreEqual(0,
                         listener.getCount(),
                         "obs produced state estimate after first set of data");

        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput0, src, 0.0)));
        Assert::AreEqual(0, listener.getCount(), "obs produced state estimate on 1 input");
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput1, src, 0.0)));
        Assert::AreEqual(0, listener.getCount(), "obs produced state estimate on 2 inputs");
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestOutput0, src, 0.0)));
        Assert::AreEqual(1,
                         listener.getCount(),
                         "obs didn't produce state estimate after two sets of data");
    }
}
