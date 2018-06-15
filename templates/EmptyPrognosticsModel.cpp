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

EmptyPrognosticsModel::EmptyPrognosticsModel() {
    // Default constructor
}

// Constructor based on configMap
EmptyPrognosticsModel::EmptyPrognosticsModel(const ConfigMap & configMap) : EmptyPrognosticsModel::EmptyPrognosticsModel() {
    // Setup model based on configuration parameters
}

// EmptyPrognosticsModel State Equation
void EmptyPrognosticsModel::stateEqn(const double, std::vector<double> & x, 
                       const std::vector<double> & u, const std::vector<double> & n, 
                       const double dt) {

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

// EmptyPrognosticsModel Output Equation
void EmptyPrognosticsModel::outputEqn(const double, const std::vector<double> & x,
                        const std::vector<double> &, const std::vector<double> & n,
                        std::vector<double> & z) {

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

// EmptyPrognosticsModel Threshold Equation
bool EmptyPrognosticsModel::thresholdEqn(const double t, const std::vector<double> & x, const std::vector<double> & u) {
    // Compute based on voltage, so use output equation to get voltage
    std::vector<double> z(2);
    std::vector<double> zeroNoise(8);
    outputEqn(t, x, u, zeroNoise, z);

    // Determine if voltage (second element in z) is below VEOD threshold
    return z[1] <= parameters.VEOD;
}

// EmptyPrognosticsModel Input Equation
void EmptyPrognosticsModel::inputEqn(const double t, const std::vector<double> & inputParameters, std::vector<double> & u) {
    // Determine what the system input should be given the current time and a set of "input parameters"
    // Extract input parameters
    // f = inputParameters[0]
    // ...
    
    // Set inputs
    // u[0] = f;
    // ...
}

// EmptyPrognosticsModel Predicted Outputs Equation
void EmptyPrognosticsModel::predictedOutputEqn(const double, const std::vector<double> & x, const std::vector<double> &, std::vector<double> & z) {

    // Extract states
    // double a = x[0];
    // double b = x[1];
    // ...
    
    // Extract inputs
    // double c = u[0];
    // ...
    
    // Output equations
    // double e = c*b;
    // ...
    
    // Set outputs
    // z[0] = e;
    // ...
}

// Initialize state, given an initial voltage, current, and temperature
void EmptyPrognosticsModel::initialize(std::vector<double> & x, const std::vector<double> & u, const std::vector<double> & z) {
    // Determine x from u and z (model-dependent) or as fixed values
    // double a0 = 1;
    // double b0 = 2;
    // ...
    
    // Set x
    // x[0] = a0;
    // x[1] = b0;
    // ...
}
