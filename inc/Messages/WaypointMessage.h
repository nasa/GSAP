// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_WAYPOINTMESSAGE_H
#define PCOE_WAYPOINTMESSAGE_H
#include "Messages/Message.h"
#include "Messages/MessageId.h"
#include "Point3D.h"

namespace PCOE {
    /**
     * Represents a message containing a single point on a 4D trajectory.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    class WaypointMessage : public PCOE::Message {
    public:
        /**
         * Constructs a new instance of @{code WaypointMessage}.
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
         * Gets the waypoint position
         */
        inline Point3D getPosition() const {
            return point;
        }

        /**
         * Gets the waypoint latitude.
         **/
        inline double getLatitude() const {
            return point.getLatitude();
        }

        /**
         * Gets the waypoint longitude.
         **/
        inline double getLongitude() const {
            return point.getLongitude();
        }

        /**
         * Gets the waypoint altitude.
         **/
        inline double getAltitude() const {
            return point.getAltitude();
        }

    protected:
        std::uint16_t getPayloadSize() const override final;

        void serializePayload(std::ostream& os) const override final;

    private:
        time_point eta;
        Point3D point;
    };
}
#endif
