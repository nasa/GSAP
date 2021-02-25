// Copyright (c) 2018-2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Loading/LoadEstimator.h"
#include "Messages/IMessageProcessor.h"
#include "Messages/MessageBus.h"
#include "Models/PrognosticsModel.h"
#include "Observers/Observer.h"
#include "Predictors/Predictor.h"

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

class TestPrognosticsModel final : public PrognosticsModel {
public:
    TestPrognosticsModel(const ConfigMap& = ConfigMap())
        : PrognosticsModel(2,
                           {MessageId::TestInput0, MessageId::TestInput1},
                           {MessageId::TestOutput0},
                           {},
                           {MessageId::TestEvent0}) {}

    state_type stateEqn(const double,
                        const state_type& x,
                        const input_type&,
                        const double) const override {
        return x;
    }

    output_type outputEqn(const double, const state_type&) const override {
        return output_type({3});
    }

    state_type initialize(const input_type& u, const output_type&) const override {
        return state_type(u.vec());
    }

    std::vector<bool> thresholdEqn(const double, const state_type& x) const override {
        return {std::abs(x[0]) > 1e-12};
    }

    event_state_type eventStateEqn(const state_type&) const override {
        return event_state_type({0.0});
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

private:
    std::vector<double> loading;
};

class TestObserver final : public Observer {
public:
    TestObserver(const PrognosticsModel& model, const ConfigMap& = ConfigMap()) : Observer(model) {}

    void initialize(double t0,
                    const PrognosticsModel::state_type& x0,
                    const PrognosticsModel::input_type& u0) override {
        xPrev = x0;
        uPrev = u0;
        std::vector<double> zeroNoiseZ(model.getOutputSize());
        zPrev = model.outputEqn(t0, x0, zeroNoiseZ);
        initialized = true;
    }

    void step(double t,
              const PrognosticsModel::input_type& u,
              const PrognosticsModel::output_type&) override {
        std::vector<double> zeroNoiseX(model.getStateSize());
        xPrev = model.stateEqn(t, xPrev, u, zeroNoiseX);
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

private:
    PrognosticsModel::state_type xPrev;
    PrognosticsModel::output_type zPrev;
};

class TestPredictor final : public Predictor {
public:
    TestPredictor(const PrognosticsModel& m,
                  LoadEstimator& le,
                  const TrajectoryService& trajService,
                  const ConfigMap& config)
        : Predictor(m, le, trajService, config) {}

    Prediction predict(double, const std::vector<UData>& state) override {
        auto prediction = UData((state[1].get() + state[0].get()) / 2);
        ProgEvent event(MessageId::TestEvent0, state, prediction);

        return Prediction({event}, std::vector<DataPoint>());
    }
};

template <class T>
class TestAllocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using void_pointer = void*;
    using const_void_pointer = const void*;

    TestAllocator() : totalAllocated(std::shared_ptr<size_type>(new std::size_t(0))) {}

    TestAllocator(const TestAllocator& other) : totalAllocated(other.totalAllocated) {}

    TestAllocator(TestAllocator&& other) : totalAllocated(std::move(other.totalAllocated)) {}

    template <class U>
    TestAllocator(const TestAllocator<U>& other) : totalAllocated(other.totalAllocated) {}

    pointer allocate(size_type n) {
        if (n > std::numeric_limits<std::size_t>::max() / sizeof(value_type)) {
            throw std::bad_alloc();
        }

        size_type bytes = sizeof(value_type) * n;
        auto p = static_cast<pointer>(std::malloc(bytes));
        if (!p) {
            throw std::bad_alloc();
        }
        *totalAllocated += bytes;
        return p;
    }

    void deallocate(pointer ptr, size_type n) {
        static_cast<void>(n);
        std::free(static_cast<void_pointer>(ptr));
    }

    friend bool operator==(const TestAllocator&, const TestAllocator&) {
        return true;
    }

    friend bool operator!=(const TestAllocator& lhs, const TestAllocator& rhs) {
        return !(lhs == rhs);
    }

    std::shared_ptr<size_type> totalAllocated;
};
