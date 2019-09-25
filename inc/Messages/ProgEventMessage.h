// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGES_PROGEVENT_H
#define PCOE_MESSAGES_PROGEVENT_H

#include "Messages/ScalarMessage.h"
#include "ProgEvent.h"

namespace PCOE {
    /**
     * A message the carries a single ProgEvent.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    class ProgEventMessage final : public Message {
    public:
        /**
         * Constructs a new instance of @{code ProgEventMessage}.
         *
         * @param id        The id of the message.
         * @param source    The source of the message.
         * @param timestamp The time at which the message or the data contained
         *                  by the message was generated.
         * @param value     The value of the message.
         **/
        ProgEventMessage(MessageId id,
                         std::string source,
                         time_point timestamp,
                         const ProgEvent& value)
            : Message(id, source, timestamp), value(value) {
            Expect((static_cast<std::uint64_t>(id) & 0x0000300000000000L) > 0,
                   "Message id is not scalar");
        }

        /**
         * Gets the value associated with the message.
         **/
        inline const ProgEvent& getValue() const {
            return value;
        }

    protected:
        std::uint16_t getPayloadSize() const override final {
            throw std::domain_error("Serialization not supported");
        }

        void serializePayload(std::ostream& os) const override final {
            (void)os;
            throw std::domain_error("Serialization not supported");
        }

    private:
        ProgEvent value;
    };
}

#endif
