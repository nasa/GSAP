// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <thread>
#include <utility>

#include "DynamicArray.h"
#include "Messages/DoubleMessage.h"
#include "Messages/MessageBus.h"
#include "Messages/MessageWatcher.h"
#include "MockClasses.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace MessageWatcherTests {
    static const auto PUBLISH_DELAY = std::chrono::milliseconds(5);

    void constructor() {
        MessageBus bus;
        const std::string src = "test";
        const std::vector<MessageId> ids = {MessageId::TestInput0, MessageId::TestInput1};
        DynamicArray<double> values(ids.size());

        MessageWatcher<DynamicArray<double>> watcher(bus,
                                                     src,
                                                     ids,
                                                     MessageId::ModelInputVector,
                                                     values);

        try {
            DynamicArray<double> values2(ids.size() + 1);
            MessageWatcher<DynamicArray<double>> watcher2(bus,
                                                          src,
                                                          ids,
                                                          MessageId::ModelInputVector,
                                                          values2);
            Assert::Fail("No precondition check for matching id and value sizes");
        }
        catch (const AssertException&) {
        }
    }

    void publish() {
        MessageBus bus;
        const std::string src = "test";
        const std::vector<MessageId> ids = {MessageId::TestInput0, MessageId::TestInput1};
        MessageId resultId = MessageId::ModelInputVector;
        MessageCounter counter(bus, src, resultId);
        std::vector<double> values(ids.size());

        MessageWatcher<std::vector<double>> watcher(bus, src, ids, resultId, values);

        Assert::AreEqual(0, counter.getCount(), "No data");
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput0, src, 0.0)));
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
        Assert::AreEqual(0, counter.getCount(), "1 input");
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput1, src, 0.0)));
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
        Assert::AreEqual(1, counter.getCount(), "Both inputs");

        auto msg =
            dynamic_cast<TemplateMessage<std::vector<double>>*>(counter.getLastMessage().get());
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
        std::vector<double> values(ids.size());

        MessageWatcher<std::vector<double>> watcher(bus, src, ids, resultId, values);

        Assert::AreEqual(0, counter.getCount(), "No data");
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput0, src, 1.0)));
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
        Assert::AreEqual(0, counter.getCount(), "Input0 first value");
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput0, src, 2.0)));
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
        Assert::AreEqual(0, counter.getCount(), "Input0 second value");
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput1, src, 3.0)));
        std::this_thread::sleep_for(std::chrono::milliseconds(PUBLISH_DELAY));
        Assert::AreEqual(1, counter.getCount(), "1 message per complete set");

        auto msg =
            dynamic_cast<TemplateMessage<std::vector<double>>*>(counter.getLastMessage().get());
        const auto& msgValues = msg->getValue();
        Assert::AreEqual(2, msgValues.size(), "Watcher message size");
        Assert::AreEqual(2.0, msgValues[0], 1e-15, "Watcher message value 0");
        Assert::AreEqual(3.0, msgValues[1], 1e-15, "Watcher message value 1");
    }
}
