// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <iostream>

#include "Messages/WaypointMessage.h"

namespace PCOE {
    WaypointMessage::WaypointMessage(PCOE::MessageId id,
                                     std::string source,
                                     time_point timestamp,
                                     time_point eta,
                                     double lat,
                                     double lon,
                                     double alt)
        : Message(id, source, timestamp), eta(eta), point(lat, lon, alt) {}

    std::uint32_t WaypointMessage::getPayloadSize() const {
        return 32;
    }

    void WaypointMessage::serializePayload(std::ostream& os) const {
        os.write(reinterpret_cast<const char*>(&eta), sizeof(eta));
        auto lat = point.getLatitude();
        auto lon = point.getLongitude();
        auto alt = point.getAltitude();
        os.write(reinterpret_cast<const char*>(&lat), sizeof(lat));
        os.write(reinterpret_cast<const char*>(&lon), sizeof(lon));
        os.write(reinterpret_cast<const char*>(&alt), sizeof(alt));
    }
}
