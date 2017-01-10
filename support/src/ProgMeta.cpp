/**  Prognostic Meta Abstract Class- Body
 *   @file      Prognostic Meta Abstract Class
 *   @ingroup   GPIC++
 *   @ingroup   ProgData
 *
 *   @brief     Prognostic Meta data abstract class definition- the class included is used to store, access, and update prognostic container meta information.
 *
 *   @author    Chris Teubert
 *   @version   0.1.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: March 3, 2016
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 **/

#include "ProgMeta.h"

namespace PCOE {
    ProgMeta::ProgMeta() :name(""),
        description("") {
    }

    std::string ProgMeta::getName() const {
        return name;
    }

    void ProgMeta::setName(const std::string & nameIn) {
        name = nameIn;
    }

    std::string ProgMeta::getDesc() const {
        return description;
    }

    void ProgMeta::setDesc(const std::string & descIn) {
        description = descIn;
    }

    void ProgMeta::setMeta(const std::string & nameIn, const std::string & descIn) {
        name = nameIn;
        description = descIn;
    }
}
