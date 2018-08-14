// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_GAUSSIANLOADESTIMATOR_H
#define PCOE_GAUSSIANLOADESTIMATOR_H
#include <random>

#include "ConfigMap.h"
#include "Contracts.h"
#include "Loading/LoadEstimator.h"

namespace PCOE {
    /**
     * Produces a constant load estimate including gaussian noise. The load
     * estimate is configured when the {@code GaussianLoadEstimator} is created.
     * Each call to {@code estimateLoad} produces a load estimate with the same
     * base load plus per-estimate gaussian nose.
     *
     * @author Chris Teubert
     * @author Jason Watkins
     * @since 1.1
     **/
    class GaussianLoadEstimator final : public LoadEstimator {
    public:
        /**
         * Constructs a new {@code GaussianLoadEstimator} instance.
         *
         * Required Keys:
         * - LoadEstimator.Loading: A vector of doubles defining the base load.
         * - LoadEstmator.StdDev: A single double or a vector of doubles
         *   matching the size of LoadEstimator.Loading.
         *
         * @param config The configuration used to initialize the load
         *               estimator.
         **/
        GaussianLoadEstimator(const ConfigMap& config) {
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
        LoadEstimate estimateLoad(const double t) override {
            static_cast<void>(t);

            std::vector<double> loading(baseLoading.size());
            for (std::size_t i = 0; i < loading.size(); ++i) {
                std::normal_distribution<> dist(baseLoading[i], stdDeviations[i]);
                loading[i] = dist(rng);
            }
            return loading;
        }

    private:
        std::mt19937 rng;
        std::vector<double> baseLoading;
        std::vector<double> stdDeviations;
    };
}
#endif
