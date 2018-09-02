// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#ifndef PCOE_ITRAJECTORYCORRELATOR_H
#define PCOE_ITRAJECTORYCORRELATOR_H

#include "Messages/Message.h"

namespace PCOE {
    class Point3D {
    public:
        Point3D(double lat, double lon, double alt)
        : lat(lat), lon(lon), alt(alt) { }
        
        /**
         * Gets the waypoint latitude.
         **/
        inline double getLatitude() const {
            return lat;
        }
        
        /**
         * Gets the waypoint longitude.
         **/
        inline double getLongitude() const {
            return lon;
        }
        
        /**
         * Gets the waypoint altitude.
         **/
        inline double getAltitude() const {
            return alt;
        }
    private:
        double lat;
        double lon;
        double alt;
    };
    
    class ITrajectoryCorrelator {
    public:
        virtual Point3D getPoint(Message::time_point) = 0;
    };
}

#endif
