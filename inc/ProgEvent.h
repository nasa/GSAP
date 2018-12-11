// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_PROGEVENT_H
#define PCOE_PROGEVENT_H
#include <string>

#include "Messages/MessageId.h"
#include "Point3D.h"
#include "UData.h"

namespace PCOE {
    /**
     * Reprsents data associated with a specific prognostic event such as end of
     * life or end of dischanrge. Data is represented by a start and end time
     * (which may be the same for an instantaneous event) along with start and
     * end position and a set of states associated with the event.
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
         * Creates a new prognostic event with an instantaneous time of event.
         *
         * @remarks
         * This constructor should be used for events which are instantaneous in
         * nature and do not have any particular location associated with them.
         * The event start and end times will be set to {@p toe}, and the start
         * and end locations will be set to to the origin.
         *
         * @param id    The message ID associated with the event.
         * @param state The state vector associated with the event.
         * @param toe   The time at which the event will occur.
         **/
        ProgEvent(MessageId id, const std::vector<UData>& state, const UData& toe)
            : ProgEvent(id, state, toe, toe) {}

        /**
         * Creates a new prognostic event with an instantaneous time of event.
         *
         * @remarks
         * This constructor should be used for events which are instantaneous in
         * nature, but do have a location associated with them.
         * The event start and end times will be set to {@p toe}, and the start
         * and end locations will be set to to {@p pos}.
         *
         * @param id    The message ID associated with the event.
         * @param state The state vector associated with the event.
         * @param toe   The time at which the event will occur.
         * @param pos   The location at which the event will occur.
         **/
        ProgEvent(MessageId id, const std::vector<UData>& state, const UData& toe, Point3D pos)
            : ProgEvent(id, state, toe, pos, toe, pos) {}

        /**
         * Creates a new prognostic event with the specified start and end
         * times from r-value references.
         *
         * @remarks
         * This constructor should be used for events which are transient in
         * nature and do not have any particular location associated with them.
         * The event start and end locations will be set to the origin.
         *
         * @param id    The message ID associated with the event.
         * @param state The state vector associated with the event.
         * @param start The time at which the event will start.
         * @param end   The time at which the event will end.
         **/
        ProgEvent(MessageId id, std::vector<UData>&& state, UData&& start, UData&& end)
            : eventId(id), eventState(state), startTime(start), endTime(end) {}

        /**
         * Creates a new prognostic event with the specified start and end
         * times.
         *
         * @remarks
         * This constructor should be used for events which are transient in
         * nature and do not have any particular location associated with them.
         * The event start and end locations will be set to the origin.
         *
         * @param id    The message ID associated with the event.
         * @param state The state vector associated with the event.
         * @param start The time at which the event will start.
         * @param end   The time at which the event will end.
         **/
        ProgEvent(MessageId id,
                  const std::vector<UData>& state,
                  const UData& start,
                  const UData& end)
            : eventId(id), eventState(state), startTime(start), endTime(end) {}

        /**
         * Creates a new prognostic event from r-value references.
         *
         * @param id        The message ID associated with the event.
         * @param state     The state vector associated with the event.
         * @param startTime The time at which the event will start.
         * @param startPos  The location at which the event will start.
         * @param endTime   The time at which the event will end.
         * @param endPos    The location at which the event will end.
         **/
        ProgEvent(MessageId id,
                  std::vector<UData>&& state,
                  UData&& startTime,
                  Point3D startPos,
                  UData&& endTime,
                  Point3D endPos)
            : eventId(id),
              eventState(state),
              startTime(startTime),
              startPosition(startPos),
              endTime(endTime),
              endPosition(endPos) {}

        /**
         * Creates a new prognostic event.
         *
         * @param id        The message ID associated with the event.
         * @param state     The state vector associated with the event.
         * @param startTime The time at which the event will start.
         * @param startPos  The location at which the event will start.
         * @param endTime   The time at which the event will end.
         * @param endPos    The location at which the event will end.
         **/
        ProgEvent(MessageId id,
                  const std::vector<UData>& state,
                  const UData& startTime,
                  Point3D startPos,
                  const UData& endTime,
                  Point3D endPos)
            : eventId(id),
              eventState(state),
              startTime(startTime),
              startPosition(startPos),
              endTime(endTime),
              endPosition(endPos) {}

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
        inline const UData& getStartTime() const {
            return startTime;
        }

        /**
         * Gets the start position of the event.
         *
         * @remarks
         * For events that do not map to a particular location, this property
         * will generally be set to the origin.
         **/
        inline Point3D getStartPosition() const {
            return startPosition;
        }

        /**
         * Gets the end time of the event.
         *
         * @remarks
         * For events that are instantaneous in nature, this property will
         * generally be the same as the start time.
         **/
        inline const UData& getEndTime() const {
            return endTime;
        }

        /**
         * Gets the end position of the event.
         *
         * @remarks
         * For events that do not map to a particular location, this property
         * will generally be set to the origin. For events that are
         * instantaneous in nature, this property will generally be set to the
         * start position.
         **/
        inline Point3D getEndPosition() const {
            return endPosition;
        }

    private:
        MessageId eventId;
        std::vector<UData> eventState;
        UData startTime;
        Point3D startPosition;
        UData endTime;
        Point3D endPosition;
    };
}

#endif
