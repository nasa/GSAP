// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGES_TEMPLATEMESSAGE_H
#define PCOE_MESSAGES_TEMPLATEMESSAGE_H

#include "Messages/Message.h"

namespace PCOE {
    /**
     * A message that carries a templatized payload.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    template <class T>
    class TemplateMessage : public Message {
    public:
        /**
         * Constructs a new instance of @{code EmptyMessage}.
         *
         * @param id     The id of the message.
         * @param source The source of the message.
         * @param v      The value held by the message.
         **/
        TemplateMessage(MessageId id, std::string source, T v)
            : Message(id, source), value(std::move(v)) {}

        /**
         * Constructs a new instance of @{code EmptyMessage}.
         *
         * @param id        The id of the message.
         * @param source    The source of the message.
         * @param v      The value held by the message.
         * @param timestamp The time at which the message or the data contained
         *                  by the message was generated.
         **/
        TemplateMessage(MessageId id, std::string source, T v, time_point timestamp)
            : Message(id, source, timestamp), value(std::move(v)) {}

        const T& getValue() {
            return value;
        }

    protected:
        std::uint16_t getPayloadSize() const override final {
            Unimplemented("Can't serialize generic type");
        }

        void serializePayload(std::ostream&) const override final {
            Unimplemented("Can't serialize generic type");
        }

    private:
        T value;
    };
}

#endif
