// Copyright (c) 2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#include <algorithm>
#include <cmath>
#include <vector>

#include "ConfigMap.h"
#include "Contracts.h"
#include "Models/CentrifugalPumpModel.h"

using namespace PCOE;

// Configuration Keys
const std::string SAMPLETIME = "CentrifugalPump.SampleTime";
const std::string CYCLETIME = "CentrifugalPump.CycleTime";
const std::string PATM = "CentrifugalPump.pATM";
const std::string A0 = "CentrifugalPump.A0";
const std::string A1 = "CentrifugalPump.A1";
const std::string A2 = "CentrifugalPump.A2";
const std::string A = "CentrifugalPump.A";
const std::string B = "CentrifugalPump.B";
const std::string I = "CentrifugalPump.I";
const std::string R = "CentrifugalPump.R";
const std::string R1 = "CentrifugalPump.R1";
const std::string R2 = "CentrifugalPump.R2";
const std::string L1 = "CentrifugalPump.L1";
const std::string FLUIDI = "CentrifugalPump.FluidI";
const std::string C = "CentrifugalPump.c";
const std::string CLEAK = "CentrifugalPump.cLeak";
const std::string ALEAK = "CentrifugalPump.ALeak";
const std::string MCTHRUST = "CentrifugalPump.mcThrust";
const std::string RTHRUST = "CentrifugalPump.rThrust";
const std::string HTHRUST1 = "CentrifugalPump.HThrust1";
const std::string HTHRUST2 = "CentrifugalPump.HThrust2";
const std::string MCRADIAL = "CentrifugalPump.mcRadial";
const std::string RRADIAL = "CentrifugalPump.rRadial";
const std::string HRADIAL1 = "CentrifugalPump.HRadial1";
const std::string HRADIAL2 = "CentrifugalPump.HRadial2";
const std::string MCOIL = "CentrifugalPump.mcOil";
const std::string HOIL1 = "CentrifugalPump.HOil1";
const std::string HOIL2 = "CentrifugalPump.HOil2";
const std::string HOIL3 = "CentrifugalPump.HOil3";
const std::string ALIM = "CentrifugalPump.ALim";
const std::string TOLIM = "CentrifugalPump.ToLim";
const std::string TRLIM = "CentrifugalPump.TrLim";
const std::string TTLIM = "CentrifugalPump.TtLim";
const std::string X0_W = "CentrifugalPump.x0.w";
const std::string X0_Q = "CentrifugalPump.x0.Q";
const std::string X0_TT = "CentrifugalPump.x0.Tt";
const std::string X0_TR = "CentrifugalPump.x0.Tr";
const std::string X0_TO = "CentrifugalPump.x0.To";
const std::string X0_A = "CentrifugalPump.x0.A";
const std::string X0_RTHRUST = "CentrifugalPump.x0.rThrust";
const std::string X0_RRADIAL = "CentrifugalPump.x0.rRadial";
const std::string X0_WA = "CentrifugalPump.x0.wA";
const std::string X0_WTHRUST = "CentrifugalPump.x0.wThrust";
const std::string X0_WRADIAL = "CentrifugalPump.x0.wRadial";

const auto INPUT_ARRAY = {
    MessageId::Kelvin, // Ambient Temp
    MessageId::Volts, // Voltage
    MessageId::Pascal, // Discharge Pressure
    MessageId::Pascal, // Suction Pressure
    MessageId::RadiansPerSecond //Syncronous Rotational Speed of Supply Voltage
};

const auto OUTPUT_ARRAY = {
    MessageId::MetersCubedPerSecond, // Discharge Flow
    MessageId::Kelvin, // Oil Temperature
    MessageId::Kelvin, // Radial Bearing Temperature
    MessageId::Kelvin, // Thrust Bearing Temperature
    MessageId::RadiansPerSecond // Mechanical Rotation
};

