// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_SCALARMESSAGE_H
#define PCOE_SCALARMESSAGE_H
#include "Contracts.h"
#include "Messages/Message.h"

namespace PCOE {
    /**
     * A message the carries a single scalar value.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    template <class T>
    class ScalarMessage final : public Message {
    public:
        /**
         * Constructs a new instance of @{code ScalarMessage}.
         *
         * @param id     The id of the message.
         * @param source The source of the message.
         * @param value  The value of the message.
         **/
        ScalarMessage(MessageId id, std::string source, const T& value)
            : Message(id, source), value(value) {
            Expect((static_cast<std::uint64_t>(id) & 0x0000300000000000L) > 0,
                   "Message id is not scalar");
        }

        /**
         * Constructs a new instance of @{code ScalarMessage}.
         *
         * @param id        The id of the message.
         * @param source    The source of the message.
         * @param timestamp The time at which the message or the data contained
         *                  by the message was generated.
         * @param value     The value of the message.
         **/
        ScalarMessage(MessageId id, std::string source, time_point timestamp, const T& value)
            : Message(id, source, timestamp), value(value) {
            Expect((static_cast<std::uint64_t>(id) & 0x0000300000000000L) > 0,
                   "Message id is not scalar");
        }

        /**
         * Gets the value associated with the message.
         **/
        inline const T& getValue() const {
            return value;
        }

    protected:
        std::uint16_t getPayloadSize() const override final {
            return sizeof(T);
        }

        void serializePayload(std::ostream& os) const override final {
            os.write(reinterpret_cast<const char*>(&value), sizeof(T));
        }

    private:
        T value;
    };

    using U8Message = ScalarMessage<std::uint8_t>;
    using U16Message = ScalarMessage<std::uint16_t>;
    using U32Message = ScalarMessage<std::uint32_t>;
    using U64Message = ScalarMessage<std::uint64_t>;
    using I8Message = ScalarMessage<std::int8_t>;
    using I16Message = ScalarMessage<std::int16_t>;
    using I32Message = ScalarMessage<std::int32_t>;
    using I64Message = ScalarMessage<std::int64_t>;
    using FloatMessage = ScalarMessage<float>;
    using DoubleMessage = ScalarMessage<double>;
}

#endif
