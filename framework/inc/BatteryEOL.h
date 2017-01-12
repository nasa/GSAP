/**  BatteryEOL - Header
 *   @file       BatteryEOL.h
 *   @ingroup    GSAP-Support
 *
 *   @brief      Battery EOL model class for prognostics
 *
 *   @author     Matthew Daigle
 *   @version    0.1.0
 *
 *   @pre        N/A
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: July 8, 2016
 *
 *   @copyright Copyright (c) 2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef BatteryEOL_h
#define BatteryEOL_h

#include <vector>
#include <math.h>
#include "Model.h"

#include "PrognosticsModel.h"
#include "ConfigMap.h"
#include "ModelFactory.h"
#include "PrognosticsModelFactory.h"

#include "Battery.h"

class BatteryEOL final: public PCOE::PrognosticsModel {
    
  private:
  
    Battery * dischargeModel;
    
    /** @brief    Simulate a reference discharge for the discharge model for given aging parameters
     *  @param    x EOL model state vector
     **/
    double simulateReferenceDischarge(const std::vector<double> & x);

  public:

    // Constructor
    BatteryEOL();
    
    // Constructor based on configMap
    BatteryEOL(const PCOE::ConfigMap & paramMap);

    // State indices
    struct stateIndices {
        static const unsigned int Ro = 0;
        static const unsigned int qMobile = 1;
        static const unsigned int tDiffusion = 2;
    };
    // Input indices
    struct inputIndices {
        static const unsigned int current = 0;
    };
    // Output indices
    struct outputIndices {
        static const unsigned int capacity = 0;
    };
    // Indices
    struct allIndices {
        struct stateIndices states;
        struct inputIndices inputs;
        struct outputIndices outputs;
    } indices;

    // Parameters
    struct Parameters {
        double wQMobile;
        double wRo;
        double wTDiffusion;
        double nominalDischargeCurrent;
        double minCapacity;
    } parameters;

    /** @brief      Execute state equation. This version of the function uses a given sampling time.
     *   @param      t Time
     *   @param      x Current state vector. This gets updated to the state at the new time.
     *   @param      u Input vector
     *   @param      n Process noise vector
     *   @param      dt Sampling time
     **/
    void stateEqn(const double t, std::vector<double> & x, const std::vector<double> & u, const std::vector<double> & n, const double dt);
    
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
    
    /** @brief    Set discharge model
     *  @param    model discharge model
     **/
    void setDischargeModel(Battery * model);
    
    /** @brief    Check that discharge model has been set
     **/
    void checkDischargeModel();
    
    /** @brief    Set default parameters
     **/
    void setParameters();

};

#endif
