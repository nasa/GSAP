// Copyright (c) 2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#include <algorithm>
#include <cmath>
#include <vector>

#include "ConfigMap.h"
#include "Contracts.h"
#include "Models/PneumaticValveModel.h"

using namespace PCOE;

// Configuration Keys
const std::string SAMPLETIME_KEY = "PneumaticValve.SampleTime";
const std::string CYCLETIME_KEY = "PneumaticValve.CycleTime";
const std::string G_KEY = "PneumaticValve.g";
const std::string PATM_KEY = "PneumaticValve.pATM";
const std::string M_KEY = "PneumaticValve.m";
const std::string R_KEY = "PneumaticValve.r";
const std::string K_KEY = "PneumaticValve.k";
const std::string X0_KEY = "PneumaticValve.x0";
const std::string AP_KEY = "PneumaticValve.Ap";
const std::string LS_KEY = "PneumaticValve.Ls";
const std::string VBOT0_KEY = "PneumaticValve.Vbot0";
const std::string VTOP0_KEY = "PneumaticValve.Vtop0";
const std::string INDICATORTOL_KEY = "PneumaticValve.indicatorTol";
const std::string AV_KEY = "PneumaticValve.Av";
const std::string CV_KEY = "PneumaticValve.Cv";
const std::string RHOL_KEY = "PneumaticValve.rhoL";
const std::string GN2_M_KEY = "PneumaticValve.GN2.M";
const std::string GN2_T_KEY = "PneumaticValve.GN2.T";
const std::string GN2_GAMMA_KEY = "PneumaticValve.GN2.gamma";
const std::string GN2_Z_KEY = "PneumaticValve.GN2.Z";
const std::string PSUPPLY_KEY = "PneumaticValve.pSupply";
const std::string AT_KEY = "PneumaticValve.At";
const std::string CT_KEY = "PneumaticValve.Ct";
const std::string AB_KEY = "PneumaticValve.Ab";
const std::string CB_KEY = "PneumaticValve.Cb";
const std::string ABMAX_KEY = "PneumaticValve.AbMax";
const std::string ATMAX_KEY = "PneumaticValve.AtMax";
const std::string AIMAX_KEY = "PneumaticValve.AiMax";
const std::string RMAX_KEY = "PneumaticValve.rMax";
const std::string KMIN_KEY = "PneumaticValve.kMin";

const auto INPUT_ARRAY = {
    MessageId::Pascal, // pL, Fluid pressure at left
    MessageId::Pascal, // pR, Fluid pressure at right
    MessageId::Pascal, // uBot, input pressure at top
    MessageId::Pascal, // uTop, input pressure at bottom
};

const auto OUTPUT_ARRAY = {
    MessageId::MetersCubedPerSecond, // Q, Fluid flow
    MessageId::Boolean, // Indicator Bottom
    MessageId::Boolean, // Indicator Top
    MessageId::Pascal, // Pressure at Bottom
    MessageId::Pascal, // Pressure at Top
    MessageId::Meters, // x
};

const auto EVENT_ARRAY = {
    MessageId::PneumaticValveExternalBottomLeak,
    MessageId::PneumaticValveExternalTopLeak,
    MessageId::PneumaticValveInternalLeak,
    MessageId::PneumaticValveSpringFailure,
    MessageId::PneumaticValveFrictionFailure,
};

PneumaticValveModel::PneumaticValveModel()
    : PrognosticsModel(12,
                       INPUT_ARRAY,
                       OUTPUT_ARRAY,
                       {} /* Observables */,
                       EVENT_ARRAY) {
}

