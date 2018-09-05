// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_POINT3D_H
#define PCOE_POINT3D_H

namespace PCOE {
    /**
     * Represents a point in 3D space
     *
     * @author Christopher Teubert
     * @since 1.2
     **/
    class Point3D {
    public:
        /**
         *  Create a new point in 3D space
         *
         *  @param  lat Latitude
         *  @param  lon Longitude
         *  @param  alt Altitude
         */
        Point3D(double lat, double lon, double alt) : lat(lat), lon(lon), alt(alt) {}

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
}

#endif
