// Copyright (c) 2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Models/CircuitBatteryModel.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>

#include "ConfigMap.h"
#include "Contracts.h"

using namespace PCOE;

static const Log& logFile = Log::Instance();

// CONST: Outputs
enum OUT { TEMP = 0, VOLTS = 1 };

// Configuration Keys
const std::string ID_KEY = "Battery.Id";
const std::string TD_KEY = "Battery.td";
const std::string V0_KEY = "Battery.V0";
const std::string QD_KEY = "Battery.qd";
const std::string Q0_KEY = "Battery.q0";
const std::string QMAX_KEY = "Battery.qMax";
const std::string CMAX_KEY = "Battery.cMax";
const std::string RP_KEY = "Battery.Rp";
const std::string P_KEY = "Battery.p";
const std::string CB0_KEY = "Battery.Cb0";
const std::string CBP0_KEY = "Battery.Cbp0";
const std::string CBP1_KEY = "Battery.Cbp1";
const std::string CBP2_KEY = "Battery.Cbp2";
const std::string CBP3_KEY = "Battery.Cbp3";
const std::string RS_KEY = "Battery.Rs";
const std::string CS_KEY = "Battery.Cs";
const std::string RCP0_KEY = "Battery.Rcp0";
const std::string RCP1_KEY = "Battery.Rcp1";
const std::string RCP2_KEY = "Battery.Rcp2";
const std::string CCP_KEY = "Battery.Ccp";
const std::string TA_KEY = "Battery.Ta";
const std::string JT_KEY = "Battery.Jt";
const std::string HA_KEY = "Battery.ha";
const std::string HCP_KEY = "Battery.hcp";
const std::string HCS_KEY = "Battery.hcs";
const std::string VEOD_KEY = "Battery.VEOD";
const std::string X0_QB_KEY = "Battery.x0.qb";
const std::string X0_QCP_KEY = "Battery.x0.qcp";
const std::string X0_QCS_KEY = "Battery.x0.qcs";
const std::string X0_TB_KEY = "Battery.x0.Tb";
const std::string V_QB_KEY = "Battery.v.qb";
const std::string V_QCP_KEY = "Battery.v.qcp";
const std::string V_QCS_KEY = "Battery.v.qcs";
const std::string V_TB_KEY = "Battery.v.Tb";
const std::string N_VM_KEY = "Battery.n.Vm";
const std::string N_TBM_KEY = "Battery.v.Tbm";

CircuitBatteryModel::CircuitBatteryModel()
    : PrognosticsModel(8,
                       {MessageId::Amperes},
                       {MessageId::Volts, MessageId::Centigrade},
                       {},
                       {MessageId::BatteryEod}) {
    // Set some default parameters
    setParameters();
}

