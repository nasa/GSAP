/**  Prognostic Events Class- Body
 *   @file      Prognostic Events Class Definition
 *   @ingroup   GPIC++
 *   @ingroup   ProgData
 *
 *   @brief     Prognostic Events Class used to handle storage, access, and updating of a collection of Prognostic Event objects
 *
 *   @note      In many cases there will be only one event (End of life).
 *
 *   @author    Marcus Postell
 *   @author    Chris Teubert
 *   @version   0.1.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: July 26, 2016
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 **/
#include <cmath>
#include "ProgEvents.h"

// *------------------------*
// |       ProgEvents       |
// *------------------------*
void ProgEvents::setNSamples(const unsigned int nSamplesIn) {
    nSamples = nSamplesIn;
    for (auto & it : dataPts) {
        if (uType != UType::MeanCovar) {
            it.second.setNPoints(nSamples);
        }
    }
}
void ProgEvents::setUncertainty(const UType uncertType) {
    uType = uncertType;
    // Set uncertainty for each datapoint
    for (auto & it : dataPts) {
        it.second.setUncertainty(uncertType);
        if (uncertType == UType::MeanCovar) {
            it.second.setNPoints(static_cast<unsigned int>(dataPts.size()));
        }
        else {
            it.second.setNPoints(nSamples);
        }
    }
}

std::vector<std::string> ProgEvents::getLabels() const {
    std::vector<std::string> Labels;
    for (auto & it : dataPts) {
        Labels.push_back(it.first);
    }

    return Labels;
}

void ProgEvents::setNTimes(const unsigned int nTimesIn) {
    nTimes = nTimesIn;
    for (auto & it : dataPts) {
        it.second.setNumTimes(nTimesIn);
    }
}

void ProgEvents::addNew(const std::string & name, const std::string & description = "") {
    dataPts.insert(std::make_pair(name, ProgEvent()));
    dataPts[name].setMeta(name, description);
    dataPts[name].setUncertainty(uType);
    dataPts[name].setNumTimes(nTimes);

    // Resize
    for (auto & it : dataPts) {
        if (it.second.getUncertainty() == UType::MeanCovar) {
            it.second.setNPoints(static_cast<unsigned int>(dataPts.size()));
        }
        else {
            it.second.setNPoints(nSamples);
        }

        it.second.setNumTimes(nTimes);
    }
}

unsigned int ProgEvents::size() const {
    return static_cast<unsigned int>(dataPts.size());
}

bool ProgEvents::includes(const std::string & key) const {
    return dataPts.find(key) != dataPts.end();
}
