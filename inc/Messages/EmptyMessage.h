// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_EMPTYMESSAGE_H
#define PCOE_EMPTYMESSAGE_H
#include "Messages/Message.h"

namespace PCOE {
    /**
     * A message with no payload.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    class EmptyMessage final : public Message {
    public:
        /**
         * Constructs a new instance of @{code EmptyMessage}.
         *
         * @param id     The id of the message.
         * @param source The source of the message.
         **/
        EmptyMessage(MessageId id, std::string source);

        /**
         * Constructs a new instance of @{code EmptyMessage}.
         *
         * @param id        The id of the message.
         * @param source    The source of the message.
         * @param timestamp The time at which the message or the data contained
         *                  by the message was generated.
         **/
        EmptyMessage(MessageId id, std::string source, time_point timestamp);

    protected:
        inline std::uint16_t getPayloadSize() const override final {
            return 0;
        }

        inline void serializePayload(std::ostream&) const override final {}
    };
}

#endif