// Constructor based on configMap
CircuitBatteryModel::CircuitBatteryModel(const ConfigMap& configMap) : CircuitBatteryModel::CircuitBatteryModel
() {
    setParameters();
    if (configMap.hasKey(ID_KEY)) {
        parameters.Id = configMap.getDouble(ID_KEY);
    }
    if (configMap.hasKey(TD_KEY)) {
        parameters.td = configMap.getDouble(TD_KEY);
    }
    if (configMap.hasKey(V0_KEY)) {
        parameters.V0 = configMap.getDouble(V0_KEY);
    }
    if (configMap.hasKey(QD_KEY)) {
        parameters.qd = configMap.getDouble(QD_KEY);
    }
    if (configMap.hasKey(Q0_KEY)) {
        parameters.q0 = configMap.getDouble(Q0_KEY);
    }
    if (configMap.hasKey(QMAX_KEY)) {
        parameters.qMax = configMap.getDouble(QMAX_KEY);
    }
    if (configMap.hasKey(CMAX_KEY)) {
        parameters.CMax = configMap.getDouble(CMAX_KEY);
    }
    if (configMap.hasKey(RP_KEY)) {
        parameters.Rp = configMap.getDouble(RP_KEY);
    }
    if (configMap.hasKey(P_KEY)) {
        parameters.p = configMap.getDoubleVector(P_KEY);
    }
    if (configMap.hasKey(CB0_KEY)) {
        parameters.Cb0 = configMap.getDouble(CB0_KEY);
    }
    if (configMap.hasKey(CBP0_KEY)) {
        parameters.Cbp0 = configMap.getDouble(CBP0_KEY);
    }
    if (configMap.hasKey(CBP1_KEY)) {
        parameters.Cbp1 = configMap.getDouble(CBP1_KEY);
    }
    if (configMap.hasKey(CBP2_KEY)) {
        parameters.Cbp2 = configMap.getDouble(CBP2_KEY);
    }
    if (configMap.hasKey(CBP3_KEY)) {
        parameters.Cbp3 = configMap.getDouble(CBP3_KEY);
    }
    if (configMap.hasKey(RS_KEY)) {
        parameters.Rs = configMap.getDouble(RS_KEY);
    }
    if (configMap.hasKey(CS_KEY)) {
        parameters.Cs = configMap.getDouble(CS_KEY);
    }
    if (configMap.hasKey(RCP0_KEY)) {
        parameters.Rcp0 = configMap.getDouble(RCP0_KEY);
    }
    if (configMap.hasKey(RCP1_KEY)) {
        parameters.Rcp1 = configMap.getDouble(RCP1_KEY);
    }
    if (configMap.hasKey(RCP2_KEY)) {
        parameters.Rcp2 = configMap.getDouble(RCP2_KEY);
    }
    if (configMap.hasKey(CCP_KEY)) {
        parameters.Ccp = configMap.getDouble(CCP_KEY);
    }
    if (configMap.hasKey(TA_KEY)) {
        parameters.Ta = configMap.getDouble(TA_KEY);
    }
    if (configMap.hasKey(JT_KEY)) {
        parameters.Jt = configMap.getDouble(JT_KEY);
    }
    if (configMap.hasKey(HA_KEY)) {
        parameters.ha = configMap.getDouble(HA_KEY);
    }
    if (configMap.hasKey(HCP_KEY)) {
        parameters.hcp = configMap.getDouble(HCP_KEY);
    }
    if (configMap.hasKey(HCS_KEY)) {
        parameters.hcs = configMap.getDouble(HCS_KEY);
    }
    if (configMap.hasKey(VEOD_KEY)) {
        parameters.VEOD = configMap.getDouble(VEOD_KEY);
    }
    if (configMap.hasKey(X0_QB_KEY)) {
        parameters.x0.qb = configMap.getDouble(X0_QB_KEY);
    }
    if (configMap.hasKey(X0_QCP_KEY)) {
        parameters.x0.qcp = configMap.getDouble(X0_QCP_KEY);
    }
    if (configMap.hasKey(X0_QCS_KEY)) {
        parameters.x0.qcs = configMap.getDouble(X0_QCS_KEY);
    }
    if (configMap.hasKey(X0_TB_KEY)) {
        parameters.x0.Tb = configMap.getDouble(X0_TB_KEY);
    }
    if (configMap.hasKey(V_QB_KEY)) {
        parameters.v.qb = configMap.getDouble(V_QB_KEY);
    }
    if (configMap.hasKey(V_QCP_KEY)) {
        parameters.v.qcp = configMap.getDouble(V_QCP_KEY);
    }
    if (configMap.hasKey(V_QCS_KEY)) {
        parameters.v.qcs = configMap.getDouble(V_QCS_KEY);
    }
    if (configMap.hasKey(V_TB_KEY)) {
        parameters.v.Tb = configMap.getDouble(V_TB_KEY);
    }
    if (configMap.hasKey(N_VM_KEY)) {
        parameters.n.Vm = configMap.getDouble(N_VM_KEY);
    }
    if (configMap.hasKey(N_TBM_KEY)) {
        parameters.n.Tbm = configMap.getDouble(N_TBM_KEY);
    }
}

// Battery State Equation
SystemModel::state_type CircuitBatteryModel::stateEqn(double,
                                               const state_type& x,
                                               const input_type& u,
                                               double dt) const {
    // StateEqn   Compute the new states of the battery model
    //
    // XNew = StateEqn(parameters,t,X,U,N,dt) computes the new states of the
    // battery model given the parameters strcucture, the current time, the
    //  current states, inputs, process noise, and the sampling time.
    
    // Extract states
    double Tb = x[0];
    double qb = x[1];
    double qcp = x[2];
    double qcs = x[3];

    // Extract inputs
    double i = u[0];

    // Constraints
    auto Vcs = qcs/parameters.Cs;
    auto Vcp = qcp/parameters.Ccp;
    auto SOC = (parameters.CMax - parameters.qMax + qb)/parameters.CMax;
    auto Cb = parameters.Cbp0*pow(SOC,3) + parameters.Cbp1*SOC*SOC+ parameters.Cbp2*SOC + parameters.Cbp3;
    auto Rcp = parameters.Rcp0 + parameters.Rcp1*exp(parameters.Rcp2*(-SOC + 1));
    auto Vb = qb/Cb;
    auto Tbdot = (Rcp*parameters.Rs*parameters.ha*(parameters.Ta - Tb) + Rcp*Vcs*Vcs*parameters.hcs + parameters.Rs*Vcp*Vcp*parameters.hcp)/(parameters.Jt*Rcp*parameters.Rs);
    auto Vp = Vb - Vcp - Vcs;
    auto ip = Vp/parameters.Rp;
    auto ib = i + ip;
    auto icp = ib - Vcp/Rcp;
    auto qcpdot = icp;
    auto qbdot = -ib;
    auto ics = ib - Vcs/parameters.Rs;
    auto qcsdot = ics;

    // Update state
    SystemModel::state_type x_new(x.size());
    x_new[0] = Tb + Tbdot*dt;
    x_new[1] = qb + qbdot*dt;
    x_new[2] = qcp + qcpdot*dt;
    x_new[3] = qcs + qcsdot*dt;
    
    return x_new;
}

