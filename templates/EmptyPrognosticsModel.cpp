/**  EmptyPrognosticsModel - Body
 *   @file       EmptyPrognosticsModel.cpp
 *   @ingroup    GSAP-Support
 *
 *   @copyright Copyright (c) 2016-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include "EmptyPrognosticsModel.h"

#include <vector>

#include "ConfigMap.h"

using namespace PCOE;

// Number of state variables
const BatteryModel::state_type::size_type STATE_SIZE = 2;

// Inputs to model (e.g., current)
auto INPUTS = {MessageId::TestInput0, MessageId::TestInput1};

// Outputs of model (e.g., voltage)
auto OUTPUTS = {MessageId::TestOutput0, MessageId::TestOutput1};

const std::vector<std::string> PREDICTED_OUTPUTS = {"pOutput1"};
const std::vector<MessageId> EVENTS = {MessageId::TestEvent0};
const BatteryModel::input_type::size_type INPUT_PARAM_COUNT = 1;

EmptyPrognosticsModel::EmptyPrognosticsModel()
    : PrognosticsModel(STATE_SIZE, INPUTS, OUTPUTS, PREDICTED_OUTPUTS, EVENTS, INPUT_PARAM_COUNT) {
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
                                                        const noise_type& n,
                                                        double dt) const {
    auto new_state = getStateVector();

    // Fill new_state

    return new_state;
}

SystemModel::output_type EmptyPrognosticsModel::outputEqn(double t,
                                                          const state_type& x,
                                                          const noise_type& n) const {
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

bool EmptyPrognosticsModel::thresholdEqn(const double t,
                                         const state_type& x) {
    // Often uses outputs to calculate threshold, remove line if not relevant
    auto z = outputEqn(t, x, std::vector<double>(2));

    bool hasReachedThreshold = false;
    // SET hasReachedThreshold

    return hasReachedThreshold;
}

PrognosticsModel::predicted_output_type
EmptyPrognosticsModel::predictedOutputEqn(double t,
                                          const state_type& x) const {
    auto predictedOutputs = getPredictedOutputVector();
    // Fill predictedOutputs

    return predictedOutputs;
}
