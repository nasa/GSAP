/**  GSAP Configuration Map - Header
 *   @class     GSAPConfigMap GSAPConfigMap.h
 *   @ingroup   GPIC++
 *   @ingroup   Support
 *
 *   @brief     GSAP Configuration Map- Extension fo ConfigMap to include GSAP-specific functionality.
 *
 *   @note      This was created as a subclass to maintain the origonal ConfigMap as reusable code outside of the project
 *
 *   @author    Chris Teubert
 *   @version   0.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoster Configuration Files
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: March 30, 2016
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_GSAPCONFIGMAP_H
#define PCOE_GSAPCONFIGMAP_H

#include <string>
#include <initializer_list>

#include "ConfigMap.h"

namespace PCOE {
    class GSAPConfigMap : public ConfigMap {
    public:
        GSAPConfigMap() = default;
        GSAPConfigMap(const std::string &filename) { loadFile(filename); }
        
        /** @function   checkRequiredParams
         *  @brief      Check if all required parameters are present in the configuration map, if they are not, write to the log and throw an error
         *  @param      list    Parameters to check
         *
         *  @example    config.checkRequiredParams({"param1", "param2"});
         **/
        void checkRequiredParams(std::initializer_list<std::string> list) const;
    };
}

#endif // PCOE_GSAPCONFIGMAP_H
