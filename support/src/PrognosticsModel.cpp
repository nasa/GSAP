/**  PrognosticsModel - Body
*   @file       PrognosticsModel.cpp
*   @ingroup    GSAP-Support
*
*   @brief      PrognosticsModel class
*
*   @author     Matthew Daigle
*   @version    0.1.0
*
*   @pre        N/A
*
*      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
*      Created: September 1, 2016
*
*   @copyright Copyright (c) 2016 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#include "PrognosticsModel.h"

#include <vector>

namespace PCOE {
    
    double PrognosticsModel::simulateToThreshold(double t0, const std::vector<double> & x0, const std::vector<double> & inputParameters, Trajectory & X, Trajectory & Z) {
        // Ensure X and Z are cleared
        X.clear();
        Z.clear();
        // Initialize noise vectors
        std::vector<double> processNoise(getNumStates());
        std::vector<double> sensorNoise(getNumOutputs());
        // Initialize t, x, u, z
        double t = t0;
        std::vector<double> x = x0;
        std::vector<double> u(getNumInputs());
        inputEqn(t,inputParameters,u);
        std::vector<double> z(getNumOutputs());
        outputEqn(t, x, u, sensorNoise, z);
        
        // Update X, Z trajectories
        X.push_back(x);
        Z.push_back(z);
        
        // Simulate until last time unit
        while (!thresholdEqn(t,x,u)) {
            // Update state to next time step
            stateEqn(t, x, u, processNoise, getDt());
            // Update time
            t += getDt();
            // Update input
            inputEqn(t,inputParameters,u);
            // Update output
            outputEqn(t, x, u, sensorNoise, z);
            
            // Update X, Z trajectories
            X.push_back(x);
            Z.push_back(z);
        }
        
        // Return time at which hit threshold
        return t;
    }
}
