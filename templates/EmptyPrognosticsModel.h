/**  EmptyPrognosticsModel - Header
*   @file       EmptyPrognosticsModel.h
*   @ingroup    GSAP-Support
*
*   @brief      EmptyPrognosticsModel model class for prognostics
*
*   @author     Matthew Daigle
*   @version    1.1.0
*
*   @pre        N/A
*
*      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
*      Created: January 10, 2017
*
*   @copyright Copyright (c) 2016-2018 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#ifndef EmptyPrognosticsModel_H
#define EmptyPrognosticsModel_H

#include <vector>

#include "PrognosticsModel.h"
#include "ConfigMap.h"
#include "ModelFactory.h"
#include "PrognosticsModelFactory.h"

class EmptyPrognosticsModel final : public PCOE::PrognosticsModel {
 public:
    // Constructor
    EmptyPrognosticsModel();

    // Constructor based on configMap
    EmptyPrognosticsModel(const PCOE::ConfigMap & paramMap);

    /** @brief      Execute state equation. This version of the function uses a given sampling time.
    *   @param      t Time
    *   @param      x Current state vector. This gets updated to the state at the new time.
    *   @param      u Input vector
    *   @param      n Process noise vector
    *   @param      dt Sampling time
    **/
    void stateEqn(const double t, std::vector<double> & x, const std::vector<double> & u,
                  const std::vector<double> & n, const double dt);
    
    /** @brief      Execute output equation
    *   @param      t Time
    *   @param      x State vector
    *   @param      u Input vector
    *   @param      n Sensor noise vector
    *   @param      z Output vector. This gets updated to the new output at the given time.
    **/
    void outputEqn(const double t, const std::vector<double> & x, const std::vector<double> & u,
                   const std::vector<double> & n, std::vector<double> & z);
    
    /** @brief      Execute threshold equation
    *   @param      t Time
    *   @param      x State vector
    *   @param      u Input vector
    **/
    bool thresholdEqn(const double t, const std::vector<double> & x, const std::vector<double> & u);
    
    /** @brief      Execute input equation.
    *               Determines what input (u) should be at the given time for the given input parameters.
    *   @param      t Time
    *   @param      inputParameters Vector of input parameters, which are values that specify how to define u for the given time.
    *   @param      u Input vector. Gets overwritten.
    **/
    void inputEqn(const double t, const std::vector<double> & inputParameters, std::vector<double> & u);
    
    /** @brief      Execute predicted output equation.
    *               Predicted outputs are those that are not measured, but are interested in being predicted for prognostics.
    *   @param      t Time
    *   @param      x State vector
    *   @param      u Input vector
    *   @param      z Predicted output vector. Gets overwritten.
    **/
    void predictedOutputEqn(const double t, const std::vector<double> & x,
                            const std::vector<double> & u, std::vector<double> & z);

    /** @brief      Initialize state vector given initial inputs and outputs.
    *   @param      x Current state vector. This gets updated.
    *   @param      u Input vector
    *   @param      z Output vector
    **/
    void initialize(std::vector<double> & x, const std::vector<double> & u, const std::vector<double> & z);
};


#endif
