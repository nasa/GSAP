/**  EmptyModel - Body
 *   @file       EmptyModel.cpp
 *   @ingroup    GSAP-Support
 *
 *   @copyright Copyright (c) 2017-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <string>
#include <vector>

#include "EmptyModel.h"

using namespace PCOE;

// Number of state variables
const Model::state_type::size_type STATE_SIZE = 2;

// Inputs to model (e.g., current)
auto INPUTS = {MessageId::TestInput0, MessageId::TestInput1};

// Outputs of model (e.g., voltage)
auto OUTPUTS = {MessageId::TestOutput0, MessageId::TestOutput1};

EmptyModel::EmptyModel() : Model(STATE_SIZE, INPUTS, OUTPUTS) {}

// Constructor based on configMap
EmptyModel::EmptyModel(const ConfigMap& configMap) : EmptyModel::EmptyModel() {
    // Setup model based on configuration parameters
}

// EmptyModel State Equation
Model::state_type EmptyModel::stateEqn(double t,
                                       const Model::state_type& x,
                                       const Model::input_type& u,
                                       const Model::noise_type& n,
                                       double dt) const {
    auto new_state = getStateVector();

    // Fill new_state
    // e.g., new_state[0] = SOMEVALUE;

    return new_state;
}

// EmptyModel Output Equation
Model::output_type EmptyModel::outputEqn(double t,
                                         const Model::state_type& x,
                                         const Model::input_type& u,
                                         const Model::noise_type& n) const {
    auto output = getOutputVector();

    // Fill output
    // e.g., output[0] = SOMEVALUE;

    return output;
}

// Initialize state, given initial inputs and outputs
Model::state_type EmptyModel::initialize(const Model::input_type& u,
                                         const Model::output_type& z) const {
    Model::state_type initialized_state = getStateVector();

    // Fill initialized_state
    // e.g., initialized_state[0] = SOMEVALUE;

    return initialized_state;
}
