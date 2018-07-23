// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGES_UDATAMESSAGE_H
#define PCOE_MESSAGES_UDATAMESSAGE_H

#include "Messages/Message.h"
#include "UData.h"

namespace PCOE {
    /**
     * A message that carries a UData as payload.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    class UDataMessage : public Message {
    public:
        /**
         * Constructs a new instance of @{code UDataMessage}.
         *
         * @param id     The id of the message.
         * @param source The source of the message.
         * @param value  The UData represented by this message.
         **/
        UDataMessage(MessageId id, std::string source, UData value)
            : Message(id, source), value(value) {}

        /**
         * Constructs a new instance of @{code UDataMessage}.
         *
         * @param id        The id of the message.
         * @param source    The source of the message.
         * @param timestamp The time at which the message or the data contained
         *                  by the message was generated.
         * @param value     The UData represented by this message.
         **/
        UDataMessage(MessageId id, std::string source, time_point timestamp, UData value)
            : Message(id, source, timestamp), value(value) {}

        /**
         * Gets the UData associated with the current message.
         **/
        UData getValue() const {
            return value;
        }

    protected:
        std::uint16_t getPayloadSize() const override final {
            return sizeof(DIST_TYPE) + sizeof(UType) + sizeof(uint16_t) +
                   value.size() * sizeof(double);
        }

        void serializePayload(std::ostream& os) const override final {
            auto dist = value.dist();
            os.write(reinterpret_cast<const char*>(dist), sizeof(dist));
            auto uncertainty = value.uncertainty();
            os.write(reinterpret_cast<const char*>(uncertainty), sizeof(uncertainty));
            if (value.size() > std::numeric_limits<uint16_t>::max()) {
                // TODO (JW): Throw here
            }
            auto size = static_cast<uint16_t>(value.size());
            os.write(reinterpret_cast<const char*>(size), sizeof(size));
            for (auto d : value) {
                os.write(reinterpret_cast<const char*>(&d), sizeof(value));
            }
        }

    private:
        UData value;
    };
}

#endif
