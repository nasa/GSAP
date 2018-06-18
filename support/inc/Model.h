/**  Model - Header
*   @file       Model.h
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
*   @copyright Copyright (c) 2018 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#ifndef PCOE_MODEL_H
#define PCOE_MODEL_H

#include <vector>
#include <string>

namespace PCOE {
    class Model {
     protected:
        unsigned int numStates;
        double m_dt;  // Sampling time

     public:
        virtual ~Model() = default;

        /** @brief      Execute state equation. This version of the function uses the default sampling time.
        *   @param      t Time
        *   @param      x Current state vector. This gets updated to the state at the new time.
        *   @param      u Input vector
        *   @param      n Process noise vector
        **/
        void stateEqn(const double t, std::vector<double> & x, const std::vector<double> & u,
            const std::vector<double> & n);
        /** @brief      Execute state equation. This version of the function uses a given sampling time.
        *   @param      t Time
        *   @param      x Current state vector. This gets updated to the state at the new time.
        *   @param      u Input vector
        *   @param      n Process noise vector
        *   @param      dt Sampling time
        **/
        virtual void stateEqn(const double t, std::vector<double> & x,
            const std::vector<double> & u, const std::vector<double> & n,
            const double dt) = 0;
        /** @brief      Execute output equation
        *   @param      t Time
        *   @param      x State vector
        *   @param      u Input vector
        *   @param      n Sensor noise vector
        *   @param      z Output vector. This gets updated to the new output at the given time.
        **/
        virtual void outputEqn(const double t, const std::vector<double> & x,
            const std::vector<double> & u, const std::vector<double> & n,
            std::vector<double> & z) = 0;
        /** @brief      Initialize state vector given initial inputs and outputs.
        *   @param      x Current state vector. This gets updated.
        *   @param      u Input vector
        *   @param      z Output vector
        **/
        virtual void initialize(std::vector<double> & x, const std::vector<double> & u,
            const std::vector<double> & z) = 0;

        // Get size of vectors
        unsigned int getNumStates() const;
        unsigned int getNumInputs() const;
        unsigned int getNumOutputs() const;
        std::vector<std::string> inputs;
        std::vector<std::string> outputs;
        std::vector<std::string> events;

        // Get/set dt
        double getDt() const;
        void setDt(const double newDt);
    };
}

#endif  // PCOE_MODEL_H
