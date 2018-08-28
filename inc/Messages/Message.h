// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGES_MESSAGE_H
#define PCOE_MESSAGES_MESSAGE_H
#include <iostream>
#include <string>
#include <vector>

#include "Contracts.h"
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
        EmptyMessage(MessageId id, std::string source) : Message(id, source) {
            Expect((static_cast<std::uint64_t>(id) & 0x0000FF0000000000L) == 0,
                   "Message id is not empty");
        }

        /**
         * Constructs a new instance of @{code EmptyMessage}.
         *
         * @param id        The id of the message.
         * @param source    The source of the message.
         * @param timestamp The time at which the message or the data contained
         *                  by the message was generated.
         **/
        EmptyMessage(MessageId id, std::string source, time_point timestamp)
            : Message(id, source, timestamp) {
            Expect((static_cast<std::uint64_t>(id) & 0x0000FF0000000000L) == 0,
                   "Message id is not empty");
        }

    protected:
        std::uint16_t getPayloadSize() const override final {
            return 0;
        }

        void serializePayload(std::ostream&) const override final {}
    };

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
         * @param id     The id of the message.
         * @param source The source of the message.
         * @param value  The value of the message.
         **/
        VectorMessage(MessageId id, std::string source, const std::vector<T>& values)
            : Message(id, source), values(values) {
            Expect((static_cast<std::uint64_t>(id) & 0x0000C00000000000L) > 0,
                   "Message id is not vector");
        }

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
         * @param id     The id of the message.
         * @param source The source of the message.
         * @param value  The value of the message.
         **/
        VectorMessage(MessageId id, std::string source, std::vector<T>&& values)
            : Message(id, source), values(values) {
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
         * Constructs a new instance of @{code VectorMessage}, with the
         * contents of {@param values}.
         *
         * @param id     The id of the message.
         * @param source The source of the message.
         * @param value  The value of the message.
         **/
        VectorMessage(MessageId id, std::string source, std::initializer_list<T> values)
            : Message(id, source), values(values) {
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
