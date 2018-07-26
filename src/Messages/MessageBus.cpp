// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <algorithm>

#include "Messages/MessageBus.h"

namespace PCOE {
    void MessageBus::subscribe(IMessageProcessor* consumer, std::string source, MessageId id) {
        lock_guard guard(m);
        subscribers[source].push_back(callback_pair(id, consumer));
    }

    void MessageBus::unsubscribe(IMessageProcessor* consumer) {
        lock_guard guard(m);
        for (auto i : subscribers) {
            unsubscribe(consumer, i.first);
        }
    }

    void MessageBus::unsubscribe(IMessageProcessor* consumer, const std::string& source) {
        lock_guard guard(m);
        auto srcSubs = subscribers.find(source);
        if (srcSubs == subscribers.cend()) {
            return;
        }
        auto& vec = (*srcSubs).second;

        vec.erase(std::remove_if(vec.begin(),
                                 vec.end(),
                                 [consumer](const callback_pair& i) {
                                     return i.second == consumer;
                                 }),
                  vec.end());
    }

    void MessageBus::publish(std::shared_ptr<Message> message) const {
        lock_guard guard(m);
        auto srcSubs = subscribers.find(message->getSource());
        if (srcSubs == subscribers.cend()) {
            return;
        }

        for (auto it : (*srcSubs).second) {
            if (it.first == MessageId::All || it.first == message->getMessageId()) {
                // TODO (JW): Process messages asynchronously
                it.second->processMessage(message);
            }
        }
    }
}
