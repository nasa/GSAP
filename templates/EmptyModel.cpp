/**  EmptyModel - Body
 *   @file       EmptyModel.cpp
 *   @ingroup    GSAP-Support
 *
 *   @copyright Copyright (c) 2017-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <vector>
#include <string>

#include "EmptyModel.h"

using namespace PCOE;

const Model::state_type::size_type STATE_SIZE = 2; // Number of state variables
const std::vector<std::string> INPUTS = {"input1", "input2"}; // Inputs to model (e.g., current)
const std::vector<std::string> OUTPUTS = {"output1", "output2"}; // Outputs of model (e.g., voltage)

EmptyModel::EmptyModel(): Model(STATE_SIZE, INPUTS, OUTPUTS) {

}

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
Model::state_type EmptyModel::initialize(const Model::input_type& u, const Model::output_type& z) const {
    Model::state_type initialized_state = getStateVector();
    
    // Fill initialized_state
    // e.g., initialized_state[0] = SOMEVALUE;
    
    return initialized_state;
}
