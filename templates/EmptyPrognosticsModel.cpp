// Copyright (c) 2017-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

// Supress warnings for unused parameters. Remove this when copying the template
// to create a new instance of the templated class.
#ifdef _MSC_VER
#pragma warning(disable : 4100)
#elif defined __clang__
#pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined __GNUC__ && __GNUC__ >= 6
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include "EmptyPrognosticsModel.h"

#include <vector>

#include "ConfigMap.h"

using namespace PCOE;

// Number of state variables
const BatteryModel::state_type::size_type STATE_SIZE = 2;

// Inputs to model (e.g., current)
static auto INPUTS = {MessageId::TestInput0, MessageId::TestInput1};

// Outputs of model (e.g., voltage)
static auto OUTPUTS = {MessageId::TestOutput0, MessageId::TestOutput1};

const std::vector<std::string> PREDICTED_OUTPUTS = {"pOutput1"};
const std::vector<MessageId> EVENTS = {MessageId::TestEvent0};

EmptyPrognosticsModel::EmptyPrognosticsModel()
    : PrognosticsModel(STATE_SIZE, INPUTS, OUTPUTS, PREDICTED_OUTPUTS, EVENTS) {
    // Default constructor
}

// Constructor based on configMap
EmptyPrognosticsModel::EmptyPrognosticsModel(const ConfigMap& configMap)
    : EmptyPrognosticsModel::EmptyPrognosticsModel() {
    // Setup model based on configuration parameters
}

SystemModel::state_type EmptyPrognosticsModel::stateEqn(double t,
                                                        const state_type& x,
                                                        const input_type& u,
                                                        double dt) const {
    auto new_state = getStateVector();

    // Fill new_state

    return new_state;
}

SystemModel::output_type EmptyPrognosticsModel::outputEqn(double t,
                                                          const state_type& x) const {
    auto output = getOutputVector();

    // Fill output

    return output;
}

// Initialize state, given initial inputs and outputs
SystemModel::state_type EmptyPrognosticsModel::initialize(const input_type& u,
                                                          const output_type& z) const {
    auto initialized_state = getStateVector();

    // Fill initialized_state

    return initialized_state;
}

std::vector<bool> EmptyPrognosticsModel::thresholdEqn(double t, const state_type& x) const {
    // Often uses outputs to calculate threshold, remove line if not relevant
    auto z = outputEqn(t, x);

    bool hasReachedThreshold = false;
    // SET hasReachedThreshold

    return {hasReachedThreshold};
}

PrognosticsModel::observables_type
EmptyPrognosticsModel::observablesEqn(double t, const state_type& x) const {
    auto observables = getObservablesVector();
    // Fill observables

    return observables;
}
