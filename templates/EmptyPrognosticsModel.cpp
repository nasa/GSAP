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
const Model::state_type::size_type STATE_SIZE = 2;

// Inputs to model (e.g., current)
auto INPUTS = {MessageId::TestInput0, MessageId::TestInput1};

// Outputs of model (e.g., voltage)
auto OUTPUTS = {MessageId::TestOutput0, MessageId::TestOutput1};

const std::vector<std::string> PREDICTED_OUTPUTS = {"pOutput1"};
const std::vector<MessageId> EVENTS = {MessageId::TestEvent0};
const Model::size_type INPUT_PARAM_COUNT = 1;

EmptyPrognosticsModel::EmptyPrognosticsModel()
    : PrognosticsModel(STATE_SIZE, INPUTS, OUTPUTS, PREDICTED_OUTPUTS, EVENTS, INPUT_PARAM_COUNT) {
    // Default constructor
}

// Constructor based on configMap
EmptyPrognosticsModel::EmptyPrognosticsModel(const ConfigMap& configMap)
    : EmptyPrognosticsModel::EmptyPrognosticsModel() {
    // Setup model based on configuration parameters
}

Model::state_type EmptyPrognosticsModel::stateEqn(double t,
                                                  const Model::state_type& x,
                                                  const Model::input_type& u,
                                                  const Model::noise_type& n,
                                                  double dt) const {
    auto new_state = getStateVector();

    // Fill new_state

    return new_state;
}

Model::output_type EmptyPrognosticsModel::outputEqn(double t,
                                                    const Model::state_type& x,
                                                    const Model::input_type& u,
                                                    const Model::noise_type& n) const {
    auto output = getOutputVector();

    // Fill output

    return output;
}

// Initialize state, given initial inputs and outputs
Model::state_type EmptyPrognosticsModel::initialize(const Model::input_type& u,
                                                    const Model::output_type& z) const {
    auto initialized_state = getStateVector();

    // Fill initialized_state

    return initialized_state;
}

bool EmptyPrognosticsModel::thresholdEqn(const double t,
                                         const Model::state_type& x,
                                         const Model::input_type& u) {
    // Often uses outputs to calculate threshold, remove line if not relevant
    auto z = outputEqn(t, x, u, std::vector<double>(2));

    bool hasReachedThreshold;
    // SET hasReachedThreshold

    return hasReachedThreshold;
}

Model::input_type EmptyPrognosticsModel::inputEqn(double t,
                                                  const std::vector<double>& params,
                                                  const std::vector<double>& loadEstimate) const {
    auto input = getInputVector();

    // Determine what the system input should be given the current time and a set of "input
    // parameters" Fill input

    return input;
}

PrognosticsModel::predicted_output_type
EmptyPrognosticsModel::predictedOutputEqn(double t,
                                          const state_type& x,
                                          const input_type& u,
                                          const output_type& z) const {
    auto predictedOutputs = getPredictedOutputVector();
    // Fill predictedOutputs

    return predictedOutputs;
}
