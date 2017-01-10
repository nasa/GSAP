/**  Prognostic Data Structure- Body
 *   @file      Prognostic Data Structure Classes
 *   @ingroup   GPIC++
 *   @ingroup   ProgData   
 *
 *   @brief     Prognostic Data Structure Classes - Classes used for storing, distributing, and manipulation prognostic data
 *
 *   @author    Chris Teubert
 *   @version   0.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoster Configuration Files
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: December 8, 2015
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <cmath>  // For NAN

#include "ProgData.h"
#include "DataPoints.h"

namespace PCOE {
    //*------------------------------------------------------------------*
    //|                        Public Functions                          |
    //*------------------------------------------------------------------*
    ProgData::ProgData() : componentName(""),
        prognoserName(""),
        uniqueId("") {
    }

    ProgData::ProgData(const std::string & progName,
        const std::string & compName,
        const std::string & uId)
        : componentName(compName),
        prognoserName(progName),
        uniqueId(uId) {
    }

    void ProgData::setUncertainty(const UType uncertType) {
        // Set uncertainty at each lower level.
        events.setUncertainty(uncertType);
        sysTrajectories.setUncertainty(uncertType);
        futureInputs.setUncertainty(uncertType);
    }

    void ProgData::setPredictions(const double interval, const unsigned int nPredictions) {
        // Set number of predictions at each lower level
        events.setNTimes(nPredictions);
        sysTrajectories.setNTimes(nPredictions);
        futureInputs.setNTimes(nPredictions);

        times.resize(nPredictions + 1);
        for (unsigned int i = 0; i <= nPredictions; i++) {
            times[i] = i*interval;
        }
    }

    void ProgData::setPredictions(const std::vector<double> & pred) {
        // Set number of predictions at each lower level
        events.setNTimes(static_cast<unsigned int>(pred.size()));
        sysTrajectories.setNTimes(static_cast<unsigned int>(pred.size()));
        futureInputs.setNTimes(static_cast<unsigned int>(pred.size()));

        times.resize(pred.size() + 1);
        for (unsigned int i = 1; i <= pred.size(); i++) {
            // @note: times[0] is allways current time
            times[i] = pred[i - 1];
        }
    }

    void ProgData::setupOccurrence(const unsigned int nSamples) {
        auto eventList = events.getLabels();
        // For each event
        for (const auto & it : eventList) {
            // Done this way so DataPoints can stay generic
            events[it].setNumOccurrenceSamples(nSamples);
        }
    }

    void ProgData::addEvents(const std::vector<std::string> & names) {
        // Add vector of events
        for (const auto & it : names) {
            events.addNew(it, "");  // Note: No description
        }
    }

    void ProgData::addSystemTrajectories(const std::vector<std::string> & names) {
        for (const auto & it : names) {
            sysTrajectories.addNew(it, "");
        }
    }

    void ProgData::addInternals(const std::vector<std::string> & names) {
        for (const auto & it : names) {
            internals[it] = NAN;
        }
    }

    std::vector<std::string> ProgData::getInternalNames() const {
        std::vector<std::string> Labels;
        for (const auto & it : internals) {
            Labels.push_back(it.first);
        }

        return Labels;
    }
}
