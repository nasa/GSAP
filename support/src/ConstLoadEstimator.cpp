
#include "ConstLoadEstimator.h"

namespace PCOE {
    const std::string ConstLoadEstimator::LOADING_KEY = "LoadEstimator.loading";
    
    ConstLoadEstimator::ConstLoadEstimator(GSAPConfigMap & configMap) {
        
        configMap.checkRequiredParams({LOADING_KEY});
        std::vector<std::string> & profileStrs = configMap[LOADING_KEY];
        for (auto && profileStr : profileStrs) {
            profile.push_back(std::stod(profileStr));
        }
    }
    
    LoadEstimate ConstLoadEstimator::estimateLoad(const double) {
        return profile;
    }
}

