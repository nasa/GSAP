// Copyright (c) 2016-2018 United States Government as represented by
// the Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <cmath>

#include "ProgEvent.h"

namespace PCOE {
    // *------------------------*
    // |     Public Functions   |
    // *------------------------*

    ProgEvent::ProgEvent() : ProgMeta() {}

    void ProgEvent::setUncertainty(const UType uncertType) {
        // NOTE: LOSE ALL SAVED DATA
        timeOfEvent.uncertainty(uncertType);
        eventState.uncertainty(uncertType);
    }

    UType ProgEvent::getUncertainty() const {
        return timeOfEvent.uncertainty();
    }

    void ProgEvent::setNPoints(const unsigned int sampleCount) {
        timeOfEvent.npoints(sampleCount);
        eventState.npoints(sampleCount);
    }

    const UData &ProgEvent::getEventState() const {
        return eventState;
    }

    UData& ProgEvent::getEventState() {
        return eventState;
    }

    void ProgEvent::setEventState(const UData &eventState) {
        ProgEvent::eventState = eventState;
    }
}
