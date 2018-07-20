// Copyright (c) 2015-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_PROGNOSERFACTORY_H
#define PCOE_PROGNOSERFACTORY_H

#include "Factory.h"
#include "ModelBasedPrognoser.h"
#include "Prognoser.h"
#include "Singleton.h"

namespace PCOE {
    /**
     * Creates new @{code Prognoser} objects.
     *
     * @author Jason Watkins
     * @since 1.0
     **/
    class PrognoserFactory : public Factory<Prognoser, GSAPConfigMap&>,
                             public Singleton<PrognoserFactory> {
        friend class Singleton<PrognoserFactory>; // Needed for singleton
    private:
        /**
         * Creates a new instance of the @{code PrognoserFactory} with default
         * prognosers registered. This constructor should only be called once,
         * from the parent @{code Singleton} class.
         **/
        PrognoserFactory() {
            Register<ModelBasedPrognoser>("ModelBasedPrognoser");
        }
    };
}

#endif