// Battery Output Equation
SystemModel::output_type CircuitBatteryModel::outputEqn(double,
                                                 const state_type& x) const {
    // Extract states
    auto Tbm = x[0];
    auto qb = x[1];
    auto qcp = x[2];
    auto qcs = x[3];

    // Constraints
    auto Vcs = qcs/parameters.Cs;
    auto Vcp = qcp/parameters.Ccp;
    auto SOC = (parameters.CMax - parameters.qMax + qb)/parameters.CMax;
    auto Cb = parameters.Cbp0*pow(SOC,3) + parameters.Cbp1*SOC*SOC + parameters.Cbp2*SOC + parameters.Cbp3;
    auto Vb = qb/Cb;
    auto Vp = Vb - Vcp - Vcs;
    auto Vm = Vp;

    // set outputs
    SystemModel::output_type z(2);
    z[OUT::TEMP] = Tbm;
    z[OUT::VOLTS] = Vm;
    return z;
}

// Battery Threshold Equation
std::vector<bool> CircuitBatteryModel::thresholdEqn(double t, const state_type& x) const {
    // Compute based on voltage, so use output equation to get voltage
    auto z = outputEqn(t, x);

    // Determine if voltage (second element in z) is below VEOD threshold
    return {z[1] <= parameters.VEOD};
}

SystemModel::event_state_type CircuitBatteryModel::eventStateEqn(const state_type& x) const {
    // Compute "nominal" SOC
    auto SOC = (parameters.CMax - parameters.qMax + x[indices.states.Qb])/parameters.CMax;
    return event_state_type({SOC});
}

// Set model parameters, given qMobile
void CircuitBatteryModel::setParameters() {
    
    // Basics
    parameters.Id = 2.02;
    parameters.td = 3850;
    parameters.V0 = 4.183; // nominal battery voltage
    parameters.qd = parameters.Id*parameters.td;
    
    // If Cb was linear then: Cb = qd/Vd; but Cb is not linear so guessing that
    // initial charge is more than qd it must be large enough such that CbEnd > Cb0
    parameters.q0 = 1.0102*parameters.qd;
    // And assuming that "max" charge is initial charge (when qMax==qb SOC is 1)
    parameters.qMax = parameters.q0;
    // When qMax-qb=qd, SOC is 0 if CMax is the following (=qd) (CMax here is
    // more like max current that can be drawn, like a qdMax)
    parameters.CMax = parameters.qMax - (parameters.q0-parameters.qd);
    // Given those assumptions assumption it follows that
    parameters.Cb0 = parameters.q0/parameters.V0;
    // Then we can set the following:
    parameters.Rp = 1e4;         // battery parasitic resistance

    // Capacitance
    parameters.p = {1.0e3 *-0.23,1.0e3 * 0.0012, 1.0e3 *2.0799};
    parameters.Cb0 = parameters.Cb0; // battery capacitance
    parameters.Cbp0 = parameters.p[0];
    parameters.Cbp1 = parameters.p[1];
    parameters.Cbp2 = parameters.p[2];
    parameters.Cbp3 = parameters.Cb0-(parameters.Cbp0+parameters.Cbp1+parameters.Cbp2);

    // R-C pairs
    parameters.Rs = 0.0538926;
    parameters.Cs = 234.387;
    parameters.Rcp0 = 0.0697776;
    parameters.Rcp1 = 1.50528e-17;
    parameters.Rcp2 = 37.223;
    parameters.Ccp = 14.8223;

    // Temperature parameters
    parameters.Ta = 18.95; // ambient temperature (deg C)
    parameters.Jt = 800;
    parameters.ha = 0.5;   // heat transfer coefficient, ambient
    parameters.hcp = 19;
    parameters.hcs = 1;

    // End of discharge voltage threshold
    parameters.VEOD = 3.0;

    // Default initial states (fully charged)
    parameters.x0.qb = parameters.Cb0*parameters.V0;
    parameters.x0.qcp = 0;
    parameters.x0.qcs = 0;
    parameters.x0.Tb = parameters.Ta;

    // Process noise variances
    parameters.v.qb = 1e-1;
    parameters.v.qcp = 1e-2;
    parameters.v.qcs = 1e-2;
    parameters.v.Tb = 1e-5;

    // Sensor noise variances
    parameters.n.Vm = 1e-3;
    parameters.n.Tbm = 1e-3;
}
