/**  Battery - Header
*   @file       Battery.h
*   @ingroup    GSAP-Support
*
*   @brief      Battery model class for prognostics
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

#ifndef BATTERY_H
#define BATTERY_H

#include <cmath>
#include <vector>

#include "PrognosticsModel.h"
#include "ConfigMap.h"
#include "ModelFactory.h"
#include "PrognosticsModelFactory.h"

// Default parameter values
static const double QMOBILE_DEFAULT_VALUE = 7600;

class Battery final : public PCOE::PrognosticsModel {
 public:
    // Constructor
    Battery();

    // Constructor based on configMap
    Battery(const PCOE::ConfigMap & paramMap);

    // State indices
    struct stateIndices {
        static const unsigned int Tb = 0;
        static const unsigned int Vo = 1;
        static const unsigned int Vsn = 2;
        static const unsigned int Vsp = 3;
        static const unsigned int qnB = 4;
        static const unsigned int qnS = 5;
        static const unsigned int qpB = 6;
        static const unsigned int qpS = 7;
    };
    // Input indices
    struct inputIndices {
        static const unsigned int P = 0;
    };
    // Output indices
    struct outputIndices {
        static const unsigned int Tbm = 0;
        static const unsigned int Vm = 1;
    };
    // Indices
    struct allIndices {
        struct stateIndices states;
        struct inputIndices inputs;
        struct outputIndices outputs;
    } indices;

    // Parameters
    struct Parameters {
        double An2;
        double qnBMax;
        double U0p;
        double An7;
        double Ro;
        double Vol;
        double qnSMax;
        double F;
        double to;
        double Ap0;
        double Ap9;
        double An5;
        double An9;
        double qpBMax;
        double alpha;
        double VolSFraction;
        double VEOD;
        double qMax;
        double xpMin;
        double Ap1;
        double qpSMin;
        double An4;
        double Ap3;
        double qpSMax;
        double Ap4;
        double An11;
        double tsp;
        double kn;
        double Ap11;
        double Ap5;
        double kp;
        double R;
        double qnBMin;
        double An12;
        double An10;
        double VolS;
        double xpMax;
        double qBMax;
        double qSMax;
        double Ap8;
        double An6;
        double Sn;
        double qpMin;
        double Ap2;
        double tsn;
        double qnMin;
        double qpMax;
        double qnMax;
        double qnSMin;
        double U0n;
        double qpBMin;
        double VolB;
        double Ap6;
        double Sp;
        double Ap10;
        double An0;
        double xnMax;
        double Ap12;
        double An1;
        double Ap7;
        double An8;
        double xnMin;
        double tDiffusion;
        double qMobile;
        double An3;
    } parameters;
        
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

    /** @brief      Execute predicted output equation.
    *               Predicted outputs are those that are not measured, but are interested in being predicted for prognostics.
    *   @param      t Time
    *   @param      x State vector
    *   @param      u Input vector
    *   @param      z Predicted output vector. Gets overwritten.
    **/
    void predictedOutputEqn(const double t, const std::vector<double> & x,
                            const std::vector<double> & u, std::vector<double> & z);

    // Set default parameters, based on 18650 cells
    void setParameters(const double qMobile = QMOBILE_DEFAULT_VALUE, const double Vol = 2e-5);

    /** @brief      Initialize state vector given initial inputs and outputs.
    *   @param      x Current state vector. This gets updated.
    *   @param      u Input vector
    *   @param      z Output vector
    **/
    void initialize(std::vector<double> & x, const std::vector<double> & u, const std::vector<double> & z);
    
    /** @brief      Transform inputs and outputs. Specifically, given current as input
     *              change it to power.
     */
    virtual void transform(std::vector<double> & u, std::vector<double> & z);
};


#endif