const auto EVENT_ARRAY = {
    MessageId::CentrifugalPumpImpellerWearFailure,
    MessageId::CentrifugalPumpOilOverheat,
    MessageId::CentrifugalPumpRadialBearingOverheat,
    MessageId::CentrifugalPumpImpellerWearFailure
};

CentrifugalPumpModel::CentrifugalPumpModel()
    : PrognosticsModel(12,
                       INPUT_ARRAY,
                       OUTPUT_ARRAY,
                       {} /* Observables */,
                       EVENT_ARRAY) {
}

CentrifugalPumpModel::CentrifugalPumpModel(const ConfigMap& configMap) : CentrifugalPumpModel::CentrifugalPumpModel() {
    if (configMap.hasKey(SAMPLETIME)) {
        parameters.sampleTime = configMap.getDouble(SAMPLETIME);
    }
    if (configMap.hasKey(CYCLETIME)) {
        parameters.cycleTime = configMap.getDouble(CYCLETIME);
    }
    if (configMap.hasKey(PATM)) {
        parameters.pAtm = configMap.getDouble(PATM);
    }
    if (configMap.hasKey(A0)) {
        parameters.a0 = configMap.getDouble(A0);
    }
    if (configMap.hasKey(A1)) {
        parameters.a1 = configMap.getDouble(A1);
    }
    if (configMap.hasKey(A2)) {
        parameters.a2 = configMap.getDouble(A2);
    }
    if (configMap.hasKey(A)) {
        parameters.A = configMap.getDouble(A);
    }
    if (configMap.hasKey(B)) {
        parameters.b = configMap.getDouble(B);
    }
    if (configMap.hasKey(I)) {
        parameters.I = configMap.getDouble(I);
    }
    if (configMap.hasKey(R)) {
        parameters.r = configMap.getDouble(R);
    }
    if (configMap.hasKey(R1)) {
        parameters.R1 = configMap.getDouble(R1);
    }
    if (configMap.hasKey(R2)) {
        parameters.R2 = configMap.getDouble(R2);
    }
    if (configMap.hasKey(FLUIDI)) {
        parameters.FluidI = configMap.getDouble(FLUIDI);
    }
    if (configMap.hasKey(C)) {
        parameters.c = configMap.getDouble(C);
    }
    if (configMap.hasKey(CLEAK)) {
        parameters.cLeak = configMap.getDouble(CLEAK);
    }
    if (configMap.hasKey(ALEAK)) {
        parameters.ALeak = configMap.getDouble(ALEAK);
    }
    if (configMap.hasKey(MCTHRUST)) {
        parameters.mcThrust = configMap.getDouble(MCTHRUST);
    }
    if (configMap.hasKey(RTHRUST)) {
        parameters.rThrust = configMap.getDouble(RTHRUST);
    }
    if (configMap.hasKey(HTHRUST1)) {
        parameters.HThrust1 = configMap.getDouble(HTHRUST1);
    }
    if (configMap.hasKey(HTHRUST2)) {
        parameters.HThrust2 = configMap.getDouble(HTHRUST2);
    }
    if (configMap.hasKey(MCRADIAL)) {
        parameters.mcRadial = configMap.getDouble(MCRADIAL);
    }
    if (configMap.hasKey(RRADIAL)) {
        parameters.rRadial = configMap.getDouble(RRADIAL);
    }
    if (configMap.hasKey(HRADIAL1)) {
        parameters.HRadial1 = configMap.getDouble(HRADIAL1);
    }
    if (configMap.hasKey(HRADIAL2)) {
        parameters.HRadial2 = configMap.getDouble(HRADIAL2);
    }
    if (configMap.hasKey(MCOIL)) {
        parameters.mcOil = configMap.getDouble(MCOIL);
    }
    if (configMap.hasKey(HOIL1)) {
        parameters.HOil1 = configMap.getDouble(HOIL1);
    }
    if (configMap.hasKey(HOIL2)) {
        parameters.HOil2 = configMap.getDouble(HOIL2);
    }
    if (configMap.hasKey(HOIL3)) {
        parameters.HOil3 = configMap.getDouble(HOIL3);
    }
    if (configMap.hasKey(ALIM)) {
        parameters.ALim = configMap.getDouble(ALIM);
    }
    if (configMap.hasKey(TOLIM)) {
        parameters.ToLim = configMap.getDouble(TOLIM);
    }
    if (configMap.hasKey(TRLIM)) {
        parameters.TrLim = configMap.getDouble(TRLIM);
    }
    if (configMap.hasKey(TTLIM)) {
        parameters.TtLim = configMap.getDouble(TTLIM);
    }
    if (configMap.hasKey(X0_W)) {
        parameters.x0.w = configMap.getDouble(X0_W);
    }
    if (configMap.hasKey(X0_Q)) {
        parameters.x0.Q = configMap.getDouble(X0_Q);
    }
    if (configMap.hasKey(X0_TT)) {
        parameters.x0.Tt = configMap.getDouble(X0_TT);
    }
    if (configMap.hasKey(X0_TR)) {
        parameters.x0.Tr = configMap.getDouble(X0_TR);
    }
    if (configMap.hasKey(X0_TO)) {
        parameters.x0.To = configMap.getDouble(X0_TO);
    }
    if (configMap.hasKey(X0_A)) {
        parameters.x0.A = configMap.getDouble(X0_A);
    }
    if (configMap.hasKey(X0_RTHRUST)) {
        parameters.x0.rThrust = configMap.getDouble(X0_RTHRUST);
    }
    if (configMap.hasKey(X0_RRADIAL)) {
        parameters.x0.rRadial = configMap.getDouble(X0_RRADIAL);
    }
    if (configMap.hasKey(X0_WA)) {
        parameters.x0.wA = configMap.getDouble(X0_WA);
    }
    if (configMap.hasKey(X0_WTHRUST)) {
        parameters.x0.wThrust = configMap.getDouble(X0_WTHRUST);
    }
    if (configMap.hasKey(X0_WRADIAL)) {
        parameters.x0.wRadial = configMap.getDouble(X0_WRADIAL);
    }
}

