// Copyright (c) 2017-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_WAYPOINT_H
#define PCOE_WAYPOINT_H

namespace PCOE {
    class Waypoint final {
    public:
        const double latitude;
        const double longitude;
        const double altitude;
        const double eta;

        Waypoint(double lat, double lon, double alt, double eta)
            : latitude(lat), longitude(lon), altitude(alt), eta(eta) {}
    };
}

#endif
