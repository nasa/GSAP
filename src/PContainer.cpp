/**  Prognostic Container Structure- Body
 *   @file      Prognostic Container Classes
 *   @ingroup   GPIC++
 *   @ingroup   ProgData
 *
 *   @brief     Prognostic Container Structure Classes - Classes used for handling access to a group of ProgEvents or dataPoints
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
 */

#include <cmath> // For NAN

#include "PContainer.h"
namespace PCOE {
    // *------------------------*
    // |       PContainer       |
    // *------------------------*
    PContainer::PContainer() : nTimes(0),
        uType(UType::Point),
        nSamples(1) {
    }

    UType PContainer::getUncertainty() const {
        return uType;
    }

    unsigned int PContainer::getNTimes() const {
        return nTimes;
    }

    unsigned int PContainer::getNSamples() const {
        return nSamples;
    }
}




