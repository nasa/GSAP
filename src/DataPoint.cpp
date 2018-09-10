/**  Prognostic Data Point Class- Body
 *   @file      Prognostic Data Point Class Definition
 *   @ingroup   GPIC++
 *   @ingroup   ProgData
 *
 *   @brief     Prognostic Data Point Class used to handle storage, access, and updating of the data associated with a specific prognostic data point (Example: state of health). This data includes the following:
 *              1. Meta Information from ProgMeta
 *              2. The value of the data point at different times with uncertainty
 *
 *   @author    Chris Teubert
 *   @version   1.1.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: March 3, 2016
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 **/

#include <cmath> // for NAN

#include "DataPoint.h"

namespace PCOE {
    // *------------------------*
    // |     Public Functions   |
    // *------------------------*

    DataPoint::DataPoint() : uType(UType::Point),
        nPoints(0) {
        setNumTimes(0);  // Default = 1 timestep (NOW)
    }

    UData& DataPoint::operator[](const std::size_t index) {
        return data.at(index);
    }

    void DataPoint::setUncertainty(const UType uncertType) {
        uType = uncertType;
        for (auto & it : data) {
            it.uncertainty(uncertType);
        }
    }

    UType DataPoint::getUncertainty() const {
        return uType;
    }

    void DataPoint::setNumTimes(const unsigned int nTimesIn) {
        data.resize(nTimesIn + 1, UData(uType));
        for (auto & it : data) {
            it.npoints(nPoints);
        }
    }

    unsigned int DataPoint::getNumTimes() const {
        return static_cast<unsigned int>(data.size() - 1);
    }

    void DataPoint::setNPoints(const unsigned int nPointsIn) {
        nPoints = nPointsIn;
        for (auto & it : data) {
            it.npoints(nPoints);
        }
    }  // Run for all when adding a new one

    // *------------------------*
    // |    Private Functions   |
    // *------------------------*

    unsigned int DataPoint::getNPoints() const {
        return nPoints;
    }
}
