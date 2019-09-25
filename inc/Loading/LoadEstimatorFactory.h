// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_LOADESTIMATORFACTORY_H
#define PCOE_LOADESTIMATORFACTORY_H

#include "Factory.h"
#include "Loading/ConstLoadEstimator.h"
#include "Loading/GaussianLoadEstimator.h"
#include "Loading/LoadEstimator.h"
#include "Loading/MovingAverageLoadEstimator.h"
#include "Singleton.h"

namespace PCOE {
    /**
     * Creates new @{code LoadEstimator} objects.
     *
     * @author Chris Teubert
     * @author Jason Watkins
     * @since 1.1
     **/
    class LoadEstimatorFactory : public Factory<LoadEstimator, const ConfigMap&>,
                                 public Singleton<LoadEstimatorFactory> {
        friend class Singleton<LoadEstimatorFactory>;

    private:
        /**
         * Creates a new instance of the @{code LoadEstimatorFactory} with
         * default load estimators registered. This constructor should only be
         * called once, from the parent @{code Singleton} class.
         **/
        LoadEstimatorFactory() {
            Register<ConstLoadEstimator>("Const");
            Register<GaussianLoadEstimator>("Gaussian");
            Register<MovingAverageLoadEstimator>("MovingAverage");
        };
    };
}

#endif
