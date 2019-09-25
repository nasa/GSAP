// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_PROGEVENT_H
#define PCOE_PROGEVENT_H
#include <string>
#include <vector>

#include "Messages/MessageClock.h"
#include "Messages/MessageId.h"
#include "Point4D.h"
#include "UData.h"

namespace PCOE {
    /**
     * Represents data associated with a specific prognostic event such as end
     * of life or end of dischanrge. Data is represented by a time of event
     * (with uncertainty) along with a set of 4D points associated with the
     * event.
     *
     * @remarks
     * In many cases there will be only one event (End of life).
     *
     * @remarks
     * In many cases startTime will be the only parameter that is used. states
     * are for advanced users.
     *
     * @author    Chris Teubert
     * @author    Jason Watkins
     * @author    Julian Vu
     * @version   1.1.0
     **/
    class ProgEvent {
    public:
        /**
         * Creates a new prognostic event
         *
         * @param id     The message ID associated with the event.
         * @param state  The state vector associated with the event.
         * @param toe    The time at which the event will occur.
         * @param points A set of points associated with the event
         * @param tag    A tag that provides aditional information about the event.
         **/
        ProgEvent(MessageId id,
                  std::vector<UData> state,
                  UData toe,
                  std::vector<Point4D<MessageClock>> points = {},
                  std::string tag = "")
            : eventId(id),
              eventState(std::move(state)),
              toe(toe),
              points(std::move(points)),
              tag(std::move(tag)) {}

        /**
         * Gets the message id for the event.
         **/
        inline MessageId getId() const {
            return eventId;
        }

        /**
         * Gets the state vector for the event.
         **/
        inline const std::vector<UData>& getState() const {
            return eventState;
        }

        /**
         * Gets the start time of the event.
         **/
        inline const UData& getTOE() const {
            return toe;
        }

        /**
         * Gets the set of state associated with the event.
         **/
        inline const std::vector<Point4D<MessageClock>> getPoints() const {
            return points;
        }

        /**
         * Gets the tag associated with the event.
         **/
        inline const std::string& getTag() const {
            return tag;
        }

    private:
        MessageId eventId;
        std::vector<UData> eventState;
        UData toe;
        std::vector<Point4D<MessageClock>> points;
        std::string tag;
    };
}

#endif
