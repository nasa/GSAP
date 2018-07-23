// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGEID_H
#define PCOE_MESSAGEID_H
#include <cstdint>

namespace PCOE {
    /**
     * The set of pre-defined message identifiers. Any integer in the range of
     * uint64_t may be a valid identifier, any entity dealing with messages should
     * not treat this list as exhaustive.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    enum class MessageId : std::uint64_t {
        // Note (JW): If you add a value to this enum, add it to the map in
        //            MessageId.cpp as well.
        All = 0x0000000000000000L,
        Voltage = 0x6272000000000000L,
        Current = 0x6272000000000100L,
        Power = 0x6272000000000200L,
        Temperature_K = 0x6272000000000300L,
        Temperature_C = 0x6272000000000301L,
        Temperature_F = 0x6272000000000302L,
        ModelStateEstimate = 0x6272000000000400L,
        TestInput0 = 0x627200000000FE00L,
        TestInput1 = 0x627200000000FE01L,
        TestInput2 = 0x627200000000FE02L,
        TestInput3 = 0x627200000000FE03L,
        TestOutput0 = 0x627200000000FF00L,
        TestOutput1 = 0x627200000000FF01L,
        TestOutput2 = 0x627200000000FF02L,
        TestOutput3 = 0x627200000000FF03L,
        EoD = 0x6272000100000000L,
        EoL = 0x6272000100000100L,
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
