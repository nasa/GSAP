/**  Prognostic Data Points Class- Body
 *   @file      Prognostic Data Points Class Definition
 *   @ingroup   GPIC++
 *   @ingroup   ProgData
 *
 *   @brief     Prognostic Data Points Class is a container for Data Point Objects
 *
 *   @author    Marcus Postell
 *   @author    Chris Teubert
 *   @version   1.1.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: July 26, 2016
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 **/

#include <cmath>
#include "DataPoints.h"


namespace PCOE{

    // *------------------------*
    // |       DataPoints       |
    // *------------------------*
    void DataPoints::setNSamples(const unsigned int nSamplesIn) {
        nSamples = nSamplesIn;
        for (auto & it : dataPts) {
            if (uType != UType::MeanCovar) {
                it.second.setNPoints(nSamples);
            }
        }
    }

    void DataPoints::setUncertainty(const UType uncertType) {
        uType = uncertType;
        // Set uncertainty for each datapoint
        for (auto & it : dataPts) {
            it.second.setUncertainty(uncertType);
            if (uncertType == UType::MeanCovar) {
                // MeanCovar size depends on number of points
                it.second.setNPoints(static_cast<unsigned int>(dataPts.size()));
            }
            else {
                it.second.setNPoints(nSamples);
            }
        }
    }

    std::vector<std::string> DataPoints::getLabels() const {
        std::vector<std::string> Labels;
        for (auto& it : dataPts) {
            Labels.push_back(it.first);
        }

        return Labels;
    }

    void DataPoints::setNTimes(const unsigned int nTimesIn) {
        nTimes = nTimesIn;
        for (auto & it : dataPts) {
            it.second.setNumTimes(nTimesIn);
        }
    }

    void DataPoints::addNew(const std::string & name, const std::string & description = "") {
        dataPts.insert(std::make_pair(name, DataPoint()));
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

    unsigned int DataPoints::size() const {
        return static_cast<unsigned int>(dataPts.size());
    }

    bool DataPoints::includes(const std::string &key) const {
        return dataPts.find(key) != dataPts.end();
    }
}
