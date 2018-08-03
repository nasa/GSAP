// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGES_MESSAGEBUS_H
#define PCOE_MESSAGES_MESSAGEBUS_H
#include <future>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "Messages/IMessagePublisher.h"

namespace PCOE {
    /**
     * A collection of message subcribers that receive messages based on their
     * message Id's.
     *
     * For each message that is published, the set of subscribers for that
     * message is determined, and each subscriber in that set is called in its
     * own {@code std::async} instance. The behavior of those instances is
     * controller by the launch policy specified when the message bus is
     * constructed.
     *
     * By default, the {@code std::launch::async} policy is specified, which
     * launches each {@code std::async} instance on its own thread. In this
     * mode, no active management of the message bus is required, as message
     * procesing will proceed automatically on threads managed by the message
     * bus.
     *
     * If {@code std::launch::defered} is specified (either alone or as part of
     * a bitmask), the end user must periodically call {@code processOne} or
     * {@code processAll} to ensure that the futures representing calls to
     * individual subscribers are eventually executed. If
     * {@code std::launch::defered} is specified alone, the MessageBus will only
     * make progres when one of the message bus's process methods is called. If
     * {@code std::launch::defered} is specified as part of a mask that includes
     * other options, it is implementation-defined whether the bus will process
     * messages without user intervention.
     *
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
         * A new {@code std::async} is created for each subscriber to each
         * published message using this launch policy.
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
         * for its completion. Ultimately, this method results in calling
         * {@code std::future::get}, so the specifics of how that callback is
         * executed match the implementation details of that method.
         **/
        void processOne();

        /**
         * Removes a message callbacks from the internal queue one at a time and
         * waits for their completion, continuing until the internal queue is
         * empty. Ultimately, this method results in calling
         * {@code std::future::get}, so the specifics of how that callbacks are
         * executed match the implementation details of that method.
         **/
        void processAll();

        /**
         * Registers the given consumer to receive messages with the given Id.
         *
         * @param consumer A pointer to a message consumer. The pointer is a
         *                 raw, unmanaged pointer, which the message bus
         *                 assumes will be valid for its lifetime. The @{code
         *                 MessageBus} will does not assume ownership of the
         *                 consumer.
         * @param source   The source of messages that the consumer is
         *                 interested in.
         * @param id       The id of the message the consumer is interested in.
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
        using callback_pair = std::pair<MessageId, IMessageProcessor*>;
        using mutex = std::recursive_mutex;
        using lock_guard = std::lock_guard<mutex>;
        using unique_lock = std::unique_lock<mutex>;

        const std::launch launchPolicy;
        std::unordered_map<std::string, std::vector<callback_pair>> subscribers;
        std::vector<std::future<void>> futures;
        mutable mutex m;
    };
}
#endif
