/**  Prognostic Event Class- Body
 *   @file      Prognostic Event Class Definition
 *   @ingroup   GPIC++
 *   @ingroup   ProgData
 *
 *   @brief     Prognostic Event Class used to handle storage, access, and
 *              updating of the data associated with a specific prognostic event
 *              (Example: End of life, end of discharge). This data includes the
 *              following:
 *              1. Meta Information from ProgMeta
 *              2. timeOfEvent with uncertainty: When the event occurs
 *              3. probMatrix with uncertainty: The probability that the event
 *                 has occured at different timestamps
 *              4. occurrenceMatrix: A collection of samples in a simulation for
 *                 different timestamps- if the event has occured for each
 *                 sample. This is the most detailed representation of
 *                 uncertainty. probMatrix can be calculated from this
 *
 *   @note      In many cases timeOfEvent will be the only parameter that is
 *              used. probMatrix and occurrenceMatrix is for advanced users.
 *
 *   @author    Chris Teubert
 *   @author    Jason Watkins
 *   @version   0.1.0
 *
 *   Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *   Created: March 3, 2016
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 **/

#include <cmath>

#include "ProgEvent.h"

namespace PCOE {
    // *------------------------*
    // |     Public Functions   |
    // *------------------------*

    ProgEvent::ProgEvent() : ProgMeta(), nSamples(0) {
        probMatrix.resize(1, NAN);
        occurrenceMatrix.resize(1, std::vector<bool>(nSamples));
    }

    void ProgEvent::setUncertainty(const UType uncertType) {
        // NOTE: LOSE ALL SAVED DATA
        timeOfEvent.uncertainty(uncertType);
    }

    UType ProgEvent::getUncertainty() const {
        return timeOfEvent.uncertainty();
    }

    void ProgEvent::setNumTimes(const unsigned int nTimes) {
        probMatrix.resize(nTimes + 1, NAN); // +1 for NOW
        occurrenceMatrix.resize(nTimes + 1, std::vector<bool>(nSamples));
    }

    unsigned int ProgEvent::getNumTimes() const {
        return static_cast<unsigned int>(probMatrix.size() - 1);
    }

    void ProgEvent::setNumOccurrenceSamples(const unsigned int nSamplesIn) {
        nSamples = nSamplesIn;
        for (auto& it : occurrenceMatrix) {
            it.resize(nSamples);
        }
    }

    unsigned int ProgEvent::getNumOccurrenceSamples() const {
        return nSamples;
    }

    // *------------------------*
    // |    Private Functions   |
    // *------------------------*

    void ProgEvent::setNPoints(const unsigned int nEvents) {
        timeOfEvent.npoints(nEvents);
    }
}
