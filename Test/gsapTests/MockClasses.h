// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Loading/LoadEstimator.h"
#include "Messages/IMessageProcessor.h"
#include "Messages/MessageBus.h"
#include "Model.h"
#include "Observers/Observer.h"
#include "Predictors/Predictor.h"
#include "PrognosticsModel.h"

class MessageCounter final : public IMessageProcessor {
public:
    MessageCounter(MessageBus& bus, std::string src, MessageId msgId = MessageId::All)
        : bus(bus), source(std::move(src)) {
        bus.subscribe(this, source, msgId);
    }

    ~MessageCounter() {
        bus.unsubscribe(this);
    }

    void processMessage(const std::shared_ptr<Message>& message) override {
        lastMessage = message;
        ++count;
    }

    inline std::size_t getCount() const {
        return count;
    }

    inline const std::shared_ptr<Message>& getLastMessage() {
        return lastMessage;
    }

private:
    MessageBus& bus;
    std::string source;
    std::size_t count = 0;
    std::shared_ptr<Message> lastMessage;
};

class TestModel final : public Model {
public:
    TestModel()
        : Model(2, {MessageId::TestInput0, MessageId::TestInput1}, {MessageId::TestOutput0}) {}

    state_type stateEqn(const double,
                        const state_type& x,
                        const input_type&,
                        const noise_type&,
                        const double) const override {
        return x;
    }

    output_type outputEqn(const double,
                          const state_type&,
                          const input_type& u,
                          const noise_type&) const override {
        return output_type({u[0] * u[1]});
    }

    state_type initialize(const input_type& u, const output_type&) const override {
        return state_type(u.vec());
    }
};

class TestPrognosticsModel final : public PrognosticsModel {
public:
    TestPrognosticsModel()
        : PrognosticsModel(2,
                           {MessageId::TestInput0, MessageId::TestInput1},
                           {MessageId::TestOutput0},
                           {},
                           {MessageId::TestEvent0}) {}

    state_type stateEqn(const double,
                        const state_type& x,
                        const input_type&,
                        const noise_type&,
                        const double) const override {
        return x;
    }

    output_type outputEqn(const double,
                          const state_type&,
                          const input_type& u,
                          const noise_type&) const override {
        return output_type({u[0] * u[1]});
    }

    state_type initialize(const input_type& u, const output_type&) const override {
        return state_type(u.vec());
    }

    bool thresholdEqn(const double, const state_type& x, const input_type&) const override {
        return x[0] != 0.0;
    }

    event_state_type eventStateEqn(const state_type&) const override {
        return 0;
    }

    input_type inputEqn(const double,
                        const std::vector<double>&,
                        const std::vector<double>&) const override {
        return getInputVector();
    }

    predicted_output_type predictedOutputEqn(const double,
                                             const state_type&,
                                             const input_type&,
                                             const output_type&) const override {
        return getPredictedOutputVector();
    }
};

class TestLoadEstimator final : public LoadEstimator {
public:
    TestLoadEstimator() : loading(8) {}

    TestLoadEstimator(const ConfigMap& config) {
        const std::string LOADING_KEY = "LoadEstimator.Loading";
        requireKeys(config, {LOADING_KEY});

        loading = config.getDoubleVector(LOADING_KEY);
    }

    LoadEstimate estimateLoad(const double) override {
        return loading;
    }

    std::vector<double> getSavePts() override {
        return std::vector<double>({0, 10, 50, 100});
    }

private:
    std::vector<double> loading;
};

class TestObserver final : public Observer {
public:
    TestObserver(const Model* model) : Observer(model) {}

    void initialize(double t0, const Model::state_type& x0, const Model::input_type& u0) override {
        xPrev = x0;
        uPrev = u0;
        std::vector<double> zeroNoiseZ(model->getOutputSize());
        zPrev = model->outputEqn(t0, x0, u0, zeroNoiseZ);
        initialized = true;
    }

    void step(double t, const Model::input_type& u, const Model::output_type&) override {
        std::vector<double> zeroNoiseX(model->getStateSize());
        xPrev = model->stateEqn(t, xPrev, u, zeroNoiseX);
    }

    const Model::state_type& getStateMean() const override {
        return xPrev;
    }

    std::vector<UData> getStateEstimate() const override {
        std::vector<UData> result;
        for (std::size_t i = 0; i < xPrev.size(); ++i) {
            UData ud;
            ud.set(xPrev[i]);
            result.push_back(ud);
        }
        return result;
    }

    const Model::output_type& getOutputMean() const override {
        return zPrev;
    }

private:
    Model::state_type xPrev;
    Model::output_type zPrev;
};

class TestPredictor final : public Predictor {
public:
    TestPredictor(const PrognosticsModel* m, LoadEstimator* le, const ConfigMap& config)
        : Predictor(m, le, config) {}

    Prediction predict(double, const std::vector<UData>& state) override {
        ProgEvent event(MessageId::TestEvent0, {UData()}, {UData()});

        return Prediction({event}, std::vector<DataPoint>());
    }
};
