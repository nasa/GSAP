// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_POINT4D_H
#define PCOE_POINT4D_H
#include <chrono>

namespace PCOE {
    /**
     * Represents a point in 4D space
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    template <class Clock, class Duration = typename Clock::duration>
    class Point4D {
    public:
        /**
         * Create a new point at the origin.
         **/
        Point4D() = default;

        /**
         * Create a new point in 4D space
         *
         * @param lat    The latitude in degrees.
         * @param lon    The longitude in degrees.
         * @param alt    The altitude in meters.
         * @param time   The time associated with the point.
         * @param states Information associated with the point
         */
        Point4D(double lat,
                double lon,
                double alt,
                std::chrono::time_point<Clock, Duration> time,
                std::vector<double> states = {})
            : lat(lat), lon(lon), alt(alt), time(time), states(std::move(states)) {}

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

        /**
         * Gets the waypoint time.
         **/
        inline std::chrono::time_point<Clock, Duration> getTime() const {
            return time;
        }

        /**
         * Get the states associated with the point.
         **/
        inline const std::vector<double> getStates() const {
            return states;
        }

    private:
        double lat;
        double lon;
        double alt;
        std::chrono::time_point<Clock, Duration> time;
        std::vector<double> states;
    };
}

#endif
