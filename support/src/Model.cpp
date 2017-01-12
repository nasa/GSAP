/**  Model - Body
*   @file       Model.cpp
*   @ingroup    GSAP-Support
*
*   @brief      Model class
*
*   @author     Matthew Daigle
*   @version    0.1.0
*
*   @pre        N/A
*
*      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
*      Created: March 5, 2016
*
*   @copyright Copyright (c) 2016 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#include "Model.h"

#include <vector>

namespace PCOE {
    void Model::stateEqn(const double t, std::vector<double> & x,
        const std::vector<double> & u,
        const std::vector<double> & n) {
        stateEqn(t, x, u, n, m_dt);
    }

    unsigned int Model::getNumStates() const {
        return numStates;
    }

    unsigned int Model::getNumInputs() const {
        return numInputs;
    }

    unsigned int Model::getNumOutputs() const {
        return numOutputs;
    }

    double Model::getDt() const {
        return m_dt;
    }

    void Model::setDt(const double newDt) {
        m_dt = newDt;
    }
    
    void Model::simulate(const double t0, const double tFinal, const std::vector<double> & x0, const std::vector<double> & u, Trajectory & X, Trajectory & Z) {
        // Construct time vector based on t0 and tFinal and default dt
        std::vector<double> T;
        double t = t0;
        while (t<=tFinal) {
            T.push_back(t);
            t += m_dt;
        }
        // Construct U trajectory assuming constant inputs
        Trajectory U(T.size(),u);
        // Call general simulate function
        simulate(T,x0,U,X,Z);
    }
    
    void Model::simulate(const double t0, const double tFinal, const std::vector<double> & x0, const Trajectory & U, Trajectory & X, Trajectory & Z) {
        // Construct time vector based on t0 and tFinal and default dt
        std::vector<double> T;
        double t = t0;
        while (t<=tFinal) {
            T.push_back(t);
            t += m_dt;
        }
        // Call general simulate function
        simulate(T,x0,U,X,Z);
    }
    
    void Model::simulate(const std::vector<double> & T, const std::vector<double> & x0, const std::vector<double> & u, Trajectory & X, Trajectory & Z) {
        // Construct U trajectory assuming constant inputs
        Trajectory U(T.size(),u);
        // Call general simulate function
        simulate(T,x0,U,X,Z);
    }
    
    // 4. Given time vector and input trajectory
    void Model::simulate(const std::vector<double> & T, const std::vector<double> & x0, const Trajectory & U, Trajectory & X, Trajectory & Z) {
        // Ensure X and Z are cleared
        X.clear();
        Z.clear();
        // Initialize noise vectors
        std::vector<double> processNoise(getNumStates());
        std::vector<double> sensorNoise(getNumOutputs());
        // Initialize t, x, u, z
        double t = T[0];
        std::vector<double> x = x0;
        std::vector<double> u = U[0];
        std::vector<double> z(getNumOutputs());
        outputEqn(t, x, u, sensorNoise, z);
        
        // Update X, Z trajectories
        X.push_back(x);
        Z.push_back(z);
        
        // Simulate until last time unit
        for (size_t i=1; i<T.size(); i++) {
            // Update state to next time step
            stateEqn(t, x, u, processNoise, T[i]-T[i-1]);
            // Update time
            t = T[i];
            // Update input
            u = U[i];
            // Update output
            outputEqn(t, x, u, sensorNoise, z);
            
            // Update X, Z trajectories
            X.push_back(x);
            Z.push_back(z);
        }
    }
}
