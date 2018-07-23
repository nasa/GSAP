// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGES_MESSAGEBUS_H
#define PCOE_MESSAGES_MESSAGEBUS_H
#include <mutex>
#include <unordered_map>
#include <vector>

#include "Messages/IMessagePublisher.h"

namespace PCOE {
    /**
     * A collection of message subcribers that receive messages based on their
     * message Id's.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    class MessageBus : public IMessagePublisher {
    public:
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
         * @param message A pointer to a message to publish. The pointer is a raw,
         *                unmanaged pointer, which the message bus assumes will
         *                be valid for its lifetime. No attempt is made to manage
         *                the lifetime of the message.
         **/
        void publish(std::shared_ptr<Message> message) const;

    private:
        using callback_pair = std::pair<MessageId, IMessageProcessor*>;
        using mutex = std::mutex;
        using lock_guard = std::lock_guard<mutex>;

        std::unordered_map<std::string, std::vector<callback_pair>> subscribers;
        mutable mutex m;
    };
}
#endif