inline double gasFlow(double pIn, double pOut, PneumaticValveModel::Parameters::GN2 gas, double C, double A) {
    // Compute the mass flow through an orifice for both choked and nonchoked flow conditions
    double k = gas.gamma;
    double R = PneumaticValveModel::consts.R;
    double T = gas.T;
    double Z = gas.Z;

    double threshold = pow((k+1)/2,k/(k-1));

    if (pIn/pOut>=threshold) {
        return (C*A*pIn*sqrt(k/(Z*R*T)*pow(2/(k+1),(k+1)/(k-1))));
    }
    else if (pIn/pOut<threshold & pIn>=pOut) {
        return C*A*pIn*sqrt(2.0f/(Z*R*T)*k/(k-1)*abs(pow(pOut/pIn,2/k)-pow(pOut/pIn,(k+1)/k)));
    }
    else if  (pOut/pIn>=threshold) {
        return  -C*A*pOut*sqrt(k/(Z*R*T)*pow(2/(k+1),(k+1)/(k-1)));
    }
    else {
        return -C*A*pOut*sqrt(2.0f/(Z*R*T)*k/(k-1)*abs(pow(pIn/pOut,2/k)-pow(pIn/pOut,(k+1)/k)));
    }
}

PneumaticValveModel::PneumaticValveModel(const ConfigMap& configMap) : PneumaticValveModel::PneumaticValveModel() {
    if (configMap.hasKey(SAMPLETIME_KEY)) {
        params.sampleTime = configMap.getDouble(SAMPLETIME_KEY);
    }
    if (configMap.hasKey(CYCLETIME_KEY)) {
        params.cycleTime = configMap.getDouble(CYCLETIME_KEY);
    }
    if (configMap.hasKey(G_KEY)) {
        params.g = configMap.getDouble(G_KEY);
    }
    if (configMap.hasKey(PATM_KEY)) {
        params.pAtm = configMap.getDouble(PATM_KEY);
    }
    if (configMap.hasKey(M_KEY)) {
        params.m = configMap.getDouble(M_KEY);
    }
    if (configMap.hasKey(R_KEY)) {
        params.r = configMap.getDouble(R_KEY);
    }
    if (configMap.hasKey(K_KEY)) {
        params.k = configMap.getDouble(K_KEY);
    }
    if (configMap.hasKey(X0_KEY)) {
        params.x0 = configMap.getDouble(X0_KEY);
    }
    if (configMap.hasKey(AP_KEY)) {
        params.Ap = configMap.getDouble(AP_KEY);
    }
    if (configMap.hasKey(LS_KEY)) {
        params.Ls = configMap.getDouble(LS_KEY);
    }
    if (configMap.hasKey(VBOT0_KEY)) {
        params.Vbot0 = configMap.getDouble(VBOT0_KEY);
    }
    if (configMap.hasKey(VTOP0_KEY)) {
        params.Vtop0 = configMap.getDouble(VTOP0_KEY);
    }
    if (configMap.hasKey(INDICATORTOL_KEY)) {
        params.indicatorTol = configMap.getDouble(INDICATORTOL_KEY);
    }
    if (configMap.hasKey(AV_KEY)) {
        params.Av = configMap.getDouble(AV_KEY);
    }
    if (configMap.hasKey(CV_KEY)) {
        params.Cv = configMap.getDouble(CV_KEY);
    }
    if (configMap.hasKey(RHOL_KEY)) {
        params.rhoL = configMap.getDouble(RHOL_KEY);
    }
    if (configMap.hasKey(GN2_M_KEY)) {
        params.gn2.M = configMap.getDouble(GN2_M_KEY);
    }
    if (configMap.hasKey(GN2_T_KEY)) {
        params.gn2.T = configMap.getDouble(GN2_T_KEY);
    }
    if (configMap.hasKey(GN2_GAMMA_KEY)) {
        params.gn2.gamma = configMap.getDouble(GN2_GAMMA_KEY);
    }
    if (configMap.hasKey(GN2_Z_KEY)) {
        params.gn2.Z = configMap.getDouble(GN2_Z_KEY);
    }
    if (configMap.hasKey(PSUPPLY_KEY)) {
        params.pSupply = configMap.getDouble(PSUPPLY_KEY);
    }
    if (configMap.hasKey(AT_KEY)) {
        params.At = configMap.getDouble(AT_KEY);
    }
    if (configMap.hasKey(CT_KEY)) {
        params.Ct = configMap.getDouble(CT_KEY);
    }
    if (configMap.hasKey(AB_KEY)) {
        params.Ab = configMap.getDouble(AB_KEY);
    }
    if (configMap.hasKey(CB_KEY)) {
        params.Cb = configMap.getDouble(CB_KEY);
    }
    if (configMap.hasKey(ABMAX_KEY)) {
        params.AbMax = configMap.getDouble(ABMAX_KEY);
    }
    if (configMap.hasKey(ATMAX_KEY)) {
        params.AtMax = configMap.getDouble(ATMAX_KEY);
    }
    if (configMap.hasKey(AIMAX_KEY)) {
        params.AiMax = configMap.getDouble(AIMAX_KEY);
    }
    if (configMap.hasKey(RMAX_KEY)) {
        params.rMax = configMap.getDouble(RMAX_KEY);
    }
    if (configMap.hasKey(KMIN_KEY)) {
        params.kMin = configMap.getDouble(KMIN_KEY);
    }
}

