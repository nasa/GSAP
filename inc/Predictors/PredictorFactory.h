// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_PREDICTORFACTORY_H
#define PCOE_PREDICTORFACTORY_H

#include "Factory.h"
#include "MonteCarloPredictor.h"
#include "Predictor.h"
#include "Singleton.h"

namespace PCOE {
    /**
     * Creates new @{code Predictor} objects.
     *
     * @author Mattew Daigle
     * @author Jason Watkins
     * @since 1.0
     **/
    class PredictorFactory : public Factory<Predictor,
                                            const PrognosticsModel&,
                                            LoadEstimator&,
                                            const TrajectoryService&,
                                            const ConfigMap&>,
                             public Singleton<PredictorFactory> {
        friend class Singleton<PredictorFactory>;

    private:
        /**
         * Creates a new instance of the @{code PredictorFactory} with default
         * predictors registered. This constructor should only be called once,
         * from the parent @{code Singleton} class.
         **/
        PredictorFactory() {
            Register<MonteCarloPredictor>("MC");
        }
    };
}

#endif
