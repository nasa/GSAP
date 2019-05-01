// Copyright (c) 2018-2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <thread>
#include <utility>

#include "Messages/IMessageProcessor.h"
#include "Messages/MessageBus.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

class TestMessage final : public Message {
public:
    TestMessage(MessageId id, const std::string& source)
        : Message(id, source, MessageClock::now()) {}

protected:
    std::uint32_t getPayloadSize() const override {
        return 0;
    }

    void serializePayload(std::ostream&) const override {}
};

class TestMessageProcesor final : public IMessageProcessor {
public:
    void processMessage(const std::shared_ptr<Message>&) {
        ++msgCount;
    }

    int msgCount = 0;
};

namespace MessageBusTests {
    void constructor() {
        MessageBus bus; // Default construct without exception
    }

    void publish() {
        MessageBus bus;

        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "test")));
    }

    void subscribeAll() {
        MessageBus bus;
        TestMessageProcesor consumer;

        bus.subscribe(&consumer, "test");

        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput1, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "Other")));
        bus.waitAll();
        Assert::AreEqual(2, consumer.msgCount, "Consumer got the wrong number of messages");
    }

    void subscribe() {
        MessageBus bus;
        TestMessageProcesor consumer;

        bus.subscribe(&consumer, "test", MessageId::TestInput0);

        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput1, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "Other")));
        bus.waitAll();
        Assert::AreEqual(1, consumer.msgCount, "Consumer got the wrong number of messages");
    }

    void subscribeMultiSource() {
        MessageBus bus;
        TestMessageProcesor consumer;

        bus.subscribe(&consumer, "test", MessageId::TestInput0);
        bus.subscribe(&consumer, "Other", MessageId::TestInput0);

        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput1, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "Other")));
        bus.waitAll();
        Assert::AreEqual(2, consumer.msgCount, "Consumer got the wrong number of messages");
    }

    void unsubscribe() {
        MessageBus bus;
        TestMessageProcesor consumer;

        bus.subscribe(&consumer, "test");

        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput1, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "Other")));
        bus.waitAll();
        Assert::AreEqual(2, consumer.msgCount, "Consumer got the wrong number of messages");

        bus.unsubscribe(&consumer);

        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput1, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "Other")));
        bus.waitAll();
        Assert::AreEqual(2, consumer.msgCount, "Consumer got the wrong number of messages");
    }

    void unsubscribePartial() {
        MessageBus bus;
        TestMessageProcesor consumer;

        bus.subscribe(&consumer, "test");
        bus.subscribe(&consumer, "Other", MessageId::TestInput0);

        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput1, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "Other")));
        bus.waitAll();
        Assert::AreEqual(3, consumer.msgCount, "Consumer got the wrong number of messages");

        bus.unsubscribe(&consumer, "test");

        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput1, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "Other")));
        bus.waitAll();
        Assert::AreEqual(4, consumer.msgCount, "Consumer got the wrong number of messages");
    }

    void waitForTimeouts() {
        using namespace std::chrono;
        MessageBus bus;

        auto start = high_resolution_clock::now();
        bus.waitFor(milliseconds(2));
        auto stop = high_resolution_clock::now();
        std::chrono::nanoseconds diff = stop - start;
        Assert::IsTrue(diff > milliseconds(2), "waitFor min");
        // Note (JW): Technically, the maximum is unbounded (once we block, we
        // are at the mercy of the OS scheduler), but I think if we don't return
        // within 100ms something is probably wrong.
        Assert::IsTrue(diff < milliseconds(100), "waitFor max");

        TestMessageProcesor consumer;
    }

    void waitUntilTimeouts() {
        using namespace std::chrono;
        MessageBus bus;

        auto start = high_resolution_clock::now();
        bus.waitUntil(start + milliseconds(2));
        auto stop = high_resolution_clock::now();
        std::chrono::nanoseconds diff = stop - start;
        Assert::IsTrue(diff > milliseconds(2), "waitFor min");
        // Note (JW): Technically, the maximum is unbounded (once we block, we
        // are at the mercy of the OS scheduler), but I think if we don't return
        // within 100ms something is probably wrong.
        Assert::IsTrue(diff < milliseconds(100), "waitFor max");

        TestMessageProcesor consumer;
    }

    void registerTests(TestContext& context) {
        context.AddTest("construct", MessageBusTests::constructor, "MessageBus");
        context.AddTest("publish", MessageBusTests::publish, "MessageBus");
        context.AddTest("subscribeAll", MessageBusTests::subscribeAll, "MessageBus");
        context.AddTest("subscribeMultiSource",
                        MessageBusTests::subscribeMultiSource,
                        "MessageBus");
        context.AddTest("subscribe", MessageBusTests::subscribe, "MessageBus");
        context.AddTest("unsubscribe", MessageBusTests::unsubscribe, "MessageBus");
        context.AddTest("unsubscribePartial", MessageBusTests::unsubscribePartial, "MessageBus");
        context.AddTest("waitForTimeouts", MessageBusTests::waitForTimeouts, "MessageBus");
        context.AddTest("waitUntilTimeouts", MessageBusTests::waitUntilTimeouts, "MessageBus");
    }
}
