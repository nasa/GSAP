// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGES_IMESSAGEPRODUCER_H
#define PCOE_MESSAGES_IMESSAGEPRODUCER_H
#include <memory>
#include <string>

#include "Messages/IMessageProcessor.h"
#include "Messages/Message.h"

namespace PCOE {
    /**
     *  An interface for objects that produce messages.
     *
     * Implementors of this interface should maintain order of subscribers.
     * I.e., callbacks should be executed in the order that they were
     * originally subscribed. The source of the callback, either a callback
     * function or an IMessageProcessor, should not be considered when ordering
     * callbacks.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    class IMessagePublisher {
    public:
        /**
         * Virtual destructor to allow for deletion of objects via their
         * interface pointers.
         **/
        virtual ~IMessagePublisher() = default;

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
         *                 By default, the consumer is subscribed to all
         *                 messages from the given source.
         **/
        virtual void subscribe(IMessageProcessor* consumer,
                               std::string source,
                               MessageId id = MessageId::All) = 0;

        /**
         * Unsubscribes the given consumer from all messages.
         *
         * @param consumer The consumer to unsubscribe.
         **/
        virtual void unsubscribe(IMessageProcessor* consumer) = 0;

        /**
         * Unsubscribes the given consumer from messages from the specified
         * source.
         *
         * @param consumer The consumer to unsubscribe.
         * @param source   The source the consumer is no longer interested in.
         **/
        virtual void unsubscribe(IMessageProcessor* consumer, const std::string& source) = 0;

        /**
         * Publishes a message to subscribers. Subscribers receive only messages
         * to which they have subscribed.
         *
         * @param message A pointer to a message to publish. The pointer is a raw,
         *                unmanaged pointer, which the message bus assumes will
         *                be valid for its lifetime. No attempt is made to manage
         *                the lifetime of the message.
         **/
        virtual void publish(std::shared_ptr<Message> message) = 0;
    };
}
#endif
