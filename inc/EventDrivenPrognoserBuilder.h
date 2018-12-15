// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#ifndef PCOE_EVENTDRIVENPROGNOSERBUILDER_H
#define PCOE_EVENTDRIVENPROGNOSERBUILDER_H

#include <string>

#include "EventDrivenPrognoser.h"

namespace PCOE {
    /**
     * Collects information about a prognostics configuration and builds the
     * necessary classes.
     * @author Jason Watkins
     * @author Chris Teubert
     **/
    class EventDrivenPrognoserBuilder {
    public:
        void setLoadEstimatorName(const std::string& value);
        
        void importConfig(const ConfigMap& config);
        
        void setConfigParam(const std::string& key, const std::string& value);
        
        virtual EventDrivenPrognoser build(PCOE::MessageBus& bus,
                                   const std::string& sensorSource,
                                   const std::string& trajectorySource);
        
        virtual void reset();
        
    protected:
        using mutex = std::mutex;
        using lock_guard = std::lock_guard<mutex>;
        
        mutable mutex m;
        PCOE::ConfigMap config;
        std::string loadEstimatorName;
    };
    
    /**
     * Function called at program startup to register models, observers and
     * predictors that are not part of core GSAP, and to perform any related
     * initialization tasks.
     **/
    void prognoserInit();
}

#endif