double sign(double in) {
    return in/abs(in);
}

SystemModel::state_type CentrifugalPumpModel::stateEqn(double, const state_type& x, const input_type& u, double dt) const {
    
    // Extract states
    double A = x[0];
    double Q = x[1];
    double To = x[2];
    double Tr = x[3];
    double Tt = x[4];
    double rRadial = x[5];
    double rThrust = x[6];
    double w = x[7];
    double wA = x[8];
    double wRadial = x[9];
    double wThrust = x[10];
    
    // Extract inputs
    double Tamb = u[0];
    double V = u[1];
    double pdisch = u[2];
    double psuc = u[3];
    double wsync = u[4];
    
    // Cached Values
    double w_squared = w*w;
    
    // Constraints
    double Todot = 1.0/parameters.mcOil * (parameters.HOil1*(Tt-To) + parameters.HOil2*(Tr-To) + parameters.HOil3*(Tamb-To));
    double Ttdot = 1.0/parameters.mcThrust * (rThrust*w*w - parameters.HThrust1*(Tt-Tamb) - parameters.HThrust2*(Tt-To));
    double wAdot = 0;
    double wThrustdot = 0;
    double Adot = -wA*Q*Q;
    double wRadialdot = 0;
    double rRadialdot = wRadial*rRadial*w_squared;
    double rThrustdot = wThrust*rThrust*w_squared;
    double friction = (parameters.r+rThrust+rRadial)*w;
    double QLeak = parameters.cLeak*parameters.ALeak*sqrt(abs(psuc-pdisch))*sign(psuc-pdisch);
    double Trdot = 1/parameters.mcRadial * (rRadial*w_squared - parameters.HRadial1 *(Tr-Tamb) - parameters.HRadial2*(Tr-To));
    double slipn = (wsync-w)/wsync;
    double ppump = A*w_squared + parameters.b*w*Q;
    double Qout = fmax(0,Q-QLeak);
    double slip = fmax(-1,(fmin(1,slipn)));
    double deltaP = ppump+psuc-pdisch;
    double Te = 3*parameters.R2/slip/(wsync+0.00001)*V*V/(pow(parameters.R1+parameters.R2/slip,2)+pow(wsync*parameters.L1,2));
    double backTorque = -parameters.a2*Qout*Qout + parameters.a1*w*Qout + parameters.a0*w_squared;
    double Qo = parameters.c*sqrt(abs(deltaP))*sign(deltaP);
    double wdot = (Te-friction-backTorque)/parameters.I;
    double Qdot = 1/parameters.FluidI*(Qo-Q);
    
    // Update state
    auto x_new = getStateVector();
    x_new[0] = A + Adot*dt;
    x_new[1] = Q + Qdot*dt;
    x_new[2] = To + Todot*dt;
    x_new[3] = Tr + Trdot*dt;
    x_new[4] = Tt + Ttdot*dt;
    x_new[5] = rRadial + rRadialdot*dt;
    x_new[6] = rThrust + rThrustdot*dt;
    x_new[7] = w + wdot*dt;
    x_new[8] = wA + wAdot*dt;
    x_new[9] = wRadial + wRadialdot*dt;
    x_new[10] = wThrust + wThrustdot*dt;
    x_new[11] = parameters.cLeak*parameters.ALeak*sqrt(abs(psuc-pdisch))*sign(psuc-pdisch);
    
    return x_new;
}