SystemModel::state_type PneumaticValveModel::stateEqn(double, const state_type& x_in, const input_type& u, double dt) const {
    
    // Extract states
    double Aeb = x_in[0];
    double Aet = x_in[1];
    double Ai = x_in[2];
    double k = x_in[4];
    double mBot = x_in[5];
    double mTop = x_in[6];
    double r = x_in[7];
    double v = x_in[8];
    double wb = x_in[9];
    double wi = x_in[10];
    double wk = x_in[11];
    double wr = x_in[12];
    double wt = x_in[13];
    double x = x_in[14];
    
    // Extract inputs
    double pL = u[0];
    double pR = u[1];
    double uBot = u[2];
    double uTop = u[3];
    
    // Constraints
    double wrdot = 0;
    double Aebdot = wb;
    double wkdot = 0;
    double Aetdot = wt;
    double wbdot = 0;
    double wtdot = 0;
    double xdot = v;
    double widot = 0;
    double pInTop = (uTop > -1e-6 && uTop < 1e-6)? params.pAtm : params.pSupply;
    double springForce = k*(params.x0+x);
    double friction = v*r;
    double fluidForce = (pL-pR)*params.Av;
    double pInBot = (uBot > -1e-6 && uBot < 1e-6)? params.pAtm : params.pSupply;
    double volumeBot = params.Vbot0 + params.Ap*x;
    double volumeTop = params.Vtop0 + params.Ap*(params.Ls-x);
    double plugWeight = params.m*params.g;
    double kdot = -wk*abs(v*springForce);
    double rdot = wr*abs(v*friction);
    double Aidot = wi*abs(v*friction);
    double pressureBot = mBot*consts.R*params.gn2.T/params.gn2.M/volumeBot;
    double mBotDotn = gasFlow(pInBot,pressureBot,params.gn2,params.Cb,params.Ab);
    double pressureTop = mTop*consts.R*params.gn2.T/params.gn2.M/volumeTop;
    double leakBotToAtm = gasFlow(pressureBot,params.pAtm,params.gn2,1,Aeb);
    double gasForceTop = pressureTop*params.Ap;
    double gasForceBot = pressureBot*params.Ap;
    double leakTopToAtm = gasFlow(pressureTop,params.pAtm,params.gn2,1,Aet);
    double leakTopToBot = gasFlow(pressureTop,pressureBot,params.gn2,1,Ai);
    double mBotdot = mBotDotn + leakTopToBot - leakBotToAtm;
    double mTopDotn = gasFlow(pInTop,pressureTop,params.gn2,params.Ct,params.At);
    double pistonForces = -fluidForce - plugWeight - friction - springForce + gasForceBot - gasForceTop;
    double mTopdot = mTopDotn - leakTopToBot - leakTopToAtm;
    double vdot = pistonForces/params.m;
    
    // Compute new x, v based on condition
    if ((x<=0 & pistonForces<0) || (x+xdot*dt<0)) {
        // Pushed botton/closed
        x = 0;
        v = 0;
    } else if ((x>=params.Ls & pistonForces>0) || (x+xdot*dt>params.Ls)) {
        // pushed top, open
        x = params.Ls;
        v = 0;
    } else {
        // moving
        x = x+xdot*dt;
        v = v+vdot*dt;
    }
    
    // Update state
    auto x_new = getStateVector();
    x_new[0] = Aeb + Aebdot*dt;
    x_new[1] = Aet + Aetdot*dt;
    x_new[2] = Ai + Aidot*dt;
    x_new[3] = pL-pR;
    x_new[4] = k + kdot*dt;
    x_new[5] = mBot + mBotdot*dt;
    x_new[6] = mTop + mTopdot*dt;
    x_new[7] = r + rdot*dt;
    x_new[8] = x;
    x_new[9] = wb + wbdot*dt;
    x_new[10] = wi + widot*dt;
    x_new[11] = wk + wkdot*dt;
    x_new[12] = wr + wrdot*dt;
    x_new[13] = wt + wtdot*dt;
    x_new[14] = x;
    
    return x_new;
}

