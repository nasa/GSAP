// Copyright (c) 2015-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_COMMUNICATORFACTORY_H
#define PCOE_COMMUNICATORFACTORY_H

#include "Communicator.h"
#include "Factory.h"
#include "PlaybackCommunicator.h"
#include "RandomCommunicator.h"
#include "RecorderCommunicator.h"
#include "Singleton.h"

namespace PCOE {
    /**
     * Creates new @{code Communicator} objects.
     *
     * @author Chris Teubert
     * @author Jason Watkins
     * @since 1.0
     **/
    class CommunicatorFactory : public Factory<Communicator, ConfigMap&>,
                                public Singleton<CommunicatorFactory> {
    private:
        friend class Singleton<CommunicatorFactory>;

    private:
        /**
         * Creates a new instance of the @{code CommunicatorFactory} with
         * default communictors registered. This constructor should only be
         * called once, from the parent @{code Singleton} class.
         **/
        CommunicatorFactory() {
            Register<PlaybackCommunicator>("playback");
            Register<RecorderCommunicator>("recorder");
            Register<RandomCommunicator>("random");
        };
    };
}

#endif
