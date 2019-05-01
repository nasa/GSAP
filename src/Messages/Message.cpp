// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <iostream>

#include "Contracts.h"
#include "Messages/Message.h"

namespace PCOE {
    void Message::serialize(std::ostream& os) const {
        Expect(source.length() < std::numeric_limits<std::uint16_t>::max(), "Source length");
        os.write(reinterpret_cast<const char*>(&id), 8);

        std::uint16_t sourceLen = static_cast<std::uint16_t>(source.length());
        os.write(reinterpret_cast<const char*>(&sourceLen), 2);
        os.write(source.c_str(), sourceLen);

        std::uint64_t raw_time = timestamp.time_since_epoch().count();
        os.write(reinterpret_cast<const char*>(&raw_time), 8);

        std::uint32_t payloadLen = getPayloadSize();
        Require(payloadLen < 1073741824, "Payload length");
        os.write(reinterpret_cast<const char*>(&payloadLen), sizeof(payloadLen));
        serializePayload(os);
    }
}