SystemModel::output_type PneumaticValveModel::outputEqn(double,
                                                 const state_type& x_in) const {
    // Extract states
    double pLmpR = x_in[3];
    double mBot = x_in[5];
    double mTop = x_in[6];
    double x = x_in[14];
    
    // Constraints
    bool indicatorTopm = x >= params.Ls-params.indicatorTol;
    bool indicatorBotm = x <= params.indicatorTol;
    double maxFlow = params.Cv*params.Av*sqrt(2.0f/params.rhoL*abs(pLmpR))*-signbit(pLmpR);
    double volumeBot = params.Vbot0 + params.Ap*x;
    double volumeTop = params.Vtop0 + params.Ap*(params.Ls-x);
    double trueFlow = maxFlow* fmax(0,x)/params.Ls;
    double pressureTop = mTop*consts.R*params.gn2.T/params.gn2.M/volumeTop;
    double pressureBot = mBot*consts.R*params.gn2.T/params.gn2.M/volumeBot;
    
    // set outputs
    auto z_new = getOutputVector();
    z_new[0] = trueFlow;
    z_new[1] = indicatorBotm;
    z_new[2] = indicatorTopm;
    z_new[3] = pressureBot;
    z_new[4] = pressureTop;
    z_new[5] = x;
    
    return z_new;
}

SystemModel::event_state_type PneumaticValveModel::eventStateEqn(const state_type& x_in) const {
    
    // Extract states
    double Aeb = x_in[0];
    double Aet = x_in[1];
    double Ai = x_in[2];
    double k = x_in[4];
    double r = x_in[7];
    
    // Individual performance constraints
    auto BAeb = (params.AbMax - Aeb) / (params.AbMax - params.Ab);
    auto BAet = (params.AtMax - Aet) / (params.AtMax - params.At);
    auto BAi = (params.AiMax - Ai) / (params.AbMax);
    auto Bk = (k - params.kMin) / (params.k - params.kMin);
    auto Br = (params.rMax - r) / (params.rMax - params.r);
    return event_state_type({BAeb, BAet, BAi, Bk, Br});
}


SystemModel::state_type PneumaticValveModel::initialize(const input_type& u, const output_type&) const {
    // Extract inputs
    double pL = u[0];
    double pR = u[1];
    
    // Initialize state
    double MoRT = params.gn2.M / (consts.R * params.gn2.T);
    double Vtopi = params.Vtop0 + params.Ap * params.Ls;
    return state_type({
        params.Ab, // Aeb, Area of External Leak- bottom
        params.At, // Aet, Area of External Leak- top
        0, // Ai, Area of Internal Leak
        pL - pR, // pLmpR difference between pL and pR (Pa)
        params.k, // k, spring constant
        params.pAtm * params.Vbot0 * MoRT, // mBot, mass of gas below piston
        params.pSupply * Vtopi * MoRT, // mTop, mass of gas above piston
        params.r, // r, friction coefficient
        0, // v, valve velocition (m/s)
        0, // wb, flowrate at bottom
        0, // wi
        0, // wk
        0, // wr
        0, // wt, flowrate at top
        0, // x, valve position (m)
    });
}
