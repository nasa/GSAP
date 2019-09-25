// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <algorithm>
#include <future>
#include <vector>

#include "Contracts.h"
#include "Messages/MessageBus.h"
#include "ThreadSafeLog.h"

namespace PCOE {
    static const Log& log = Log::Instance();
    static const std::string MODULE_NAME = "M-BUS";

    void MessageBus::wait() {
        std::future<void> f = dequeue();
        Require(f.valid(), "Invalid future in queue");
        f.get();
    }

    void MessageBus::waitAll() {
        bool valid = false;
        do {
            std::future<void> f = try_dequeue_for(std::chrono::milliseconds(0));
            valid = f.valid();
            if (valid) {
                f.get();
            }
        } while (valid);
    }

    void MessageBus::subscribe(IMessageProcessor* consumer, std::string source, MessageId id) {
        std::lock_guard<std::recursive_mutex> guard(subs_mutex);
        log.FormatLine(LOG_TRACE,
                       MODULE_NAME,
                       "Adding subscriber %x for source '%s' and id %x",
                       consumer,
                       source.c_str(),
                       static_cast<std::uint64_t>(id));
        subscribers[source].push_back(callback_pair(id, consumer));
    }

    void MessageBus::unsubscribe(IMessageProcessor* consumer) {
        std::lock_guard<std::recursive_mutex> guard(subs_mutex);
        log.FormatLine(LOG_TRACE, MODULE_NAME, "Removing subscriber %x", consumer);
        for (auto i : subscribers) {
            unsubscribe(consumer, i.first);
        }
    }

    void MessageBus::unsubscribe(IMessageProcessor* consumer, const std::string& source) {
        std::lock_guard<std::recursive_mutex> guard(subs_mutex);
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

    void MessageBus::publish(std::shared_ptr<Message> message) {
        std::unique_lock<std::recursive_mutex> lock(subs_mutex);
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
                enqueue(std::async(launchPolicy,
                                   &IMessageProcessor::processMessage,
                                   it.second,
                                   message));
            }
        }

        lock.unlock(); // No need to hold the subs lock while pruning callback queue
        clear_completed();
    }

    static bool future_ready(std::future<void>& f) {
        // Note (JW): Only calls to future.get trigger exception propagation, so
        //            first we do a 0 wait, then if the future is ready we call
        //            get, even though the return type is void, to trigger any
        //            exceptions that may have occured. We do this here because
        //            ready futures will be discarded immediately following this
        //            call in the `clear_completed` function below and
        //            discarding the future without calling get will discard any
        //            stored exceptions along with it.
        Expect(f.valid(), "Invalid future in queue");
        bool ready = f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
        if (ready) {
            f.get();
        }
        return ready;
    }

    void MessageBus::clear_completed() {
        std::lock_guard<std::mutex> lock(queue_mutex);
        queue.erase(std::remove_if(queue.begin(), queue.end(), future_ready), queue.end());
    }

    void MessageBus::enqueue(std::future<void> msg) {
        // Note (JW): Unlock before notify to prevent the case where another
        // thread is waiting, receives a notification, and is imediately
        // blocked because this thread is still holding the lock.
        std::unique_lock<std::mutex> lock(queue_mutex);
        queue.push_back(std::move(msg));
        lock.unlock();

        queue_cv.notify_one();
    }

    std::future<void> MessageBus::dequeue() {
        std::unique_lock<std::mutex> lock(queue_mutex);
        queue_cv.wait(lock, [&]() { return !queue.empty(); });

        std::future<void> msg = std::move(queue.front());
        queue.pop_front();
        return msg;
    }
}
