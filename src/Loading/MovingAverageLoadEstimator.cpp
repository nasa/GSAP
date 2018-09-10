// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Loading/MovingAverageLoadEstimator.h"
#include "ConfigMap.h"
#include "Contracts.h"

namespace PCOE {
    MovingAverageLoadEstimator::MovingAverageLoadEstimator(const ConfigMap& config) {
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

    void MovingAverageLoadEstimator::addLoad(const LoadEstimator::LoadEstimate& load) {
        Expect(load.size() == currentEstimate.size(), "Size mismatch");
        for (std::size_t i = 0; i < load.size(); ++i) {
            double newEst = load[i] / pastEstimates.size();
            currentEstimate[i] -= pastEstimates[pos][i];
            currentEstimate[i] += newEst;
            pastEstimates[pos][i] = newEst;
        }
        pos = (pos + 1) % pastEstimates.size();
    }

    LoadEstimator::LoadEstimate MovingAverageLoadEstimator::estimateLoad(const double t) {
        static_cast<void>(t);

        return currentEstimate;
    }
}
