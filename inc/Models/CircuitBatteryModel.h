// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_CIRCUITBATTERYMODEL_H
#define PCOE_CIRCUITBATTERYMODEL_H

#include <cmath>
#include <vector>

#include "ConfigMap.h"
#include "PrognosticsModel.h"

/**
 * A circuit-based model of a battery.
 *
 * @author Matthew Daigle
 * @author Chris Teubert
 * @since 2.1
 **/
class CircuitBatteryModel final : public PCOE::PrognosticsModel {
public:
    CircuitBatteryModel();

    CircuitBatteryModel(const PCOE::ConfigMap& paramMap);

    struct stateIndices {
        static const unsigned int Tb = 0;
        static const unsigned int Qb = 1;
        static const unsigned int Qcp = 2;
        static const unsigned int Qcs = 3;
    };

    struct inputIndices {
        static const unsigned int I = 0;
    };

    struct outputIndices {
        static const unsigned int Tbm = 0;
        static const unsigned int Vm = 1;
    };

    struct allIndices {
        struct stateIndices states;
        struct inputIndices inputs;
        struct outputIndices outputs;
    } indices;

    struct Parameters {
        double Id;
        double td;
        double V0;
        double qd;
        double q0;
        double qMax;
        double CMax;
        double Rp;
        std::vector<double> p;
        double Cb0;
        double Cbp0;
        double Cbp1;
        double Cbp2;
        double Cbp3;
        double Rs;
        double Cs;
        double Rcp0;
        double Rcp1;
        double Rcp2;
        double Ccp;
        double Ta;
        double Jt;
        double ha;
        double hcp;
        double hcs;
        double VEOD;
        struct X0 {
            double qb;
            double qcp;
            double qcs;
            double Tb;
        } x0;
    } parameters;

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
     * Calculate whether the model threshold is reached.
     *
     * @param t  Time
     * @param x  The model state vector at the current time step.
     * @return  For each event: true if the threshold is reached; otherwise, false.
     **/
    std::vector<bool> thresholdEqn(double t, const state_type& x) const override;

    event_state_type eventStateEqn(const state_type& x) const override;
    
    state_type initialize(const input_type&, const output_type&) const override {
        return state_type({
                           parameters.x0.Tb,
                           parameters.x0.qb,
                           parameters.x0.qcp,
                           parameters.x0.qcs
                          });
    }

    // Set default parameters
    void setParameters();
};
#endif
