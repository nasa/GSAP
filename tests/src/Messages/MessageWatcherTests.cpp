// Copyright (c) 2018-2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <thread>
#include <utility>

#include "DynamicArray.h"
#include "Messages/MessageBus.h"
#include "Messages/MessageWatcher.h"
#include "Messages/ScalarMessage.h"
#include "Messages/VectorMessage.h"
#include "MockClasses.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace MessageWatcherTests {
    void constructor() {
        MessageBus bus;
        const std::string src = "test";
        const std::vector<MessageId> ids = {MessageId::TestInput0, MessageId::TestInput1};
        DynamicArray<double> values(ids.size());

        MessageWatcher<double> watcher(bus, src, ids, MessageId::ModelInputVector);
    }

    void publish() {
        MessageBus bus;
        const std::string src = "test";
        const std::vector<MessageId> ids = {MessageId::TestInput0, MessageId::TestInput1};
        MessageId resultId = MessageId::ModelInputVector;
        MessageCounter counter(bus, src, resultId);

        MessageWatcher<double> watcher(bus, src, ids, resultId);

        Assert::AreEqual(0, counter.getCount(), "No data");
        bus.publish(std::shared_ptr<Message>(
            new DoubleMessage(MessageId::TestInput0, src, MessageClock::now(), 0.0)));
        bus.waitAll();
        Assert::AreEqual(0, counter.getCount(), "1 input");
        bus.publish(std::shared_ptr<Message>(
            new DoubleMessage(MessageId::TestInput1, src, MessageClock::now(), 0.0)));
        bus.waitAll();
        Assert::AreEqual(1, counter.getCount(), "Both inputs");

        auto msg = dynamic_cast<VectorMessage<double>*>(counter.getLastMessage().get());
        const auto& msgValues = msg->getValue();
        Assert::AreEqual(2, msgValues.size(), "Watcher message size");
        Assert::AreEqual(0.0, msgValues[0], 1e-15, "Watcher message value 0");
        Assert::AreEqual(0.0, msgValues[1], 1e-15, "Watcher message value 1");
    }

    void messageCount() {
        MessageBus bus;
        const std::string src = "test";
        const std::vector<MessageId> ids = {MessageId::TestInput0, MessageId::TestInput1};
        MessageId resultId = MessageId::ModelInputVector;
        MessageCounter counter(bus, src, resultId);

        MessageWatcher<double> watcher(bus, src, ids, resultId);

        Assert::AreEqual(0, counter.getCount(), "No data");
        bus.publish(std::shared_ptr<Message>(
            new DoubleMessage(MessageId::TestInput0, src, MessageClock::now(), 1.0)));
        bus.waitAll();
        Assert::AreEqual(0, counter.getCount(), "Input0 first value");
        bus.publish(std::shared_ptr<Message>(
            new DoubleMessage(MessageId::TestInput0, src, MessageClock::now(), 2.0)));
        bus.waitAll();
        Assert::AreEqual(0, counter.getCount(), "Input0 second value");
        bus.publish(std::shared_ptr<Message>(
            new DoubleMessage(MessageId::TestInput1, src, MessageClock::now(), 3.0)));
        bus.waitAll();
        Assert::AreEqual(1, counter.getCount(), "1 message per complete set");

        auto msg = dynamic_cast<VectorMessage<double>*>(counter.getLastMessage().get());
        const auto& msgValues = msg->getValue();
        Assert::AreEqual(2, msgValues.size(), "Watcher message size");
        Assert::AreEqual(2.0, msgValues[0], 1e-15, "Watcher message value 0");
        Assert::AreEqual(3.0, msgValues[1], 1e-15, "Watcher message value 1");
    }
    
    void registerTests(TestContext& context) {
        context.AddTest("Construct", MessageWatcherTests::constructor, "MessageWatcher");
        context.AddTest("Publish", MessageWatcherTests::publish, "MessageWatcher");
        context.AddTest("Message Count", MessageWatcherTests::messageCount, "MessageWatcher");
    }
}
