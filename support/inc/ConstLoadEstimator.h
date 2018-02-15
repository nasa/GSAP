
#ifndef PCOE_CONST_LOAD_EST_H
#define PCOE_CONST_LOAD_EST_H

#include "LoadEstimator.h"

namespace PCOE {
    class ConstLoadEstimator : public LoadEstimator {
     public:
        static const std::string LOADING_KEY;

        ConstLoadEstimator(GSAPConfigMap & configMap);
        
        LoadEstimate estimateLoad(const double);
        
     protected:
        LoadEstimate profile;

    };
}
#endif //PCOE_CONST_LOAD_EST_H
