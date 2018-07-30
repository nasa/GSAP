/**  GSAP Configuration Map - Body
 *   @class     GSAPConfigMap GSAPConfigMap.h
 *   @ingroup   GPIC++
 *   @ingroup   Support
 *
 *   @brief     GSAP Configuration Map- Extension fo ConfigMap to include GSAP-specific functionality.
 *
 *   @note      This was created as a subclass to maintain the origonal ConfigMap as reusable code outside of the project
 *
 *   @author    Chris Teubert
 *   @version   1.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoster Configuration Files
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: March 30, 2016
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <string>

#include "GSAPConfigMap.h"
#include "ThreadSafeLog.h"  ///< For the log

namespace PCOE {
    // For log
    const std::string MODULE_NAME = "GSAPConfigMap";

    const static Log & logger = Log::Instance();

    void GSAPConfigMap::checkRequiredParams(std::initializer_list<std::string> list) const {
        std::string missingParams;
        for (auto & elem : list) {
            // Check if each element is there
            if (!hasKey(elem)) {
                missingParams += elem + ", ";
            }
        }
        missingParams = missingParams.substr(0, missingParams.size() - 2);  // remove ,

        if (missingParams.size() != 0) {
            // If some elements are missing
            logger.FormatLine(LOG_ERROR, MODULE_NAME, "Missing the following parameters - %s", missingParams.c_str());
            throw std::range_error("Missing required configuration parameters: " + missingParams);
        }
    }
}
