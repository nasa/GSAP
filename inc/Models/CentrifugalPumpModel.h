// Copyright (c) 2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_CENTRIFUGALPUMPMODEL_H
#define PCOE_CENTRIFUGALPUMPMODEL_H

#include <cmath>
#include <vector>

#include "ConfigMap.h"
#include "PrognosticsModel.h"

// Default parameter values
static const double QMOBILE_DEFAULT_VALUE = 7600;

/**
 * A model of a Centrifugal Pump
 * Original Model by Matthew Daigle can be found here:
 *      http://matthewjdaigle.com/pubs/DaigleEtAl-PrognosticsConcurrentDamage-SMC.pdf
 *
 * @author Matthew Daigle
 * @author Chris Teubert
 * @since 2.0
 **/
class CentrifugalPumpModel final : public PCOE::PrognosticsModel {
public:
    CentrifugalPumpModel();
    
    CentrifugalPumpModel(const PCOE::ConfigMap& paramMap);
    
    struct Parameters {
        double sampleTime = 1;
        double cycleTime = 3600; // length of a pump usage cycle
        
        // Environmental parameters
        double pAtm = 101325;
        
        // Torque and pressure parameters
        double a0 = 0.00149204; // empirical coefficient for flow torque eqn
        double a1 = 5.77703; // empirical coefficient for flow torque eqn
        double a2 = 9179.4; // empirical coefficient for flow torque eqn
        double A = 12.7084;  // impeller blade area
        double b = 17984.6;
        
        // Pump/motor dynamics
        double I = 50; // impeller/shaft/motor lumped inertia
        double r = 0.008; // lumped friction parameter (minus bearing friction)
        double R1 = 0.36;
        double R2 = 0.076;
        double L1 = 0.00063;
        
        // Flow coefficients
        double FluidI = 5; // pump fluid inertia
        double c = 8.24123e-005; // pump flow coefficient
        double cLeak = 1; // internal leak flow coefficient
        double ALeak = 1e-10; // internal leak area
        
        // Thrust bearing temperature
        double mcThrust = 7.3;
        double rThrust = 1.4e-6;
        double HThrust1 = 0.0034;
        double HThrust2 = 0.0026;
        
        //  Radial bearing temperature
        double mcRadial = 2.4;
        double rRadial = 1.8e-006;
        double HRadial1 = 0.0018;
        double HRadial2 = 0.020;
        
        //  Bearing oil temperature
        double mcOil = 8000;
        double HOil1 = 1.0;
        double HOil2 = 3.0;
        double HOil3 = 1.5;
        
        //  Parameter limits
        double ALim = 9.5;
        double ToLim = 350;
        double TrLim = 370;
        double TtLim = 370;
        
        //  Initial state
        struct X0 {
            double A = 12.7084;
            double Q = 0;
            double To = 290;
            double Tr = 290;
            double Tt = 290;
            double rRadial = 1.8e-006;
            double rThrust = 1.4e-6;
            double w = 3600*2*M_PI/60;  //  3600 rpm
            double wA = 0;
            double wRadial = 0;
            double wThrust = 0;
        } x0;
        
        //  Process noise
        struct V {
            double w = 1e-3;
            double Q = 1e-8;
            double Tt = 1e-7;
            double Tr = 1e-7;
            double To = 1e-7;
            double A = 1e-30;
            double rThrust = 1e-30;
            double rRadial = 1e-30;
            double wA = 1e-30;
            double wThrust = 1e-30;
            double wRadial = 1e-30;
        } v;
        
        //  Sensor noise
        struct N {
            double wm = 1e-2;
            double Qoutm = 1e-7;
            double Ttm = 1e-2;
            double Trm = 1e-2;
            double Tom = 1e-2;
        } n;
    } parameters;
    
    /**
     * Calculate the model state using the given sampling time.
     *
     * @param t  Time
     * @param x  The model state vector at the current time step.
     * @param u  The model input vector at the current time step.
     * @param n  The process noise vector.
     * @param dt The size of the time step to calculate
     * @return   The model state vector at the next time step.
     **/
    state_type stateEqn(double t,
                        const state_type& x,
                        const input_type& u,
                        const noise_type& n,
                        double dt) const override;
    
    /**
     * Calculate the model output.
     *
     * @param t  Time
     * @param x  The model state vector at the current time step.
     * @param n  The process noise vector.
     * @return   The model output vector at the next time step.
     **/
    output_type outputEqn(double t, const state_type& x, const noise_type& n) const override;
    
    /**
     * Initialize the model state.
     *
     * @param u The model input vector.
     * @param z The model output vector.
     * @returns The initial model state vector.
     **/
    state_type initialize(const input_type& u, const output_type& z) const override;
    
    /**
     * Calculate whether the model threshold is reached.
     *
     * @param t  Time
     * @param x  The model state vector at the current time step.
     * @return  For each event: true if the threshold is reached; otherwise, false.
     **/
    std::vector<bool> thresholdEqn(double t, const state_type& x) const override;
    
    event_state_type eventStateEqn(const state_type& x) const override;
};
#endif
