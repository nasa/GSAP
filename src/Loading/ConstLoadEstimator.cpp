// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Loading/ConstLoadEstimator.h"
#include "ConfigMap.h"

namespace PCOE {
    ConstLoadEstimator::ConstLoadEstimator(const ConfigMap& config) {
        const std::string LOADING_KEY = "LoadEstimator.Loading";
        requireKeys(config, {LOADING_KEY});

        loading = config.getDoubleVector(LOADING_KEY);
    }

    /**
     * Returns the loading configured when the current instance was
     * initialized.
     *
     * @param t      Not used.
     * @return       The current estimated load.
     **/
    LoadEstimator::LoadEstimate ConstLoadEstimator::estimateLoad(const double t) {
        static_cast<void>(t);

        return loading;
    }
}
