// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <algorithm>
#include <future>
#include <vector>

#include "Messages/MessageBus.h"
#include "ThreadSafeLog.h"

namespace PCOE {
    static const Log& log = Log::Instance();
    static const std::string MODULE_NAME = "M-BUS";

    void MessageBus::processOne() {
        unique_lock lock(m);
        log.FormatLine(LOG_TRACE,
                       MODULE_NAME,
                       "Processing one message. %u in queue.",
                       futures.size());
        if (!futures.empty()) {
            std::future<void> f = std::move(futures.back());
            futures.pop_back();

            // Note (JW): Unlock before calling get because the mutex is not
            //            recursive and processing one message often leads to
            //            the publishing of further messages.
            lock.unlock();
            f.get();
            log.WriteLine(LOG_TRACE, MODULE_NAME, "Message processed");
        }
    }

    void MessageBus::processAll() {
        unique_lock lock(m);
        log.FormatLine(LOG_TRACE,
                       MODULE_NAME,
                       "Processing all messages. Starting with %u in queue.",
                       futures.size());
        while (!futures.empty()) {
            std::future<void> f = std::move(futures.back());
            futures.pop_back();

            // Note (JW): Unlock before calling get because the mutex is not
            //            recursive and processing one message often leads to
            //            the publishing of further messages.
            lock.unlock();
            f.get();
            log.FormatLine(LOG_TRACE,
                           MODULE_NAME,
                           "Processed message. %u left in queue.",
                           futures.size());
            lock.lock();
        }
    }

    void MessageBus::subscribe(IMessageProcessor* consumer, std::string source, MessageId id) {
        lock_guard guard(m);
        log.FormatLine(LOG_TRACE,
                       MODULE_NAME,
                       "Adding subscriber %x for source '%s' and id %x",
                       consumer,
                       source.c_str(),
                       static_cast<std::uint64_t>(id));
        subscribers[source].push_back(callback_pair(id, consumer));
    }

    void MessageBus::unsubscribe(IMessageProcessor* consumer) {
        lock_guard guard(m);
        log.FormatLine(LOG_TRACE, MODULE_NAME, "Removing subscriber %x", consumer);
        for (auto i : subscribers) {
            unsubscribe(consumer, i.first);
        }
    }

    void MessageBus::unsubscribe(IMessageProcessor* consumer, const std::string& source) {
        lock_guard guard(m);
        log.FormatLine(LOG_TRACE,
                       MODULE_NAME,
                       "Adding subscriber %x for source '%s'",
                       consumer,
                       source.c_str());
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

    static bool future_ready(std::future<void>& f) {
        // Note (JW): Only calls to future.get trigger exception propagation, so
        //            first we do a 0 wait, then if the future is ready we call
        //            get, even though the return type is void, to trigger any
        //            exceptions that may have occured. We do this here because
        //            ready futures will be discarded immediately following this
        //            call in the `publish` function below and discarding the
        //            future without calling get will discard any stored
        //            exceptions along with it.
        bool ready = f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
        if (ready) {
            f.get();
        }
        return ready;
    }

    void MessageBus::publish(std::shared_ptr<Message> message) {
        lock_guard guard(m);
        log.FormatLine(LOG_TRACE,
                       MODULE_NAME,
                       "Publishing message from source %s with id %x",
                       message->getSource().c_str(),
                       static_cast<std::uint64_t>(message->getMessageId()));
        auto srcSubs = subscribers.find(message->getSource());
        if (srcSubs == subscribers.cend()) {
            log.WriteLine(LOG_TRACE, MODULE_NAME, "No subscribers");
            return;
        }

        for (auto it : (*srcSubs).second) {
            if (it.first == MessageId::All || it.first == message->getMessageId()) {
                log.FormatLine(LOG_TRACE,
                               MODULE_NAME,
                               "Creating future for subscriber %x",
                               it.second);
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
