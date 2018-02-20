/**  Constant Load Estimator - Body
 *   @class     ConstLoadEstimator ConstLoadEstimator.h
 *   @ingroup   GPIC++
 *   @ingroup   Support
 *
 *   @brief     Constant Load Estimator Class- simulates constant loading
 *
 *   The purpose of this class is to handle the logic of estimating future loading in the case
 *   where future loading is constant.
 *
 *   @author    Chris Teubert
 *   @version   1.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoser Configuration Files
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: February, 2018
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include "ThreadSafeLog.h"
#include "ConstLoadEstimator.h"

namespace PCOE {
    const std::string ConstLoadEstimator::LOADING_KEY = "LoadEstimator.loading";
    
    const std::string DEBUG_TAG = "Const Load Estimator";
    
    static Log & log = Log::Instance();
    
    ConstLoadEstimator::ConstLoadEstimator(GSAPConfigMap & configMap) {
        log.WriteLine(LOG_INFO, DEBUG_TAG, "Configuring");
        configMap.checkRequiredParams({LOADING_KEY});
        std::vector<std::string> & profileStrs = configMap[LOADING_KEY];
        for (auto && profileStr : profileStrs) {
            profile.push_back(std::stod(profileStr));
        }
        log.WriteLine(LOG_TRACE, DEBUG_TAG, "Completed configuration");
    }
    
    LoadEstimate ConstLoadEstimator::estimateLoad(const double, const unsigned int) {
        log.WriteLine(LOG_DEBUG, DEBUG_TAG, "Sampling...");
        return profile;
    }
}

