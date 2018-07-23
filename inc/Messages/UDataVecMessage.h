// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGES_UDATAVECMESSAGE_H
#define PCOE_MESSAGES_UDATAVECMESSAGE_H

#include "Messages/Message.h"
#include "UData.h"

namespace PCOE {
    /**
     * A message that carries a vector of UData as payload
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    class UDataVecMessage : public Message {
    public:
        /**
         * Constructs a new instance of @{code UDataVecMessage}.
         *
         * @param id        The id of the message.
         * @param source    The source of the message.
         * @param value     The UData vector represented by this message.
         **/
        UDataVecMessage(MessageId id, std::string source, std::vector<UData> value)
            : Message(id, source), value(value) {}

        /**
         * Constructs a new instance of @{code UDataVecMessage}.
         *
         * @param id        The id of the message.
         * @param source    The source of the message.
         * @param timestamp The time at which the message or the data contained
         *                  by the message was generated.
         * @param value     The UData vector represented by this message.
         **/
        UDataVecMessage(MessageId id,
                        std::string source,
                        time_point timestamp,
                        std::vector<UData> value)
            : Message(id, source, timestamp), value(value) {}

        /**
         * Gets the UData vector associated with the current message.
         **/
        std::vector<UData> getValue() const {
            return value;
        }

    protected:
        std::uint16_t getPayloadSize() const override final {
            return 0;
        }

        void serializePayload(std::ostream& os) const override final {
            // TODO (JW): Distinguish between serializable and non-serializable messages?
            return;
        }

    private:
        std::vector<UData> value;
    };
}

#endif
