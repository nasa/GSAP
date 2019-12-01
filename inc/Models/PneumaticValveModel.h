// Copyright (c) 2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_PNEUMATICVALVEMODEL_H
#define PCOE_PNEUMATICVALVEMODEL_H

#include <cmath>
#include <vector>

#include "ConfigMap.h"
#include "PrognosticsModel.h"

/**
 * A model of a Pneumatic Valve
 * Original Model by Matthew Daigle can be found in:
 *   M. Daigle and K. Goebel, "A Model-based Prognostics Approach Applied to Pneumatic Valves," International Journal of Prognostics and Health Management, vol. 2, no. 2, August 2011.
 *
 * @author Matthew Daigle
 * @author Chris Teubert
 * @since 2.1
 **/
class PneumaticValveModel final : public PCOE::PrognosticsModel {
public:
    PneumaticValveModel();
    
    PneumaticValveModel(const PCOE::ConfigMap& paramMap);
    
    struct Parameters {
        // Environmental parameters
        double g = 9.81; // Acceleration of gravity in m/s^2
        double pAtm = 101325; // spring constant in N/m
        
        // Valve Parameters
        double m = 50.0; // plug mass in kg
        double r = 6e-3; // lumped nominal friction parameter
        double k = 4.8e4; // Spring constant (N/m)
        double x0 = 0.254; // Offset for displacement of spring (m)
        double Ap = 0.08107; // Surface area of piston for gas contact (m^2)
        double Ls = 0.0381; // Stroke length (m)
        double Vbot0 = 0.08107 * 0.1; // Below piston default volume (m^3)
        double Vtop0 = Vbot0; // Above piston default volume (m^3)
        double indicatorTol = 1e-3; // Tolerance bound for open/close indicators
        
        // Flow parameters
        double Av = 0.05067; // Surface area of plug end (m^2)
        double Cv = 0.08202/(Av * 3.7134); // Flow coefficient
        double rhoL = 70.99; // Density of LH2 in kg/m^3
        
        // Gas parameters
        struct GN2 {
            double M = 28.01e-3; // Molar mass of GN2 (kg/mol)
            double T = 293; // Temperature of GN2 (K) (default is ambient)
            double gamma = 1.4; // Specific heat ratio
            double Z = 1; // Gas copressibility factor (1=ideal gas)
        } gn2;
        double pSupply = 5.2724e6; // Pneumatic supply pressure (Pa)
        
        // Orifice parameters
        double At = 1e-5; // Orifice area(m^2)
        double Ct = 0.62;
        double Ab = 1e-5;
        double Cb = 0.62;
        
        // Fault parameter limits (defining EOL)
        double AbMax = 4e-5; // Maximum external bottom leak (m^2)
        double AtMax = 4e-5; // Maximum external top leak (m^2)
        double AiMax = 1.7e-6; // Maximum internal leak (m^2)
        double rMax = 4e6; // Maximum friction
        double kMin = 3.95e4; // Minimum spring coefficient
    } params;
    
    /**
     * Calculate the model state using the given sampling time.
     *
     * @param t  Time
     * @param x  The model state vector at the current time step.
     * @param u  The model input vector at the current time step.
     * @param dt The size of the time step to calculate
     * @return   The model state vector at the next time step.
     **/
    state_type stateEqn(double t,
                        const state_type& x,
                        const input_type& u,
                        double dt) const override;
    
    /**
     * Calculate the model output.
     *
     * @param t  Time
     * @param x  The model state vector at the current time step.
     * @return   The model output vector at the next time step.
     **/
    output_type outputEqn(double t, const state_type& x) const override;
    
    /**
     * Initialize the model state.
     * @returns The initial model state vector.
     **/
    state_type initialize(const input_type& u, const output_type&) const override;
    
    event_state_type eventStateEqn(const state_type& x) const override;
};
#endif
