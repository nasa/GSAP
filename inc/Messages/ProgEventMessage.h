// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGES_PROGEVENT_H
#define PCOE_MESSAGES_PROGEVENT_H

#include "Messages/Message.h"
#include "ProgEvent.h"

namespace PCOE {
    /**
     * A message that carries a ProgEvent as payload. This message is not
     * serializable.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    class ProgEventMessage : public Message {
    public:
        /**
         * Constructs a new instance of @{code ProgEventMessage}.
         *
         * @param id     The id of the message.
         * @param source The source of the message.
         * @param value  The ProgEvent represented by this message.
         **/
        ProgEventMessage(MessageId id, std::string source, ProgEvent value)
            : Message(id, source), value(value) {}

        /**
         * Constructs a new instance of @{code ProgEventMessage}.
         *
         * @param id        The id of the message.
         * @param source    The source of the message.
         * @param timestamp The time at which the message or the data contained
         *                  by the message was generated.
         * @param value  The ProgEvent represented by this message.
         **/
        ProgEventMessage(MessageId id, std::string source, time_point timestamp, ProgEvent value)
            : Message(id, source, timestamp), value(value) {}

        /**
         * Gets the ProgEvent associated with the current message.
         **/
        const ProgEvent& getValue() const {
            return value;
        }

    protected:
        std::uint16_t getPayloadSize() const override final {
            throw std::domain_error("Note serializable");
        }

        void serializePayload(std::ostream& os) const override final {
            throw std::domain_error("Note serializable");
        }

    private:
        ProgEvent value;
    };
}

#endif
