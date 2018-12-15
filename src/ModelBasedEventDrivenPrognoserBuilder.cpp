// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <sstream>

#include "ConfigMap.h"
#include "Contracts.h"
#include "ModelBasedEventDrivenPrognoserBuilder.h"
#include "Loading/LoadEstimatorFactory.h"
#include "Models/PrognosticsModelFactory.h"
#include "Models/SystemModelFactory.h"
#include "Observers/Observer.h"
#include "Observers/ObserverFactory.h"
#include "Predictors/PredictorFactory.h"

static std::vector<std::string> split(const std::string& value) {
    std::istringstream ss(value);
    std::string token;
    std::vector<std::string> result;

    while (std::getline(ss, token, ',')) {
        result.push_back(token);
    }

    return result;
}

namespace PCOE {
    const static Log& log = Log::Instance();
    
    const std::string MODULE_NAME = "MBEDPrognoserBuilder";
    
    class DummyModel final : public PrognosticsModel {
    public:
        DummyModel() : PrognosticsModel(0, {}, {}, {}, {}, 0) {}

        DummyModel(const ConfigMap&) : DummyModel() {}

        state_type stateEqn(const double,
                            const state_type&,
                            const input_type&,
                            const noise_type&,
                            const double) const override {
            return state_type();
        }

        output_type outputEqn(const double, const state_type&, const noise_type&) const override {
            return output_type();
        }

        state_type initialize(const input_type&, const output_type&) const override {
            return state_type();
        }

        bool thresholdEqn(const double, const state_type&) const override {
            return false;
        }

        event_state_type eventStateEqn(const state_type&) const override {
            return event_state_type();
        }

        predicted_output_type predictedOutputEqn(const double, const state_type&) const override {
            return predicted_output_type();
        }
    };

    class DummyObserver final : public Observer {
    public:
        DummyObserver(const SystemModel& model)
            : Observer(model), state(model.getInputSize() + model.getOutputSize()) {}

        DummyObserver(const SystemModel& model, const ConfigMap&) : DummyObserver(model) {}

        void initialize(double t0,
                        const SystemModel::state_type&,
                        const SystemModel::input_type&) override {}

        void step(double,
                  const SystemModel::input_type& inputs,
                  const SystemModel::output_type& outputs) override {
            Expect(inputs.size() + outputs.size() == state.size(),
                   "Unexpected size of inputs/outputs");
            for (std::size_t i = 0; i < inputs.size(); i++) {
                state[i] = inputs[i];
            }
            for (std::size_t i = 0; i < outputs.size(); i++) {
                state[i + inputs.size()] = outputs[i];
            }
        }

        std::vector<PCOE::UData> getStateEstimate() const override {
            return state;
        }

    private:
        std::vector<PCOE::UData> state;
    };


    void ModelBasedEventDrivenPrognoserBuilder::setModelName(const std::string& value, bool isPrognosticsModel) {
        lock_guard guard(m);
        Expect(value.length() > 0, "Model name length");
        modelName = value;
        modelIsPrognosticsModel = isPrognosticsModel;
    }

    void ModelBasedEventDrivenPrognoserBuilder::setObserverName(const std::string& value) {
        lock_guard guard(m);
        Expect(value.length() > 0, "Observer name length");
        observerName = value;
    }

    void ModelBasedEventDrivenPrognoserBuilder::setPredictorName(const std::string& value) {
        lock_guard guard(m);
        Expect(value.length() > 0, "Predictor name length");
        predictorName = value;
    }

    EventDrivenPrognoser ModelBasedEventDrivenPrognoserBuilder::build(PCOE::MessageBus& bus,
                                                 const std::string& sensorSource,
                                                 const std::string& trajectorySource) {
        lock_guard guard(m);
        EventDrivenPrognoser container;
        SystemModel* model = nullptr;
        PrognosticsModel* progModel = nullptr;
        std::unique_ptr<Observer> observer;
        std::unique_ptr<Predictor> predictor;
        LoadEstimator* loadEstimator = nullptr;
        EventDrivenTrajectoryService* ts = new EventDrivenTrajectoryService(bus, std::unique_ptr<TrajectoryService>(new TrajectoryService()), trajectorySource);
        container.addEventListener(ts);
        
        if (loadEstimatorName.empty() && config.hasKey("loadEstimator")) {
            loadEstimatorName = config.getString("loadEstimator");
        }
        if (!loadEstimatorName.empty()) {
            log.FormatLine(LOG_DEBUG, MODULE_NAME, "Building load estimator %s", loadEstimatorName.c_str());
            auto& factory = LoadEstimatorFactory::instance();
            loadEstimator = factory.Create(loadEstimatorName, config).release();
            container.setLoadEstimator(loadEstimator);
        }
        else {
            log.WriteLine(LOG_WARN, MODULE_NAME, "No load estimator name found");
        }
        

        if (modelName.empty() && config.hasKey("model")) {
            modelName = config.getString("model");
        }
        if (!modelName.empty() && !modelIsPrognosticsModel) {
            log.FormatLine(LOG_DEBUG, MODULE_NAME, "Building model %s", modelName.c_str());
            auto& factory = SystemModelFactory::instance();
            model = factory.Create(modelName).release();
            container.setModel(model);
        }
        else if (!modelName.empty()) {
            log.FormatLine(LOG_DEBUG, MODULE_NAME, "Building prognostics model %s", modelName.c_str());
            auto& factory = PrognosticsModelFactory::instance();
            progModel = factory.Create(modelName, config).release();
            container.setModel(progModel);
        }
        else {
            log.WriteLine(LOG_WARN, MODULE_NAME, "No model name found");
        }

        if (observerName.empty() && config.hasKey("observer")) {
            observerName = config.getString("observer");
        }
        if (!observerName.empty()) {
            log.FormatLine(LOG_DEBUG, MODULE_NAME, "Building observer %s", modelName.c_str());
            auto& factory = ObserverFactory::instance();
            const SystemModel* m = progModel ? progModel : model;
            Require(m, "Observer missing model");
            observer = factory.Create(observerName, *m, config);
        }
        else {
            log.WriteLine(LOG_WARN, MODULE_NAME, "No observer name found");
        }

        if (predictorName.empty() && config.hasKey("predictor")) {
            predictorName = config.getString("predictor");
        }
        if (!predictorName.empty()) {
            log.FormatLine(LOG_DEBUG, MODULE_NAME, "Building predictor %s", modelName.c_str());
            auto& factory = PredictorFactory::instance();
            Require(progModel, "Predictor missing model");
            Require(loadEstimator, "Predictor missing load estimator");
            predictor = factory.Create(predictorName, *progModel, *loadEstimator, ts->getTrajectoryService(), config);
        }
        else {
            log.WriteLine(LOG_WARN, MODULE_NAME, "No predictor name found");
        }

        if (observer) {
            container.addEventListener(new EventDrivenObserver(bus, std::move(observer), sensorSource));
        }

        if (predictor) {
            container.addEventListener(
                new EventDrivenPredictor(bus, std::move(predictor), sensorSource));
        }

        Ensure(!(model && progModel), "SystemModel and PrognosticsModel both created");
        log.WriteLine(LOG_WARN, MODULE_NAME, "Build complete");
        return std::move(container);
    }

    void ModelBasedEventDrivenPrognoserBuilder::reset() {
        lock_guard guard(m);
        config = ConfigMap();
        loadEstimatorName.clear();
        modelName.clear();
        observerName.clear();
        predictorName.clear();
    }
}
