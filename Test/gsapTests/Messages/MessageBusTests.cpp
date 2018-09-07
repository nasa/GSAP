// Copyright (c) 2018 United States Government as represented by the
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
    std::uint16_t getPayloadSize() const override {
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
    static const auto PUBLISH_DELAY = std::chrono::milliseconds(5);

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
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
        Assert::AreEqual(2, consumer.msgCount, "Consumer got the wrong number of messages");
    }

    void subscribe() {
        MessageBus bus;
        TestMessageProcesor consumer;

        bus.subscribe(&consumer, "test", MessageId::TestInput0);

        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput1, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "Other")));
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
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
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
        Assert::AreEqual(2, consumer.msgCount, "Consumer got the wrong number of messages");
    }

    void unsubscribe() {
        MessageBus bus;
        TestMessageProcesor consumer;

        bus.subscribe(&consumer, "test");

        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput1, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "Other")));
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
        Assert::AreEqual(2, consumer.msgCount, "Consumer got the wrong number of messages");

        bus.unsubscribe(&consumer);

        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput1, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "Other")));
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
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
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
        Assert::AreEqual(3, consumer.msgCount, "Consumer got the wrong number of messages");

        bus.unsubscribe(&consumer, "test");

        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput1, "test")));
        bus.publish(std::shared_ptr<Message>(new TestMessage(MessageId::TestInput0, "Other")));
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
        Assert::AreEqual(4, consumer.msgCount, "Consumer got the wrong number of messages");
    }
}
