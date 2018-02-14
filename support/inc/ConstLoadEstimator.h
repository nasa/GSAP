
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
