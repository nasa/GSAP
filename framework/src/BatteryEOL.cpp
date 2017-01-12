/**  BatteryEOL - Body
 *   @file       BatteryEOL.cpp
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

#include <vector>
#include <math.h>
#include "BatteryEOL.h"

#include "ConfigMap.h"

#include <iostream>

using namespace PCOE;

// Configuration Keys
const std::string WQMOBILE_KEY = "Battery.wQMobile";
const std::string WRO_KEY = "Battery.wRo";
const std::string WTDIFF_KEY = "Battery.wTDiffusion";
const std::string MINC_KEY = "Battery.minCapacity";

// Constructor
BatteryEOL::BatteryEOL() {
    numStates = 3;
    numInputs = 1;
    numOutputs = 1;
    numInputParameters = 1;
    numPredictedOutputs = 1;
    m_dt = 1;
    // Set default parameter values
    setParameters();
}

// Constructor based on configMap
BatteryEOL::BatteryEOL(const ConfigMap & configMap) : BatteryEOL::BatteryEOL() {
    if (configMap.includes(WQMOBILE_KEY)) {
        parameters.wQMobile = std::stod(configMap.at(WQMOBILE_KEY)[0]);
    }
    if (configMap.includes(WRO_KEY)) {
        parameters.wRo = std::stod(configMap.at(WRO_KEY)[0]);
    }
    if (configMap.includes(WTDIFF_KEY)) {
        parameters.wTDiffusion = std::stod(configMap.at(WTDIFF_KEY)[0]);
    }
    if (configMap.includes(MINC_KEY)) {
        parameters.minCapacity = std::stod(configMap.at(MINC_KEY)[0]);
    }
}

// BatteryEOL State Equation
void BatteryEOL::stateEqn(const double t, std::vector<double> & x, const std::vector<double> & u, const std::vector<double> & n, double dt) {

    // Extract states
    double Ro = x[0];
    double qMobile = x[1];
    double tDiffusion = x[2];

    // Extract inputs
    double current = u[0];

    // Constraints
    double qMobiledot = parameters.wQMobile*fabs(current);
    double tDiffusiondot = parameters.wTDiffusion*fabs(current);
    double Rodot = parameters.wRo*fabs(current);

    // Update state
    x[0] = Ro + Rodot*dt;
    x[1] = qMobile + qMobiledot*dt;
    x[2] = tDiffusion + tDiffusiondot*dt;

    // Add process noise
    x[0] += dt*n[0];
    x[1] += dt*n[1];
    x[2] += dt*n[2];

}


// BatteryEOL Output Equation
void BatteryEOL::outputEqn(const double t, const std::vector<double> & x, const std::vector<double> & u, const std::vector<double> & n, std::vector<double> & z) {

    // Extract states
    double Ro = x[0];
    double qMobile = x[1];
    double tDiffusion = x[2];

    // Extract inputs
    double current = u[0];

    // Constraints
    double capacity = simulateReferenceDischarge(x);

    // Set outputs
    z[0] = capacity;

    // Add noise
    z[0] += n[0];

}


// Battery Threshold Equation
bool BatteryEOL::thresholdEqn(const double t, const std::vector<double> & x, const std::vector<double> & u) {
    
    // Extract states
    double Ro = x[0];
    double qMobile = x[1];
    double tDiffusion = x[2];
    
    // Extract inputs
    double current = u[0];
    
    // Constraints
    double capacity = simulateReferenceDischarge(x);
    
    return (capacity < parameters.minCapacity);
}


// Battery Input Equation
void BatteryEOL::inputEqn(const double t, const std::vector<double> & inputParameters, std::vector<double> & u) {
    if (inputParameters.empty()) {
        u[0] = parameters.nominalDischargeCurrent;
    }
    else {
        u[0] = inputParameters[0];
    }
}


// Battery Predicted Outputs Equation
void BatteryEOL::predictedOutputEqn(const double t, const std::vector<double> & x, const std::vector<double> & u, std::vector<double> & z) {
    // Use default output equation
    std::vector<double> zeroNoise(getNumOutputs());
    outputEqn(t, x, u, zeroNoise, z);
}


// Initialize state
void BatteryEOL::initialize(std::vector<double> & x, const std::vector<double> & u, const std::vector<double> & z) {
    // Ensure discharge model has been set
    checkDischargeModel();
    
    // Set default values. There is no one-to-one mapping between measured capacity and the
    // three aging parameters.
    x[0] = dischargeModel->parameters.Ro;
    x[1] = dischargeModel->parameters.qMobile;
    x[2] = dischargeModel->parameters.tDiffusion;
}

// Set default parameters
void BatteryEOL::setParameters() {
    parameters.wQMobile = 0;
    parameters.wRo = 0;
    parameters.wTDiffusion = 0;
    parameters.nominalDischargeCurrent = 2.2;
    parameters.minCapacity = 0.5*6952;  // 50% nominal
}


// Set discharge model
void BatteryEOL::setDischargeModel(Battery * model) {
    dischargeModel = model;
}


// Check discharge model has been set
void BatteryEOL::checkDischargeModel() {
    if (dischargeModel==NULL) {
        throw std::runtime_error("BatteryEOL: Discharge model has not been set!");
    }
}


// Simulate discharge model to compute capacity
double BatteryEOL::simulateReferenceDischarge(const std::vector<double> & x) {
    // Simulate discharge model reference conditions.
    // For example, if nominal capacity is 2.2 Ah, we simulate with a discharge current of 2.2A,
    // and determine how long it takes to discharge. This time*2.2 is the capacity in Ah.
    
    // Ensure discharge model has been set
    checkDischargeModel();
    
    // Extract states
    double Ro = x[0];
    double qMobile = x[1];
    double tDiffusion = x[2];
    
    // Set parameters of discharge model. We set using qMobile first, since some
    // other parameters are dependent on its value.
    dischargeModel->setParameters(qMobile);
    dischargeModel->parameters.Ro = Ro;
    dischargeModel->parameters.tDiffusion = tDiffusion;
    
    // Initialize discharge model
    std::vector<double> u(dischargeModel->getNumInputs());
    std::vector<double> xd(dischargeModel->getNumStates());
    std::vector<double> z(dischargeModel->getNumOutputs());
    std::vector<double> processNoise(dischargeModel->getNumStates(),0);
    std::vector<double> sensorNoise(dischargeModel->getNumOutputs(),0);
    u[0] = 0;  // power
    z[dischargeModel->indices.outputs.Tbm] = 20;    // temperature
    z[dischargeModel->indices.outputs.Vm] = 4.2;    // voltage
    dischargeModel->initialize(xd,u,z);
    
    // Set initial input
    u[0] = 4.2*parameters.nominalDischargeCurrent;
    
    // Simulate until discharge (determined by thresholdEqn)
    double t = 0;
    while (!dischargeModel->thresholdEqn(t,xd,u)) {
        // Update state for t+dt
        dischargeModel->stateEqn(t,xd,u,processNoise,dischargeModel->getDt());
        // Update time
        t += dischargeModel->getDt();
        // Compute outputs
        dischargeModel->outputEqn(t,xd,u,sensorNoise,z);
        // Update input
        u[0] = z[dischargeModel->indices.outputs.Vm]*parameters.nominalDischargeCurrent;
    }
    
    // Compute capacity
    double capacity = t*parameters.nominalDischargeCurrent;
    return capacity;
    
}

