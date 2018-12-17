// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MODELBASEDEVENTDRIVENPROGNOSERBUILDER_H
#define PCOE_MODELBASEDEVENTDRIVENPROGNOSERBUILDER_H

#include <string>

#include "EventDrivenPrognoser.h"
#include "EventDrivenPrognoserBuilder.h"

namespace PCOE {
    const extern std::string MODEL_KEY;
    const extern std::string OBSERVER_KEY;
    const extern std::string PREDICTOR_KEY;
    
    /**
     * Collects information about a prognostics configuration and builds the
     * necessary classes.
     **/
    class ModelBasedEventDrivenPrognoserBuilder : public EventDrivenPrognoserBuilder {
    public:
        void setModelName(const std::string& value, bool isPrognosticsModel = false);

        void setObserverName(const std::string& value);

        void setPredictorName(const std::string& value);

        EventDrivenPrognoser build(PCOE::MessageBus& bus,
                                   const std::string& sensorSource,
                                   const std::string& trajectorySource) override;

    private:
        bool modelIsPrognosticsModel;
    };
}
#endif
