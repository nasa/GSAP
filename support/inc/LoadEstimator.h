#ifndef PCOE_LOAD_ESTIMATOR_H
#define PCOE_LOAD_ESTIMATOR_H

#include <vector>

#include "GSAPConfigMap.h"

namespace PCOE {
    typedef std::vector<double> LoadEstimate;
    class LoadEstimator {
     public:
        virtual LoadEstimate estimateLoad(const double t) = 0;
    };
}
#endif //PCOE_LOAD_ESTIMATOR_H
