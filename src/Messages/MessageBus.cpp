// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <algorithm>
#include <future>
#include <vector>

#include "Messages/MessageBus.h"

namespace PCOE {
    void MessageBus::processOne() {
        unique_lock lock(m);
        if (!futures.empty()) {
            std::future<void> f = std::move(futures.back());
            futures.pop_back();

            // Note (JW): Unlock before calling get because the mutex is not
            //            recursive and processing one message often leads to
            //            the publishing of further messages.
            lock.unlock();
            f.get();
        }
    }

    void MessageBus::processAll() {
        unique_lock lock(m);
        while (!futures.empty()) {
            std::future<void> f = std::move(futures.back());
            futures.pop_back();

            // Note (JW): Unlock before calling get because the mutex is not
            //            recursive and processing one message often leads to
            //            the publishing of further messages.
            lock.unlock();
            f.get();
            lock.lock();
        }
    }

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

    bool future_ready(std::future<void>& f) {
        return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }

    void MessageBus::publish(std::shared_ptr<Message> message) {
        lock_guard guard(m);
        auto srcSubs = subscribers.find(message->getSource());
        if (srcSubs == subscribers.cend()) {
            return;
        }

        for (auto it : (*srcSubs).second) {
            if (it.first == MessageId::All || it.first == message->getMessageId()) {
                auto f = std::async(launchPolicy,
                                    &IMessageProcessor::processMessage,
                                    it.second,
                                    message);
                futures.push_back(std::move(f));
            }
        }

        futures.erase(std::remove_if(futures.begin(), futures.end(), future_ready), futures.end());
    }
}
