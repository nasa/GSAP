// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_OBSERVERFACTORY_H
#define PCOE_OBSERVERFACTORY_H

#include "Factory.h"
#include "Observers/Observer.h"
#include "Observers/ParticleFilter.h"
#include "Observers/UnscentedKalmanFilter.h"
#include "Singleton.h"

namespace PCOE {
    /**
     * Creates new @{code Observer} objects.
     *
     * @author Jason Watkins
     * @since 1.0
     **/
    class ObserverFactory : public Factory<Observer, const Model*, const GSAPConfigMap&>,
                            public Singleton<ObserverFactory> {
        friend class Singleton<ObserverFactory>;

    private:
        /**
         * Creates a new instance of the @{code ObserverFactory} with default
         * observers registered. This constructor should only be called once,
         * from the parent @{code Singleton} class.
         **/
        ObserverFactory() {
            Register<UnscentedKalmanFilter>("UKF");
            Register<ParticleFilter>("PF");
        }
    };
}

#endif
