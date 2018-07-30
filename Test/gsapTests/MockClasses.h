// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Model.h"
#include "Observers/Observer.h"

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
