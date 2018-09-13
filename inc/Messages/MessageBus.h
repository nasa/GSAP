// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGES_MESSAGEBUS_H
#define PCOE_MESSAGES_MESSAGEBUS_H
#include <condition_variable>
#include <deque>
#include <future>
#include <mutex>
#include <unordered_map>

#include "Messages/IMessagePublisher.h"

namespace PCOE {
    /**
     * A collection of message subcribers that receive messages based on their
     * message Id's.
     *
     * @remarks
     * For each message that is published, the set of subscribers for that
     * message is determined, and each subscriber in that set is called in its
     * own {@code std::async} instance. The behavior of those instances is
     * controlled by the launch policy specified when the message bus is
     * constructed.
     *
     * @remarks
     * By default, the {@code std::launch::async} policy is specified, which
     * launches each {@code std::async} instance on its own thread. In this
     * mode, no active management of the message bus is required, as message
     * procesing will proceed automatically on the threads spawned by
     * {@code std::async}, and the message bus will automatically clean up the
     * completed futures.
     *
     * @remarks
     * If {@code std::launch::defered} is specified (either alone or as part of
     * a bitmask), the end user must periodically call {@code wait} or one of
     * its alternates to ensure that the futures representing calls to
     * individual subscribers are eventually executed. If
     * {@code std::launch::defered} is specified alone, the MessageBus will only
     * make progres when one of the message bus's wait methods is called. If
     * {@code std::launch::defered} is specified as part of a mask that includes
     * other options, it is implementation-defined whether the bus will process
     * messages without user intervention.
     *
     * @remarks
     * The order in which subscribers are notified of new messages is not well
     * defined. In the case where {@code std::launch::async} is used, all
     * standard caveats about thread execution ordering apply. In other cases,
     * the message bus may still make decisions to improve execution efficiency,
     * such as using a stack to keep track of futures, which would result in a
     * "depth first" execution of message callback rather than execution in the
     * order messages are published.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    class MessageBus final : public IMessagePublisher {
    public:
        /**
         * Constructs a new {@code MessageBus} instance.
         *
         * @param launchPolicy The launch policy used when publishing messages.
         *                     A new {@code std::async} is created for each
         *                     subscriber to each published message using this
         *                     launch policy.
         **/
        explicit MessageBus(std::launch launchPolicy = std::launch::async)
            : launchPolicy(launchPolicy) {}

        /**
         * Deleted copy constructor. The {@code MessageBus} may use mutexes
         * internally, which are not copyable.
         **/
        MessageBus(const MessageBus&) = delete;

        /**
         * Constructs a new {@code MessageBus} instance that takes its internal
         * state from the provided {@code MessageBus} and leaves that instance
         * in a default state.
         **/
        MessageBus(MessageBus&&) = default;

        /**
         * Removes a single message callback from the internal queue and waits
         * for its completion. If the queue is empty, blocks until a message is
         * inserted into the queue.
         *
         * @remarks
         * Ultimately, this method results in calling
         * {@code std::future::get}, thus the exact execution model is dependent
         * on the behavior of that method.
         **/
        void wait();

        /**
         * Behaves as if {@code wait} is called repeatedly in a loop until the
         * internal queue is empty, except that if the queue is empty at the
         * time {@code waitAll} is called, it will return immediately.
         **/
        void waitAll();

        /**
         * Removes a single message callback from the internal queue and waits
         * for its completion. If the queue is empty, blocks until a message is
         * inserted into the queue or until the specied duration has elapsed.
         *
         * @remarks
         * The timeout specified by {@p dur} is used only while waiting for a
         * message to be inserted into the internal queue. Once a message is
         * dequed, the function will block until the message has completed
         * processing.
         *
         * @remarks
         * Ultimately, this method results in calling {@code std::future::get},
         * thus the exact execution model is dependent on the behavior of that
         * method.
         *
         * @param dur The amount of time to wait for a message to be inserted.
         **/
        template <class Rep, class Period>
        void waitFor(std::chrono::duration<Rep, Period> dur) {
            std::future<void> f = try_dequeue_for(dur);
            if (f.valid()) {
                f.get();
            }
        }

        /**
         * Removes a single message callback from the internal queue and waits
         * for its completion. If the queue is empty, blocks until a message is
         * inserted into the queue or until the specied time has been reached.
         *
         * @remarks
         * The timeout specified by {@p timeout} is used only while waiting for
         * a message to be inserted into the internal queue. Once a message is
         * dequed, the function will block until the message has completed
         * processing.
         *
         * @remarks
         * Ultimately, this method results in calling {@code std::future::get},
         * thus the exact execution model is dependent on the behavior of that
         * method.
         *
         * @param timeout The time at which to give up waiting.
         **/
        template <class Clock, class Duration>
        void waitUntil(std::chrono::time_point<Clock, Duration> timeout) {
            std::future<void> f = try_dequeue_until(timeout);
            if (f.valid()) {
                f.get();
            }
        }

        /**
         * Registers the given consumer to receive messages with the given Id.
         *
         * @param consumer A pointer to a message consumer. The pointer is a
         *                 raw, unmanaged pointer, which the message bus assumes
         *                 will be valid for its lifetime. The @{code
         *                 MessageBus} will does not assume ownership of the
         *                 consumer.
         * @param source   The source of messages that the consumer is
         *                 interested in.
         * @param id       The id of the message the consumer is interested in.
         *                 if no {@p id} is specified, the consumer will receive
         *                 all messages from the source.
         **/
        void subscribe(IMessageProcessor* consumer,
                       std::string source,
                       MessageId id = MessageId::All) override;

        /**
         * Unsubscribes the given consumer from all messages.
         *
         * @param consumer The consumer to unsubscribe.
         **/
        void unsubscribe(IMessageProcessor* consumer) override;

        /**
         * Unsubscribes the given consumer from messages from the specified
         * source.
         *
         * @param consumer The consumer to unsubscribe.
         * @param source   The source the consumer is no longer interested in.
         **/
        void unsubscribe(IMessageProcessor* consumer, const std::string& source) override;

        /**
         * Publishes a message to subscribers. Subscribers receive only messages
         * to which they have subscribed.
         *
         * @param message A pointer to a message to publish.
         **/
        void publish(std::shared_ptr<Message> message) override;

    private:
        void clear_completed();
        void enqueue(std::future<void> message);
        std::future<void> dequeue();

        template <class Rep, class Period>
        std::future<void> try_dequeue_for(std::chrono::duration<Rep, Period> rel_time) {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (!queue_cv.wait_for(lock, rel_time, [&]() { return !queue.empty(); })) {
                return std::future<void>();
            }

            std::future<void> f = std::move(queue.front());
            queue.pop_front();
            return f;
        }

        template <class Clock, class Duration>
        std::future<void> try_dequeue_until(std::chrono::time_point<Clock, Duration> timeout_time) {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (!queue_cv.wait_until(lock, timeout_time, [&]() { return !queue.empty(); })) {
                return std::future<void>();
            }

            std::future<void> f = std::move(queue.front());
            queue.pop_front();
            return f;
        }

        using callback_pair = std::pair<MessageId, IMessageProcessor*>;

        const std::launch launchPolicy;
        std::unordered_map<std::string, std::vector<callback_pair>> subscribers;
        std::deque<std::future<void>> queue;

        std::recursive_mutex subs_mutex;
        std::mutex queue_mutex;
        std::condition_variable queue_cv;
    };
}
#endif
