// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGEID_H
#define PCOE_MESSAGEID_H
#include <cstdint>

namespace PCOE {
    /**
     * The set of pre-defined message identifiers. Any integer in the range of
     * uint64_t may be a valid identifier. Any entity dealing with messages
     * should not treat this list as exhaustive.
     *
     * @remarks The two most significant bytes are used to encode the source of
     * an enum value. End users should pick a value that does not conflict with
     * any other source that they are using. Built in enum values are given the
     * prefix "0x6272" (NASA on a touch-tone phone).
     *
     * @remarks The third most significant byte encodes the payload of the
     * message as described in the following tables. All scalar payloads are
     * serialized directly. All vector payloads are serialized as a 16-bit
     * unsigned integer describing the number of elements followed by the
     * elements themselves.
     *
     * | Value | Meaning                                                       |
     * |------:|:--------------------------------------------------------------|
     * |  0x00 | The message has no payload                                    |
     * |  0x11 | A single unsigned 8 bit integer                               |
     * |  0x12 | A single unsigned 16 bit integer                              |
     * |  0x13 | A single unsigned 32 bit integer                              |
     * |  0x14 | A single unsigned 64 bit integer                              |
     * |  0x21 | A single signed 8 bit integer                                 |
     * |  0x22 | A single signed 16 bit integer                                |
     * |  0x23 | A single signed 32 bit integer                                |
     * |  0x24 | A single signed 64 bit integer                                |
     * |  0x31 | A single precision floating point number                      |
     * |  0x32 | A double precision floating point number                      |
     * |  0x38 | A single message-defined struct                               |
     * |  0x41 | A vector of unsigned 8 bit integers                           |
     * |  0x42 | A vector of unsigned 16 bit integers                          |
     * |  0x43 | A vector of unsigned 32 bit integers                          |
     * |  0x44 | A vector of unsigned 64 bit integers                          |
     * |  0x81 | A vector of signed 8 bit integers                             |
     * |  0x82 | A vector of signed 16 bit integers                            |
     * |  0x83 | A vector of signed 32 bit integers                            |
     * |  0x84 | A vector of signed 64 bit integers                            |
     * |  0xC1 | A vector of single precision floating point numbers           |
     * |  0xC2 | A vector of double precision floating point numbers           |
     * |  0xC8 | A vector of  message-defined structs                          |
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    enum class MessageId : std::uint64_t {
        // Note (JW): If you add a value to this enum, add it to the map in
        //            MessageId.cpp as well.
        All = 0x0000000000000000L,
        Volts = 0x6272320000000000L,
        Amperes = 0x6272320000000100L,
        Watts = 0x6272320000000200L,
        Kelvin = 0x6272320000000300L,
        Centigrade = 0x6272320000000301L,
        Fahrenheit = 0x6272320000000302L,
        ModelStateEstimate = 0x6272C20000000400L,
        ModelStateVector = 0x6272C20000000500L,
        ModelInputVector = 0x6272C20000000501L,
        ModelOutputVector = 0x6272C20000000502L,
        TestInput0 = 0x627232000000FE00L,
        TestInput1 = 0x627232000000FE01L,
        TestInput2 = 0x627232000000FE02L,
        TestInput3 = 0x627232000000FE03L,
        TestOutput0 = 0x627232000000FF00L,
        TestOutput1 = 0x627232000000FF01L,
        TestOutput2 = 0x627232000000FF02L,
        TestOutput3 = 0x627232000000FF03L,
        BatteryEod = 0x6272380100000000L,
        BatteryEol = 0x6272380100000100L,
        TestEvent0 = 0x627238FF00000000L,
        Intialize = 0x627200FE00000000L,
        Start = 0x627200FF00000000L,
        Stop = 0x627200FF00000001L,
        Pause = 0x627200FF00000002L,
        Resume = 0x627200FF00000003L,
    };

    /**
     * Parses a string and returns the corresponding message id.
     **/
    MessageId messageIdFromString(const std::string& key);
}
#endif
