// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <thread>
#include <utility>

#include "MockClasses.h"
#include "Observers/EventDrivenObserver.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace EventDrivenObserverTests {
    static const auto PUBLISH_DELAY = std::chrono::milliseconds(5);

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

        MessageCounter listener(bus, src, MessageId::ModelStateEstimate);
        EventDrivenObserver edObs(bus, std::unique_ptr<Observer>(new TestObserver(&tm)), src);

        Assert::AreEqual(0, listener.getCount(), "obs produced state estimate on construction");
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput0, src, 0.0)));
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
        Assert::AreEqual(0, listener.getCount(), "obs produced state estimate before init (1)");
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput1, src, 0.0)));
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
        Assert::AreEqual(0, listener.getCount(), "obs produced state estimate before init (2)");
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestOutput0, src, 0.0)));
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
        Assert::AreEqual(0,
                         listener.getCount(),
                         "obs produced state estimate after first set of data");

        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput0, src, 0.0)));
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
        Assert::AreEqual(0, listener.getCount(), "obs produced state estimate on 1 input");
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput1, src, 0.0)));
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
        Assert::AreEqual(0, listener.getCount(), "obs produced state estimate on 2 inputs");
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestOutput0, src, 0.0)));
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
        Assert::AreEqual(1,
                         listener.getCount(),
                         "obs didn't produce state estimate after two sets of data");
    }
}
