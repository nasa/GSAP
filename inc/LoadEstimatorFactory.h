// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_LOADESTIMATORFACTORY_H
#define PCOE_LOADESTIMATORFACTORY_H

#include "ConstLoadEstimator.h"
#include "Factory.h"
#include "LoadEstimator.h"
#include "MovingAverageLoadEstimator.h"
#include "Singleton.h"

namespace PCOE {
    /**
     * Creates new @{code LoadEstimator} objects.
     *
     * @author Chris Teubert
     * @author Jason Watkins
     * @since 1.1
     **/
    class LoadEstimatorFactory : public Factory<LoadEstimator, const GSAPConfigMap&>,
                                 public Singleton<LoadEstimatorFactory> {
        friend class Singleton<LoadEstimatorFactory>;

    private:
        /**
         * Creates a new instance of the @{code LoadEstimatorFactory} with
         * default load estimators registered. This constructor should only be
         * called once, from the parent @{code Singleton} class.
         **/
        LoadEstimatorFactory() {
            Register<ConstLoadEstimator>("const");
            Register<MovingAverageLoadEstimator>("movingAverage");
        };
    };
}

#endif
