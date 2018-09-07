// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_GAUSSIANLOADESTIMATOR_H
#define PCOE_GAUSSIANLOADESTIMATOR_H
#include <random>

#include "Loading/LoadEstimator.h"

namespace PCOE {
    class ConfigMap;

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
        GaussianLoadEstimator(const ConfigMap& config);

        /**
         * Returns the loading configured when the current instance was
         * initialized with gaussian noise added.
         *
         * @param t      Not used.
         * @return       The current estimated load.
         **/
        LoadEstimate estimateLoad(const double t) override;

    private:
        std::mt19937 rng;
        std::vector<double> baseLoading;
        std::vector<double> stdDeviations;
    };
}
#endif
