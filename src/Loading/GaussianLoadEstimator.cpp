// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Loading/GaussianLoadEstimator.h"
#include "ConfigMap.h"
#include "Contracts.h"

namespace PCOE {
    GaussianLoadEstimator::GaussianLoadEstimator(const ConfigMap& config) {
        const std::string LOADING_KEY = "LoadEstimator.Loading";
        const std::string STDDEV_KEY = "LoadEstimator.StdDev";
        requireKeys(config, {LOADING_KEY, STDDEV_KEY});

        baseLoading = config.getDoubleVector(LOADING_KEY);
        if (config.getVector(STDDEV_KEY).size() == 1) {
            Require(stdDeviations.size() == 0, "StdDev initial size");
            stdDeviations.resize(baseLoading.size(), config.getDouble(STDDEV_KEY));
        }
        else {
            stdDeviations = config.getDoubleVector(STDDEV_KEY);
            Require(stdDeviations.size() == baseLoading.size(), "StdDev config size");
        }

        std::random_device rd;
        rng.seed(rd());
    }

    /**
     * Returns the loading configured when the current instance was
     * initialized with gaussian noise added.
     *
     * @param t      Not used.
     * @return       The current estimated load.
     **/
    LoadEstimator::LoadEstimate GaussianLoadEstimator::estimateLoad(const double t) {
        static_cast<void>(t);

        std::vector<double> loading(baseLoading.size());
        for (std::size_t i = 0; i < loading.size(); ++i) {
            std::normal_distribution<> dist(baseLoading[i], stdDeviations[i]);
            loading[i] = dist(rng);
        }
        return loading;
    }
}
