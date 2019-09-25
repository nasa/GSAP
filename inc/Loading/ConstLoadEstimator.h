// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_CONSTLOADESTIMATOR_H
#define PCOE_CONSTLOADESTIMATOR_H

#include "Loading/LoadEstimator.h"

namespace PCOE {
    class ConfigMap;

    /**
     * Produces a constant load estimate. The load estimate is configured when
     * the {@code ConstLoadEstimator} is created and all calls to
     * {@code estimateLoad} produce the same load estimate.
     *
     * @author Chris Teubert
     * @author Jason Watkins
     * @since 1.1
     **/
    class ConstLoadEstimator final : public LoadEstimator {
    public:
        /**
         * Constructs a new {@code ConstLoadEstimator} instance.
         *
         * Required Keys:
         * - LoadEstimator.Loading: A vector of doubles defining the base load.
         *
         * @param config The configuration used to initialize the load
         *               estimator.
         **/
        ConstLoadEstimator(const ConfigMap& config);

        /**
         * Returns the loading configured when the current instance was
         * initialized.
         *
         * @param t      Not used.
         * @return       The current estimated load.
         **/
        LoadEstimate estimateLoad(const double t) override;

    private:
        std::vector<double> loading;
    };
}
#endif
