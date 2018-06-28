
#include "Predictor.h"

namespace PCOE {
    Predictor::Predictor(GSAPConfigMap&) : model(nullptr), log(Log::Instance()) {
        using std::placeholders::_1;
        using std::placeholders::_2;
        loadEstFcn = std::bind(&LoadEstimator::estimateLoad, defaultLoadEst, _1, _2);
    }
}
