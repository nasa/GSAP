// Copyright (c) 2016-2021 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_BATTERYMODEL_H
#define PCOE_BATTERYMODEL_H

#include <cmath>
#include <vector>

#include "ConfigMap.h"
#include "PrognosticsModel.h"

// Default parameter values
static const double QMOBILE_DEFAULT_VALUE = 7600;

/**
 * An electro-chemical model of a battery.
 *
 * @author Matthew Daigle
 * @author Chetan Kulkarni
 * @author Chris Teubert
 * @author Jason Watkins
 * @author Julian Vu
 * @since 1.0
 **/
class BatteryModel final : public PCOE::PrognosticsModel {
public:
    BatteryModel();

    BatteryModel(const PCOE::ConfigMap& paramMap);

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

    struct inputIndices {
        static const unsigned int P = 0;
    };

    struct outputIndices {
        static const unsigned int Tbm = 1;
        static const unsigned int Vm = 0;
    };

    struct allIndices {
        struct stateIndices states;
        struct inputIndices inputs;
        struct outputIndices outputs;
    } indices;

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
        double VDropoff;
        double Tau;
        double Tb0;
        double mC;
        
        struct CachedParams {
            double An12_F;
            double An11_F;
            double An10_F;
            double An9_F;
            double An8_F;
            double An7_F;
            double An6_F;
            double An5_F;
            double An4_F;
            double An3_F;
            double An2_F;
            double An1_F;
            double An0_F;
            double Ap12_F;
            double Ap11_F;
            double Ap10_F;
            double Ap9_F;
            double Ap8_F;
            double Ap7_F;
            double Ap6_F;
            double Ap5_F;
            double Ap4_F;
            double Ap3_F;
            double Ap2_F;
            double Ap1_F;
            double Ap0_F;
            double R_F;
            double R_FAlpha;
        } cached;
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

    // Set default parameters, based on 18650 cells
    void setParameters(const double qMobile = QMOBILE_DEFAULT_VALUE, const double Vol = 2e-5);
};
#endif
