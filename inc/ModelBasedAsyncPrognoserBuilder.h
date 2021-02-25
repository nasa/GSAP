// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MODELBASEDEVENTDRIVENPROGNOSERBUILDER_H
#define PCOE_MODELBASEDEVENTDRIVENPROGNOSERBUILDER_H

#include <string>

#include "AsyncPrognoser.h"
#include "AsyncPrognoserBuilder.h"

namespace PCOE {
    const extern std::string MODEL_KEY;
    const extern std::string OBSERVER_KEY;
    const extern std::string PREDICTOR_KEY;

    /**
     * Collects information about a prognostics configuration and builds the
     * necessary classes.
     **/
    class ModelBasedAsyncPrognoserBuilder : public AsyncPrognoserBuilder {
    public:
        /**
         * Initializes a new builder with an empty configuration.
         **/
        ModelBasedAsyncPrognoserBuilder() = default;

        /**
         * Initializes a new builder using the specified configuration.
         **/
        explicit ModelBasedAsyncPrognoserBuilder(ConfigMap config);

        void setModelName(const std::string& value);

        void setObserverName(const std::string& value);

        void setPredictorName(const std::string& value);

        AsyncPrognoser build(PCOE::MessageBus& bus,
                                   const std::string& sensorSource,
                                   const std::string& trajectorySource) override;
    };
}
#endif
