// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGE_H
#define PCOE_MESSAGE_H
#include <iosfwd>
#include <string>

#include "Messages/MessageClock.h"
#include "Messages/MessageId.h"

namespace PCOE {
    /**
     * Defines the structure of messages passed between components
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    class Message {
    public:
        /**
         * The type used by messages to describe when they were created.
         **/
        using time_point = MessageClock::time_point;

        /**
         * Constructs a new instance of @{code Message} with the timestamp set
         * to the current time.
         *
         * @param id     The id of the message.
         * @param source The source of the message.
         **/
        Message(MessageId id, std::string source) : Message(id, source, MessageClock::now()) {}

        /**
         * Constructs a new instance of @{code Message}.
         *
         * @param id        The id of the message.
         * @param source    The source of the message.
         * @param timestamp The time at which the message or the data contained
         *                  by the message was generated.
         **/
        Message(MessageId id, std::string source, time_point timestamp)
            : id(id), source(source), timestamp(timestamp) {}

        /**
         * Constructs a new instance of @{code Message} with a copy of the data
         * held by the provided message.
         *
         * @param other The message to copy.
         **/
        Message(const Message& other) = default;

        /**
         * Default destructor. A virtual default destructor is necessary to
         * ensure that resources in classes inherting from this class are
         * cleaned up, even if the destructor is called through a pointer of the
         * base class type.
         **/
        virtual ~Message() = default;

        /**
         * Gets the id of the message.
         **/
        inline MessageId getMessageId() const {
            return id;
        }

        /**
         * Gets the source of the message.
         **/
        inline const std::string& getSource() const {
            return source;
        }

        /**
         * Gets the timestamp of the message.
         **/
        inline time_point getTimestamp() const {
            return timestamp;
        }

        /**
         * Serializes the message to the provided stream.
         **/
        void serialize(std::ostream& os) const;

    protected:
        virtual std::uint16_t getPayloadSize() const = 0;

        virtual void serializePayload(std::ostream& os) const = 0;

    private:
        const MessageId id;
        const std::string source;
        const time_point timestamp;
    };
}
#endif
