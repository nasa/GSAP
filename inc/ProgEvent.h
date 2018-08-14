// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_PROGEVENT_H
#define PCOE_PROGEVENT_H
#include <string>

#include "Messages/MessageId.h"
#include "UData.h"

namespace PCOE {
    /**
     * Prognostic Event Class used to handle storage, access, and updating of the data
     * associated with a specific prognostic event (Example: End of life, end of
     * discharge). This data includes the following:
     *   -# Meta Information from ProgMeta
     *   -# timeOfEvent with uncertainty: When the event occurs
     *   -# probMatrix with uncertainty: The probability that the event has occured at
     *      different timestamps
     *   -# occurrenceMatrix: A collection of samples in a simulation for different
     *      timestamps- if the event has occured for each sample. This is the most
     *      detailed representation of uncertainty. probMatrix can be calculated from this
     *
     * @note In many cases there will be only one event (End of life).
     * @note In many cases timeOfEvent will be the only parameter that is used. probMatrix
     *       and occurrenceMatrix is for advanced users.
     *
     * @author    Chris Teubert
     * @author    Jason Watkins
     * @author    Julian Vu
     * @version   1.1.0
     **/
    class ProgEvent {
        friend class ProgEvents;

    public:
        ProgEvent(MessageId id, std::vector<UData>&& state, UData&& toe)
            : eventId(id), eventState(state), timeOfEvent(toe) {}

        ProgEvent(MessageId id, const std::vector<UData>& state, const UData& toe)
            : eventId(id), eventState(state), timeOfEvent(toe) {}

        inline MessageId getId() const {
            return eventId;
        }

        inline const std::vector<UData>& getState() const {
            return eventState;
        }

        inline const UData& getTOE() const {
            return timeOfEvent;
        }

    private:
        MessageId eventId;

        std::vector<UData> eventState;

        UData timeOfEvent;
    };
}

#endif
