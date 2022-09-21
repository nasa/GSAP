// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <sstream>

#include "ConfigMap.h"
#include "Contracts.h"
#include "Loading/LoadEstimatorFactory.h"
#include "Loading/LoadListener.h"
#include "ModelBasedAsyncPrognoserBuilder.h"
#include "Models/PrognosticsModelFactory.h"
#include "Observers/AsyncObserver.h"
#include "Observers/Observer.h"
#include "Observers/ObserverFactory.h"
#include "Predictors/AsyncPredictor.h"
#include "Predictors/PredictorFactory.h"

namespace PCOE {
    const static Log& log = Log::Instance();

    const std::string MODEL_KEY = "model";
    const std::string OBSERVER_KEY = "observer";
    const std::string PREDICTOR_KEY = "predictor";

    const std::string MODULE_NAME = "MBEDPrognoserBuilder";

    ModelBasedAsyncPrognoserBuilder::ModelBasedAsyncPrognoserBuilder(ConfigMap config)
        : AsyncPrognoserBuilder(std::move(config)) {}

    void ModelBasedAsyncPrognoserBuilder::setModelName(const std::string& value) {
        lock_guard guard(m);
        Expect(value.length() > 0, "Model name length");
        config.set(MODEL_KEY, value);
    }

    void ModelBasedAsyncPrognoserBuilder::setObserverName(const std::string& value) {
        lock_guard guard(m);
        Expect(value.length() > 0, "Observer name length");
        config.set(OBSERVER_KEY, value);
    }

    void ModelBasedAsyncPrognoserBuilder::setPredictorName(const std::string& value) {
        lock_guard guard(m);
        Expect(value.length() > 0, "Predictor name length");
        config.set(PREDICTOR_KEY, value);
    }

    AsyncPrognoser
    ModelBasedAsyncPrognoserBuilder::build(PCOE::MessageBus& bus,
                                                 const std::string& sensorSource,
                                                 const std::string& trajectorySource) {
        lock_guard guard(m);
        AsyncPrognoser container(bus);
        PrognosticsModel* progModel = nullptr;
        std::unique_ptr<Observer> observer;
        std::unique_ptr<Predictor> predictor;
        LoadEstimator* loadEstimator = nullptr;
        AsyncTrajectoryService* ts = new AsyncTrajectoryService(
            bus, std::unique_ptr<TrajectoryService>(new TrajectoryService()), trajectorySource);
        container.addEventListener(ts);

        if (config.hasKey(LOAD_ESTIMATOR_KEY)) {
            log.FormatLine(LOG_DEBUG,
                           MODULE_NAME,
                           "Building load estimator %s",
                           config.getString(LOAD_ESTIMATOR_KEY).c_str());
        }
        else {
            log.FormatLine(LOG_INFO,
                           MODULE_NAME,
                           "Using default Load Estimator: %s",
                           DEFAULT_LOAD_ESTIMATOR.c_str());
            config.set(LOAD_ESTIMATOR_KEY, DEFAULT_LOAD_ESTIMATOR);
        }
        auto& leFactory = LoadEstimatorFactory::instance();
        loadEstimator = leFactory.Create(config.getString(LOAD_ESTIMATOR_KEY), config).release();
        container.setLoadEstimator(loadEstimator);

        if (config.hasKey(MODEL_KEY)) {
            std::string modelName = config.getString(MODEL_KEY);
            log.FormatLine(LOG_DEBUG,
                           MODULE_NAME,
                           "Building prognostics model %s",
                           modelName.c_str());
            auto& pmFactory = PrognosticsModelFactory::instance();
            progModel = pmFactory.Create(modelName, config).release();
            container.setModel(progModel);
        }
        else {
            log.WriteLine(LOG_WARN, MODULE_NAME, "No model name found");
        }

        if (config.hasKey(OBSERVER_KEY)) {
            std::string observerName = config.getString(OBSERVER_KEY);
            log.FormatLine(LOG_DEBUG, MODULE_NAME, "Building observer %s", observerName.c_str());
            auto& obsFactory = ObserverFactory::instance();
            Require(progModel, "Observer missing model");
            observer = obsFactory.Create(observerName, *progModel, config);
        }
        else {
            log.WriteLine(LOG_WARN, MODULE_NAME, "No observer name found");
        }

        if (config.hasKey(PREDICTOR_KEY)) {
            std::string predictorName = config.getString(PREDICTOR_KEY);
            log.FormatLine(LOG_DEBUG, MODULE_NAME, "Building predictor %s", predictorName.c_str());
            auto& predFactory = PredictorFactory::instance();
            Require(progModel, "Predictor missing model");
            Require(loadEstimator, "Predictor missing load estimator");
            predictor = predFactory.Create(predictorName,
                                           *progModel,
                                           *loadEstimator,
                                           ts->getTrajectoryService(),
                                           config);
        }
        else {
            log.WriteLine(LOG_WARN, MODULE_NAME, "No predictor name found");
        }

        if (observer) {
            container.addEventListener(
                new AsyncObserver(bus, std::move(observer), sensorSource));
        }

        if (predictor) {
            container.addEventListener(
                new AsyncPredictor(bus, std::move(predictor), sensorSource));
        }

        LoadListener* loadListener = new LoadListener(bus, sensorSource, loadEstimator);
        container.addEventListener(loadListener);

        log.WriteLine(LOG_WARN, MODULE_NAME, "Build complete");
        return container;
    }
}
