// Copyright (c) 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#ifndef PCOE_PROFILELOADESTIMATOR_H
#define PCOE_PROFILELOADESTIMATOR_H

#include <chrono>

#include "Loading/LoadEstimator.h"

namespace PCOE {
    class ConfigMap;

    /**
     * Produces a load estimate based on a profile. The load estimate is configured when
     * the {@code ProfiletLoadEstimator}::{@code estimateLoad} is first called, and all subsequent
     * calls to {@code estimateLoad} produce the same load estimate.
     *
     * @author Chris Teubert
     * @since July, 2020
     **/
    class ProfileLoadEstimator final : public LoadEstimator {
    public:
        /**
         * Constructs a new {@code ProfileLoadEstimator} instance.
         *
         * Required Keys:
         * - LoadEstimator.Loading: A vector of doubles defining the base load.
         *
         * @param config The configuration used to initialize the load
         *               estimator.
         **/
       ProfileLoadEstimator(const ConfigMap& config);

        /**
         * Returns the loading for a specific time.
         *
         * @param t      Current time in seconds
         * @return       The estimated load for time t
         **/
        LoadEstimate estimateLoad(const double t) override;

    private:
        /**
         *  Profile in format (duration (s), Load Estimate)
         **/
        using ProfileElement = std::pair<double, LoadEstimate>;
        std::vector<ProfileElement> profile;

        double startT_s; // First time that the profile load estimator is initialized to.
        bool is_started = false;
    };
}
#endif 
