// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MOVINAVERAGELOADESTIMATOR_H
#define PCOE_MOVINAVERAGELOADESTIMATOR_H

#include "ConfigMap.h"
#include "Contracts.h"
#include "Loading/LoadEstimator.h"

namespace PCOE {
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
        static const std::string
            WINDOW_SIZE_KEY; // Key for window size (number of steps in average window)
        static const size_t DEFAULT_WINDOW_SIZE;

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
        MovingAverageLoadEstimator(const ConfigMap& config) {
            const std::string LOADING_KEY = "LoadEstimator.Loading";
            const std::string WINDOW_KEY = "LoadEstimator.Window";
            requireKeys(config, {LOADING_KEY});

            std::size_t window = 10;
            if (config.hasKey(WINDOW_KEY)) {
                window = config.getSize(WINDOW_KEY);
            }

            currentEstimate = config.getDoubleVector(LOADING_KEY);

            std::vector<double> partialEstimate(currentEstimate.size());
            for (std::size_t i = 0; i < currentEstimate.size(); ++i) {
                partialEstimate[i] = currentEstimate[i] / window;
            }

            pastEstimates.resize(window, partialEstimate);
        }

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
        void addLoad(const LoadEstimate& load) override {
            Expect(load.size() == currentEstimate.size(), "Size mismatch");
            for (std::size_t i = 0; i < load.size(); ++i) {
                double newEst = load[i] / pastEstimates.size();
                currentEstimate[i] -= pastEstimates[pos][i];
                currentEstimate[i] += newEst;
                pastEstimates[pos][i] = newEst;
            }
            pos = (pos + 1) % pastEstimates.size();
        }

        /**
         * Gets the current load estimate.
         *
         * @param t      Not used.
         * @return       The current estimated load.
         **/
        LoadEstimate estimateLoad(const double t) override {
            static_cast<void>(t);

            return currentEstimate;
        }

    protected:
        std::size_t pos = 0;
        std::vector<std::vector<double>> pastEstimates;
        std::vector<double> currentEstimate;
    };
}
#endif
