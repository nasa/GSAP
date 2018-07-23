// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGES_DOUBLEMESSAGE_H
#define PCOE_MESSAGES_DOUBLEMESSAGE_H

#include "Messages/Message.h"

namespace PCOE {
    /**
     * A message that carries a single double as payload. The meaning of the
     * payload depends on the message's id.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    class DoubleMessage : public Message {
    public:
        /**
         * Constructs a new instance of @{code DoubleMessage}.
         *
         * @param id     The id of the message.
         * @param source The source of the message.
         * @param value  The value of the message.
         **/
        DoubleMessage(MessageId id, std::string source, double value)
            : Message(id, source), value(value) {}

        /**
         * Constructs a new instance of @{code DoubleMessage}.
         *
         * @param id        The id of the message.
         * @param source    The source of the message.
         * @param timestamp The time at which the message or the data contained
         *                  by the message was generated.
         * @param value     The value of the message.
         **/
        DoubleMessage(MessageId id, std::string source, time_point timestamp, double value)
            : Message(id, source, timestamp), value(value) {}

        /**
         * Gets the value associated with the message.
         **/
        inline double getValue() const {
            return value;
        }

    protected:
        std::uint16_t getPayloadSize() const override final {
            return sizeof(double);
        }

        void serializePayload(std::ostream& os) const override final {
            os.write(reinterpret_cast<const char*>(&value), sizeof(double));
        }

    private:
        double value;
    };
}

#endif
