// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_VECTORMESSAGE_H
#define PCOE_VECTORMESSAGE_H
#include <iostream>
#include <vector>

#include "Contracts.h"
#include "Messages/Message.h"

namespace PCOE {
    /**
     * A message the carries a vector of values.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    template <class T>
    class VectorMessage final : public Message {
    public:
        /**
         * Constructs a new instance of @{code VectorMessage}, copying the
         * contents of {@param values}.
         *
         * @param id        The id of the message.
         * @param source    The source of the message.
         * @param timestamp The time at which the message or the data contained
         *                  by the message was generated.
         * @param value     The value of the message.
         **/
        VectorMessage(MessageId id,
                      std::string source,
                      time_point timestamp,
                      const std::vector<T>& values)
            : Message(id, source, timestamp), values(values) {
            Expect((static_cast<std::uint64_t>(id) & 0x0000C00000000000L) > 0,
                   "Message id is not vector");
        }

        /**
         * Constructs a new instance of @{code VectorMessage}, moving the
         * contents of {@param values}.
         *
         * @param id        The id of the message.
         * @param source    The source of the message.
         * @param timestamp The time at which the message or the data contained
         *                  by the message was generated.
         * @param value     The value of the message.
         **/
        VectorMessage(MessageId id,
                      std::string source,
                      time_point timestamp,
                      std::vector<T>&& values)
            : Message(id, source, timestamp), values(values) {
            Expect((static_cast<std::uint64_t>(id) & 0x0000C00000000000L) > 0,
                   "Message id is not vector");
        }

        /**
         * Constructs a new instance of @{code VectorMessage} with the
         * contents of {@param values}.
         *
         * @param id        The id of the message.
         * @param source    The source of the message.
         * @param timestamp The time at which the message or the data contained
         *                  by the message was generated.
         * @param value     The value of the message.
         **/
        VectorMessage(MessageId id,
                      std::string source,
                      time_point timestamp,
                      std::initializer_list<T> values)
            : Message(id, source, timestamp), values(values) {
            Expect((static_cast<std::uint64_t>(id) & 0x0000C00000000000L) > 0,
                   "Message id is not vector");
        }

        /**
         * Gets the value associated with the message.
         **/
        inline const std::vector<T>& getValue() const {
            return values;
        }

    protected:
        std::uint16_t getPayloadSize() const override final {
            std::size_t size = 2 + values.size() * sizeof(T);
            Expect(size < std::numeric_limits<std::uint16_t>::max(), "Payload size too big");
            return static_cast<std::uint16_t>(size);
        }

        void serializePayload(std::ostream& os) const override final {
            Expect(values.size() < std::numeric_limits<std::uint16_t>::max(), "Vector size");
            std::uint16_t size = static_cast<std::uint16_t>(values.size());
            os.write(reinterpret_cast<const char*>(&size), 2);
            for (const T& value : values) {
                os.write(reinterpret_cast<const char*>(&value), sizeof(T));
            }
        }

    private:
        std::vector<T> values;
    };

    using U8VecMessage = VectorMessage<std::uint8_t>;
    using U16VecMessage = VectorMessage<std::uint16_t>;
    using U32VecMessage = VectorMessage<std::uint32_t>;
    using U64VecMessage = VectorMessage<std::uint64_t>;
    using I8VecMessage = VectorMessage<std::int8_t>;
    using I16VecMessage = VectorMessage<std::int16_t>;
    using I32VecMessage = VectorMessage<std::int32_t>;
    using I64VecMessage = VectorMessage<std::int64_t>;
    using FloatVecMessage = VectorMessage<float>;
    using DoubleVecMessage = VectorMessage<double>;
}

#endif
