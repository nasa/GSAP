// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_WAYPOINTMESSAGE_H
#define PCOE_WAYPOINTMESSAGE_H
#include "Messages/Message.h"
#include "Messages/MessageId.h"

namespace PCOE {
    /**
     * Represents a single configuration key/value pair.
     *
     * @remarks
     * Note that this message is used for all types of configuration, including
     * scalar numeric values and vectors. Vectors should be represetned as
     * comma-separated lists. Historically GSAP configs were read from a file on
     * disk, where this string-based interface made sense.
     *
     * @author Jason Watkins
     * @since 1.1
     **/
    class WaypointMessage : public PCOE::Message {
    public:
        /**
         * Constructs a new instance of @{code ConfigMessage}.
         *
         * @param id     The id of the message.
         * @param source The source of the message.
         * @param eta    The ETA of the waypoint
         * @param lat    The latitude of the waypoint
         * @param lon    The longitude of the waypoint
         * @param alt    The altitude of the waypoint
         **/
        WaypointMessage(PCOE::MessageId id,
                        std::string source,
                        time_point eta,
                        double lat,
                        double lon,
                        double alt);

        /**
         * Constructs a new instance of @{code ConfigMessage}.
         *
         * @param id        The id of the message.
         * @param source    The source of the message.
         * @param timestamp The time at which the message or the data contained
         *                  by the message was generated.
         * @param eta       The ETA of the waypoint
         * @param lat       The latitude of the waypoint
         * @param lon       The longitude of the waypoint
         * @param alt       The altitude of the waypoint
         **/
        WaypointMessage(PCOE::MessageId id,
                        std::string source,
                        time_point timestamp,
                        time_point eta,
                        double lat,
                        double lon,
                        double alt);

        /**
         * Gets the waypoint ETA.
         **/
        inline time_point getEta() const {
            return eta;
        }

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

    protected:
        std::uint16_t getPayloadSize() const override final;

        void serializePayload(std::ostream& os) const override final;

    private:
        time_point eta;
        double lat;
        double lon;
        double alt;
    };
}
#endif
