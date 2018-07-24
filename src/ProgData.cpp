// Copyright (c) 2013-2018 United States Government as represented by
// the Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
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
        sysTrajectories.setUncertainty(uncertType);
    }

    void ProgData::setPredictions(const double interval, const unsigned int nPredictions) {
        // Set number of predictions at each lower level
        sysTrajectories.setNTimes(nPredictions);

        times.resize(nPredictions + 1);
        for (unsigned int i = 0; i <= nPredictions; i++) {
            times[i] = i*interval;
        }
    }

    void ProgData::setPredictions(const std::vector<double> & pred) {
        // Set number of predictions at each lower level
        sysTrajectories.setNTimes(static_cast<unsigned int>(pred.size()));

        times.resize(pred.size() + 1);
        for (unsigned int i = 1; i <= pred.size(); i++) {
            // @note: times[0] is allways current time
            times[i] = pred[i - 1];
        }
    }

    void ProgData::addSystemTrajectories(const std::vector<std::string> & names) {
        for (const auto & it : names) {
            sysTrajectories.addNew(it);
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
