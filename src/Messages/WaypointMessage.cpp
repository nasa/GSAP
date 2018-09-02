// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Messages/WaypointMessage.h"

namespace PCOE {
    WaypointMessage::WaypointMessage(PCOE::MessageId id,
                                     std::string source,
                                     time_point eta,
                                     double lat,
                                     double lon,
                                     double alt)
        : Message(id, source), eta(eta), lat(lat), lon(lon), alt(alt) {}

    WaypointMessage::WaypointMessage(PCOE::MessageId id,
                                     std::string source,
                                     time_point timestamp,
                                     time_point eta,
                                     double lat,
                                     double lon,
                                     double alt)
        : Message(id, source, timestamp), eta(eta), lat(lat), lon(lon), alt(alt) {}

    std::uint16_t WaypointMessage::getPayloadSize() const {
        return 32;
    }

    void WaypointMessage::serializePayload(std::ostream& os) const {
        os.write(reinterpret_cast<const char*>(&eta), sizeof(eta));
        os.write(reinterpret_cast<const char*>(&lat), sizeof(lat));
        os.write(reinterpret_cast<const char*>(&lon), sizeof(lon));
        os.write(reinterpret_cast<const char*>(&alt), sizeof(alt));
    }
}