SystemModel::output_type CentrifugalPumpModel::outputEqn(double,
                                                 const state_type& x) const {
    // Extract states
    double Q = x[1];
    double To = x[2];
    double Tr = x[3];
    double Tt = x[4];
    double w = x[7];
    double QLeak = x[11];
    
    // Constraints
    auto Ttm = Tt;
    auto Tom = To;
    auto wm = w;
    auto Trm = Tr;
    auto Qoutm = fmax(0,Q-QLeak);
    
    // set outputs
    auto z_new = getOutputVector();
    z_new[0] = Qoutm;
    z_new[1] = Tom;
    z_new[2] = Trm;
    z_new[3] = Ttm;
    z_new[4] = wm;
    
    return z_new;
}

std::vector<bool> CentrifugalPumpModel::thresholdEqn(double, const state_type& x) const {
    
    auto eventState = eventStateEqn(x);
    
    std::vector<bool> thresholdMet(eventState.size());
    
    for (size_type i = 0; i < eventState.size(); i++) {
        thresholdMet[i] = eventState[i] <= 0.0;
    }
    return thresholdMet;
}

SystemModel::event_state_type CentrifugalPumpModel::eventStateEqn(const state_type& x) const {
    
    // Extract states
    double A = x[0];
    double To = x[2];
    double Tr = x[3];
    double Tt = x[4];
    
    // Individual performance constraints
    auto BA = (A - parameters.ALim) / (parameters.x0.A - parameters.ALim);
    auto BTo = (parameters.ToLim - To) / (parameters.ToLim - parameters.x0.To);
    auto BTt = (parameters.TtLim - Tt) / (parameters.TtLim - parameters.x0.Tt);
    auto BTr = (parameters.TrLim - Tr) / (parameters.TrLim - parameters.x0.Tr);
    return event_state_type({BA, BTo, BTr, BTt});
}


SystemModel::state_type CentrifugalPumpModel::initialize(const input_type&, const output_type&) const {
    SystemModel::state_type x0 = getStateVector();
    x0[0] = parameters.x0.A;
    x0[1] = parameters.x0.Q;
    x0[2] = parameters.x0.To;
    x0[3] = parameters.x0.Tr;
    x0[4] = parameters.x0.Tt;
    x0[5] = parameters.x0.rRadial;
    x0[6] = parameters.x0.rThrust;
    x0[7] = parameters.x0.w;
    x0[8] = parameters.x0.wA;
    x0[9] = parameters.x0.wRadial;
    x0[10] = parameters.x0.wThrust;
    
    return x0;
}
