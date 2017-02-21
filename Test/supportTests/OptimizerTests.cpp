/**  OptimizerTests - Body
*   @file      Unit tests for Optimizer classes
*   @ingroup   GPIC++
*   @ingroup   Optimizers
*
*   @brief     Unit tests for Optimizer classes
*
*   @author    Matthew Daigle
*   @version   0.1.0
*
*   @pre       N/A
*
*      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
*      Created: August 16, 2016
*
*   @copyright Copyright (c) 2016 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#include <iostream>
#include <cmath>

#include "Test.h"
#include "ThreadSafeLog.h"
#include "OptimizerTests.h"
#include "Battery.h"
#include "BatteryEOL.h"
#include "ParticleSwarmOptimizer.h"

using namespace PCOE;
using namespace PCOE::Test;

void optimizerTestsInit() {
    // Set up the log
    Log & log = Log::Instance("OptimizerTests.log");
    log.Initialize("OptimizerTests", "1.0", "No comments.");

    // Create the needed factories
    ModelFactory & pModelFactory = ModelFactory::instance();

    // Register battery model
    pModelFactory.Register("Battery", ModelFactory::Create<Battery>);
}

void testBatteryParamEst() {
    // Create battery model
    Battery battery = Battery();
    
    // Set different aging parameters
    battery.setParameters(battery.parameters.qMobile-500);
    battery.parameters.Ro = 1.5*battery.parameters.Ro;
    battery.parameters.tDiffusion = 1.2*battery.parameters.tDiffusion;
    
    // Create some discharge data...
    std::vector<std::vector<double>> data;
    
    // Initialize discharge model
    std::vector<double> u(battery.getNumInputs());
    std::vector<double> xd(battery.getNumStates());
    std::vector<double> z(battery.getNumOutputs());
    std::vector<double> processNoise(battery.getNumStates(),0);
    std::vector<double> sensorNoise(battery.getNumOutputs(),0);
    u[0] = 0;  // power
    z[battery.indices.outputs.Tbm] = 20;    // temperature
    z[battery.indices.outputs.Vm] = 4.2;    // voltage
    battery.initialize(xd,u,z);
    
    // Set initial input
    u[0] = 4.2*2.2;
    
    // Simulate until discharge
    double t = 0;
    while (!battery.thresholdEqn(t,xd,u)) {
        // Add data for this time
        std::vector<double> datum = {t, u[0], z[1]};
        data.push_back(datum);
        // Update state for t+dt
        battery.stateEqn(t,xd,u,processNoise,battery.getDt());
        // Update time
        t += battery.getDt();
        // Compute outputs
        battery.outputEqn(t,xd,u,sensorNoise,z);
        // Update input
        u[0] = z[battery.indices.outputs.Vm]*2.2;
    }
    
    // Create descent optimizer
    ParticleSwarmOptimizer opt = ParticleSwarmOptimizer();
    std::vector<double> p0 = {0.1, 7600, 7e6};
    std::vector<double> p(3);
    double error = opt.minimize(p0,[&data](std::vector<double> x)->double {
        return evaluateBattery(data,x);
    },p);
    //std::cout << "Optimal value:" << p[0] << ", " << p[1] << ", " << p[2] << std::endl;
    //std::cout << "Actual value:" << battery.parameters.Ro << ", " << battery.parameters.qMobile << ", " << battery.parameters.tDiffusion << std::endl;
    
    // Check accuracy
    Assert::AreEqual(battery.parameters.qMobile,p[1],0.05*battery.parameters.qMobile,"qMobile value");
    Assert::AreEqual(battery.parameters.Ro,p[0],0.05*battery.parameters.Ro,"Ro value");
    Assert::AreEqual(battery.parameters.tDiffusion,p[2],0.5*battery.parameters.tDiffusion,"tDiffusion value");
    Assert::AreEqual(error,1,1,"Final error");
}

double evaluateBattery(std::vector<std::vector<double>> & data, std::vector<double> x) {
    // Extract aging parameters
    double Ro = x[0];
    double qMobile = x[1];
    double tDiffusion = x[2];
    
    // Create battery model
    Battery battery;
    battery.setParameters(qMobile);
    battery.parameters.Ro = Ro;
    battery.parameters.tDiffusion = tDiffusion;
    
    // Initialize discharge model
    std::vector<double> u(battery.getNumInputs());
    std::vector<double> xd(battery.getNumStates());
    std::vector<double> z(battery.getNumOutputs());
    std::vector<double> processNoise(battery.getNumStates(),0);
    std::vector<double> sensorNoise(battery.getNumOutputs(),0);
    u[0] = data[0][1];  // power
    z[battery.indices.outputs.Tbm] = 20;    // temperature
    z[battery.indices.outputs.Vm] = data[0][2];    // voltage
    battery.initialize(xd,u,z);
    
    // Initialize error
    double e = 0;
    
    // Simulate until discharge
    double t = data[0][0];
    for (size_t i=1; i<data.size(); i++) {
        // Update state for t+dt
        battery.stateEqn(t,xd,u,processNoise,battery.getDt());
        // Update time
        t = data[i][0];
        // Compute outputs
        battery.outputEqn(t,xd,u,sensorNoise,z);
        // Update input
        u[0] = data[i][1];
        // Add to error
        e = e + (data[i][2]-z[1])*(data[i][2]-z[1]);
    }
    
    return e;
}

void testBatteryAgingRateParamEst() {
    // Construct some history of aging parameters
    std::vector<double> qMobiles = {7600, 7500, 7400, 7300, 7100, 6900, 6800};
    std::vector<double> Ros = {0.1, 0.11, 0.13, 0.15, 0.16, 0.20, 0.22};
    std::vector<double> tDiffusions = {7e6, 6.9e6, 6.7e6, 6.5e6, 6.8e6, 6.2e6, 6.0e6};
    // Construct history of average current
    std::vector<double> currents = {1, 2, 2.2, 1.8, 1.0, 1.3};
    // Construct history of cycle durations (dt for the EOL model)
    std::vector<double> dts = {3600, 3600, 3600, 3000, 3200, 3600};
    
    // Create descent optimizer
    ParticleSwarmOptimizer opt = ParticleSwarmOptimizer();
    // Initialize aging rate parameters
    std::vector<double> p0 = {5e-6, -0.03, 100};
    std::vector<double> p(3);
    double error = opt.minimize(p0,[&qMobiles,&Ros,&tDiffusions,&currents,&dts](std::vector<double> x)->double {
        return evaluateBatteryEOL(x,qMobiles,Ros,tDiffusions,currents,dts);
    },p);
    //std::cout << "Optimal value:" << p[0] << ", " << p[1] << ", " << p[2] << std::endl;

    // Check accuracy
    Assert::AreEqual(p[0],5.2e-6,2e-6,"wRo value");
    Assert::AreEqual(p[1],-0.0347,0.015,"wQMobile value");
    Assert::AreEqual(p[2],-25.6,5,"wTDiffusion value");
}

double evaluateBatteryEOL(std::vector<double> p, std::vector<double> & qMobiles, std::vector<double> & Ros, std::vector<double> & tDiffusions, std::vector<double> & currents, std::vector<double> & dts) {
    // For given aging rate parameters (p), simulate batteryEOL model, and compute error based
    // on how those parameters reproduce the given aging parameters
    
    // Create battery EOL model
    BatteryEOL batteryEOL;
    
    // Set aging rate parameters based on p
    batteryEOL.parameters.wRo = p[0];
    batteryEOL.parameters.wQMobile = p[1];
    batteryEOL.parameters.wTDiffusion = p[2];
    
    // State is initialized to initial data
    std::vector<double> x(batteryEOL.getNumStates());
    x[batteryEOL.indices.states.qMobile] = qMobiles[0];
    x[batteryEOL.indices.states.Ro] = Ros[0];
    x[batteryEOL.indices.states.tDiffusion] = tDiffusions[0];
    
    // Setup zero process noise
    std::vector<double> processNoise(batteryEOL.getNumStates(),0);
    
    // Initialize error
    double e = 0;
    
    // Simulate given currents and dts
    double t = 0;
    for (size_t i=0; i<currents.size(); i++) {
        // Update state for t+dt
        std::vector<double> u = {currents[i]};
        batteryEOL.stateEqn(t,x,u,processNoise,dts[i]);
        // Update time
        t += dts[i];
        // Compute unscaled errors
        double eQ = (qMobiles[i+1]-x[batteryEOL.indices.states.qMobile])*(qMobiles[i+1]-x[batteryEOL.indices.states.qMobile]);
        double eR = (Ros[i+1]-x[batteryEOL.indices.states.Ro])*(Ros[i+1]-x[batteryEOL.indices.states.Ro]);
        double eT = (tDiffusions[i+1]-x[batteryEOL.indices.states.tDiffusion])*(tDiffusions[i+1]-x[batteryEOL.indices.states.tDiffusion]);
        // Normalize errors by initial values and add to error
        e += eQ/qMobiles[0];
        e += eR/Ros[0];
        e += eT/tDiffusions[0];
        // NOTE: May want to scale errors before mean squared... must be standard ways to handle this...
        //std::cout << "raw errors are " << eQ << "," << eR << "," << eT << std::endl;
        //std::cout << "scaled are " << eQ/qMobiles[0] << "," << eR/Ros[0] << "," << eT/tDiffusions[0] << std::endl;
    }
    
    //std::cout << "For " << p[0] << "," << p[1] << "," << p[2] << " e is " << e << std::endl;
    
    return e;
}
