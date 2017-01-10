/**  EmptyModel - Body
*   @file       EmptyModel.cpp
*   @ingroup    GSAP-Support
*
*   @brief      EmptyModel model class for prognostics
*
*   @author     Matthew Daigle
*   @version    0.1.0
*
*   @pre        N/A
*
*      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
*      Created: January 10, 2017
*
*   @copyright Copyright (c) 2017 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#include "EmptyModel.h"
#include "ConfigMap.h"

using namespace PCOE;

EmptyModel::EmptyModel() {
    // Default constructor
}

// Constructor based on configMap
EmptyModel::EmptyModel(const ConfigMap & configMap) : EmptyModel::EmptyModel() {
    // Setup model based on configuration parameters
}

// EmptyModel State Equation
void EmptyModel::stateEqn(const double, std::vector<double> & x, 
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

// EmptyModel Output Equation
void EmptyModel::outputEqn(const double, const std::vector<double> & x,
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

// Initialize state, given initial inputs and outputs
void EmptyModel::initialize(std::vector<double> & x, const std::vector<double> & u, const std::vector<double> & z) {
    // Determine x from u and z (model-dependent) or as fixed values
    // double a0 = 1;
    // double b0 = 2;
    // ...
    
    // Set x
    // x[0] = a0;
    // x[1] = b0;
    // ...
}
