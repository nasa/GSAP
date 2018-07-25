// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_EVENTDRIVENPROGNOSER_H
#define PCOE_EVENTDRIVENPROGNOSER_H
#include <memory>

#include "GSAPConfigMap.h"
#include "LoadEstimatorFactory.h"
#include "Messages/MessageBus.h"
#include "Observers/EventDrivenObserver.h"
#include "Observers/ObserverFactory.h"
#include "Predictors/EventDrivenPredictor.h"
#include "Predictors/PredictorFactory.h"
#include "PrognosticsModelFactory.h"

namespace PCOE {
    const std::string SOURCE_KEY = "Source";
    const std::string MODEL_KEY = "Model";
    const std::string OBSERVER_KEY = "Observer";
    const std::string PREDICTOR_KEY = "Predictor";
    const std::string LOAD_EST_KEY = "LoadEstimator";

    /**
     * Manages event driven components.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    class EventDrivenPrognoser {
    public:
        // TODO (JW): Figure out how to make this sane
        EventDrivenPrognoser(MessageBus& bus, const GSAPConfigMap& config)
            : bus(bus),
              model(PrognosticsModelFactory::instance().Create(config.at(MODEL_KEY)[0], config)),
              loadEstimator(
                  LoadEstimatorFactory::instance().Create(config.at(LOAD_EST_KEY)[0], config)),
              observer(bus,
                       ObserverFactory::instance().Create(config.at(OBSERVER_KEY)[0],
                                                          model.get(),
                                                          config),
                       config.at(SOURCE_KEY)[0]),
              predictor(bus,
                        PredictorFactory::instance().Create(config.at(PREDICTOR_KEY)[0],
                                                            model.get(),
                                                            loadEstimator.get(),
                                                            config),
                        config.at(SOURCE_KEY)[0]) {}

    private:
        MessageBus& bus;
        std::unique_ptr<PrognosticsModel> model;
        std::unique_ptr<LoadEstimator> loadEstimator;
        EventDrivenObserver observer;
        EventDrivenPredictor predictor;
    };
}

#endif
