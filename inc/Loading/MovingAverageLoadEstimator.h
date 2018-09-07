// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MOVINAVERAGELOADESTIMATOR_H
#define PCOE_MOVINAVERAGELOADESTIMATOR_H
#include "Loading/LoadEstimator.h"

namespace PCOE {
    class ConfigMap;

    /**
     * Produces a constant load estimate that is the average of several previous
     * loads.
     *
     * @author Chris Teubert
     * @author Jason Watkins
     * @since 1.1
     **/
    class MovingAverageLoadEstimator final : public LoadEstimator {
    public:
        // The window is the number of samples considered in the moving average.
        static const std::string WINDOW_SIZE_KEY;

        /**
         * Constructs a new {@code MovingAverageLoadEstimator} instance.
         *
         * Required Keys:
         * - LoadEstimator.Loading: A vector of doubles defining the base load.
         *   This sets the initial loading and determines the size of the load
         *   estimate vector.
         *
         * Optional Keys:
         * - LoadEstmator.Window: The number of previous samples to base the
         *   current load estimate on.
         *
         * @param config The configuration used to initialize the load
         *               estimator.
         **/
        MovingAverageLoadEstimator(const ConfigMap& config);

        /**
         * Returns a value indicating that the load estimator supports
         * historical loading. In fact, the {@code MovingAverageLoadEstimator}
         * requires historical loading.
         **/
        bool canAddLoad() override {
            return true;
        }

        /**
         * Updates the moving average load.
         *
         * @param load The new load to add.
         **/
        void addLoad(const LoadEstimate& load) override;

        /**
         * Gets the current load estimate.
         *
         * @param t      Not used.
         * @return       The current estimated load.
         **/
        LoadEstimate estimateLoad(const double t) override;

    protected:
        std::size_t pos = 0;
        std::vector<std::vector<double>> pastEstimates;
        std::vector<double> currentEstimate;
    };
}
#endif
