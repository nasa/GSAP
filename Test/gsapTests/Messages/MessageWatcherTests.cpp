// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <utility>

#include "DynamicArray.h"
#include "Messages/DoubleMessage.h"
#include "Messages/MessageBus.h"
#include "Messages/MessageWatcher.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace MessageWatcherTests {
    void constructor() {
        MessageBus bus;
        const std::string src = "test";
        const std::vector<MessageId> ids = {MessageId::TestInput0, MessageId::TestInput1};
        DynamicArray<double> values(ids.size());

        MessageWatcher<DynamicArray<double>> watcher(bus, src, ids, values);

        try {
            DynamicArray<double> values2(ids.size() + 1);
            MessageWatcher<DynamicArray<double>> watcher2(bus, src, ids, values2);
            Assert::Fail("No precondition check for matching id and value sizes");
        }
        catch (const AssertException&) {
        }
    }

    void allPresent() {
        MessageBus bus;
        const std::string src = "test";
        const std::vector<MessageId> ids = {MessageId::TestInput0, MessageId::TestInput1};
        std::vector<double> values(ids.size());

        MessageWatcher<std::vector<double>> watcher(bus, src, ids, values);

        Assert::IsFalse(watcher.allPresent(), "No data");
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput0, src, 0.0)));
        Assert::IsFalse(watcher.allPresent(), "1 input");
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput1, src, 0.0)));
        Assert::IsTrue(watcher.allPresent(), "Both inputs");
    }

    void reset() {
        MessageBus bus;
        const std::string src = "test";
        const std::vector<MessageId> ids = {MessageId::TestInput0, MessageId::TestInput1};
        std::vector<double> values(ids.size());

        MessageWatcher<std::vector<double>> watcher(bus, src, ids, values);

        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput0, src, 0.0)));
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput1, src, 0.0)));
        Assert::IsTrue(watcher.allPresent(), "All present");

        watcher.reset();
        Assert::IsFalse(watcher.allPresent(), "After reset");
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput0, src, 0.0)));
        Assert::IsFalse(watcher.allPresent(), "1 input after reset");
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput1, src, 0.0)));
        Assert::IsTrue(watcher.allPresent(), "Both inputs after reset");
    }

    void getValues() {
        MessageBus bus;
        const std::string src = "test";
        const std::vector<MessageId> ids = {MessageId::TestInput0, MessageId::TestInput1};
        std::vector<double> values(ids.size());

        MessageWatcher<std::vector<double>> watcher(bus, src, ids, values);
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput0, src, 42.0)));
        bus.publish(std::shared_ptr<Message>(new DoubleMessage(MessageId::TestInput1, src, 97.0)));
        Assert::IsTrue(watcher.allPresent(), "All present");

        const auto& watcherValues = watcher.getValues();
        Assert::AreEqual(42.0, watcherValues[0], 1e-6, "Value 1");
        Assert::AreEqual(97.0, watcherValues[1], 1e-6, "Value 2");
    }
}
