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

#include <string>
#include <vector>

#include "EmptyModel.h"

using namespace PCOE;

// Number of state variables
const SystemModel::state_type::size_type STATE_SIZE = 2;

// Inputs to model (e.g., current)
static auto INPUTS = {MessageId::TestInput0, MessageId::TestInput1};

// Outputs of model (e.g., voltage)
static auto OUTPUTS = {MessageId::TestOutput0, MessageId::TestOutput1};

EmptyModel::EmptyModel() : SystemModel(STATE_SIZE, INPUTS, OUTPUTS, {}, {}) {}

// Constructor based on configMap
EmptyModel::EmptyModel(const ConfigMap& configMap) : EmptyModel::EmptyModel() {
    // Setup model based on configuration parameters
}

// EmptyModel State Equation
SystemModel::state_type EmptyModel::stateEqn(double t,
                                             const SystemModel::state_type& x,
                                             const SystemModel::input_type& u,
                                             const SystemModel::noise_type& n,
                                             double dt) const {
    auto new_state = getStateVector();

    // Fill new_state
    // e.g., new_state[0] = SOMEVALUE;

    return new_state;
}

// EmptyModel Output Equation
SystemModel::output_type EmptyModel::outputEqn(double t,
                                               const SystemModel::state_type& x,
                                               const SystemModel::noise_type& n) const {
    auto output = getOutputVector();

    // Fill output
    // e.g., output[0] = SOMEVALUE;

    return output;
}

// Initialize state, given initial inputs and outputs
SystemModel::state_type EmptyModel::initialize(const SystemModel::input_type& u,
                                               const SystemModel::output_type& z) const {
    SystemModel::state_type initialized_state = getStateVector();

    // Fill initialized_state
    // e.g., initialized_state[0] = SOMEVALUE;

    return initialized_state;
}
