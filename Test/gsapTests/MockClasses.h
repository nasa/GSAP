// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "LoadEstimator.h"
#include "Model.h"
#include "Observers/Observer.h"
#include "Predictors/Predictor.h"
#include "PrognosticsModel.h"

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
                           {"TestPrediction0"}) {}

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

    bool thresholdEqn(const double t, const state_type& x, const input_type& u) const override {
        return x[0] != 0.0;
    }

    event_state_type eventStateEqn(const state_type& x) const override {
        return 0;
    }

    input_type inputEqn(const double t,
                        const std::vector<double>& params,
                        const std::vector<double>& loadEstimate) const override {
        return getInputVector();
    }

    predicted_output_type predictedOutputEqn(const double t,
                                             const state_type& x,
                                             const input_type& u,
                                             const output_type& z) const override {
        return getPredictedOutputVector();
    }
};

class TestLoadEstimator final : public LoadEstimator {
    LoadEstimate estimateLoad(const double t, const unsigned int sample) override {
        return LoadEstimate(0);
    }
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

    void step(double t, const Model::input_type& u, const Model::output_type& z) override {
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
    TestPredictor(const PrognosticsModel* m, LoadEstimator* le, const GSAPConfigMap& config)
        : Predictor(m, le, config) {}

    Prediction predict(double t, const std::vector<UData>& state) override {
        Prediction result;

        ProgEvent event;
        event.setNPoints(1);
        event.setEventState(state[0]);
        result.events.push_back(event);

        return result;
    }
};
