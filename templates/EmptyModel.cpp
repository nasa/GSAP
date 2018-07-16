/**  EmptyModel - Body
 *   @file       EmptyModel.cpp
 *   @ingroup    GSAP-Support
 *
 *   @copyright Copyright (c) 2017-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include "EmptyModel.h"
#include "ConfigMap.h"

using namespace PCOE;

EmptyModel::EmptyModel() {
    // Default constructor
    inputs = {} // SET MODEL INPUT TAG NAMES
    // e.g., inputs = {"power"}
    outputs = {} // SET MODEL OUTPUT TAG NAMES
    // e.g., outputs = {"voltage", "temperature"}
    events = {} // SET EVENT NAMES
    // e.g., events = {"EOD"}
    predictedOutputs = {} // SET PREDICTED OUTPUT NAMES
    // e.g., predictedOutputs = {"SOC"}
}

// Constructor based on configMap
EmptyModel::EmptyModel(const ConfigMap& configMap) : EmptyModel::EmptyModel() {
    // Setup model based on configuration parameters
}

// EmptyModel State Equation
std::vector<double> EmptyModel::stateEqn(const double,
                                         std::vector<double>& x,
                                         const std::vector<double>& u,
                                         const std::vector<double>& n,
                                         const double dt) const {
    // Extract states
    // double a = x[0];
    // double b = x[1];
    // ...

    // Extract inputs
    // double c = u[0];
    // ...

    // State equations
    // double adot = a + b;
    // double bdot = c;
    // ...

    // Update state
    // x[0] = a + adot*dt;
    // x[1] = b + bdot*dt;
    // ...

    // Add process noise
    // x[0] += dt*n[0];
    // x[1] += dt*n[1];
    // ...
}

// EmptyModel Output Equation
std::vector<double> EmptyModel::outputEqn(const double,
                                          const std::vector<double>& x,
                                          const std::vector<double>&,
                                          const std::vector<double>& n,
                                          const std::vector<double>& z) const {
    // Extract states
    // double a = x[0];
    // double b = x[1];
    // ...

    // Extract inputs
    // double c = u[0];
    // ...

    // Output equations
    // double d = a + b + c;
    // ...

    // Set outputs
    // z[0] = d;
    // ...

    // Add noise
    // z[0] += n[0];
    // ...
}

// Initialize state, given initial inputs and outputs
std::vector<double> EmptyModel::initialize(const std::vector<double>& u,
                                           const std::vector<double>& z) const {
    // Determine x from u and z (model-dependent) or as fixed values
    // double a0 = 1;
    // double b0 = 2;
    // ...

    // Set x
    // x[0] = a0;
    // x[1] = b0;
    // ...
}
