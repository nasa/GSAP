// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Models/BatteryModel.h"

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
const std::string QMOBILE_KEY = "Battery.qMobile";
const std::string VOL_KEY = "Battery.Vol";
const std::string AN0_KEY = "Battery.An0";
const std::string AN1_KEY = "Battery.An1";
const std::string AN10_KEY = "Battery.An10";
const std::string AN11_KEY = "Battery.An11";
const std::string AN12_KEY = "Battery.An12";
const std::string AN2_KEY = "Battery.An2";
const std::string AN3_KEY = "Battery.An3";
const std::string AN4_KEY = "Battery.An4";
const std::string AN5_KEY = "Battery.An5";
const std::string AN6_KEY = "Battery.An6";
const std::string AN7_KEY = "Battery.An7";
const std::string AN8_KEY = "Battery.An8";
const std::string AN9_KEY = "Battery.An9";
const std::string AP0_KEY = "Battery.Ap0";
const std::string AP1_KEY = "Battery.Ap1";
const std::string AP10_KEY = "Battery.Ap10";
const std::string AP11_KEY = "Battery.Ap11";
const std::string AP12_KEY = "Battery.Ap12";
const std::string AP2_KEY = "Battery.Ap2";
const std::string AP3_KEY = "Battery.Ap3";
const std::string AP4_KEY = "Battery.Ap4";
const std::string AP5_KEY = "Battery.Ap5";
const std::string AP6_KEY = "Battery.Ap6";
const std::string AP7_KEY = "Battery.Ap7";
const std::string AP8_KEY = "Battery.Ap8";
const std::string AP9_KEY = "Battery.Ap9";
const std::string RO_KEY = "Battery.Ro";
const std::string SN_KEY = "Battery.Sn";
const std::string SP_KEY = "Battery.Sp";
const std::string U0N_KEY = "Battery.U0n";
const std::string U0P_KEY = "Battery.U0p";
const std::string VEOD_KEY = "Battery.VEOD";
const std::string VOLSFRACTION_KEY = "Battery.VolSFraction";
const std::string KN_KEY = "Battery.kn";
const std::string KP_KEY = "Battery.kp";
const std::string TDIFFUSION_KEY = "Battery.tDiffusion";
const std::string TO_KEY = "Battery.to";
const std::string TSN_KEY = "Battery.tsn";
const std::string TSP_KEY = "Battery.tsp";
const std::string XNMAX_KEY = "Battery.xnMax";
const std::string XNMIN_KEY = "Battery.xnMin";
const std::string XPMAX_KEY = "Battery.xpMax";
const std::string XPMIN_KEY = "Battery.xpMin";

BatteryModel::BatteryModel()
    : PrognosticsModel(8,
                       {MessageId::Watts},
                       {MessageId::Volts, MessageId::Centigrade},
                       {},
                       {MessageId::BatteryEod}) {
    // Set some default parameters
    setParameters();
}

// Constructor based on configMap
BatteryModel::BatteryModel(const ConfigMap& configMap) : BatteryModel::BatteryModel() {
    // Check for arguments to setParameters first, qMobile and Vol
    // There are default values available, so call to setParameters is different depending
    // on which are given in the configMap.

    if (configMap.hasKey(QMOBILE_KEY) && configMap.hasKey(VOL_KEY)) {
        setParameters(configMap.getDouble(QMOBILE_KEY), configMap.getDouble(VOL_KEY));
    }
    else if (configMap.hasKey(QMOBILE_KEY) && !configMap.hasKey(VOL_KEY)) {
        setParameters(configMap.getDouble(QMOBILE_KEY));
    }
    else if (!configMap.hasKey(QMOBILE_KEY) && configMap.hasKey(VOL_KEY)) {
        setParameters(QMOBILE_DEFAULT_VALUE, configMap.getDouble(VOL_KEY));
    }
    // For the remaining parameters, we set them after the call to setParameters,
    // because no other parameter values are derived from them.
    if (configMap.hasKey(AN0_KEY)) {
        parameters.An0 = configMap.getDouble(AN0_KEY);
    }
    if (configMap.hasKey(AN1_KEY)) {
        parameters.An1 = configMap.getDouble(AN1_KEY);
    }
    if (configMap.hasKey(AN10_KEY)) {
        parameters.An10 = configMap.getDouble(AN10_KEY);
    }
    if (configMap.hasKey(AN11_KEY)) {
        parameters.An11 = configMap.getDouble(AN11_KEY);
    }
    if (configMap.hasKey(AN12_KEY)) {
        parameters.An12 = configMap.getDouble(AN12_KEY);
    }
    if (configMap.hasKey(AN2_KEY)) {
        parameters.An2 = configMap.getDouble(AN2_KEY);
    }
    if (configMap.hasKey(AN3_KEY)) {
        parameters.An3 = configMap.getDouble(AN3_KEY);
    }
    if (configMap.hasKey(AN4_KEY)) {
        parameters.An4 = configMap.getDouble(AN4_KEY);
    }
    if (configMap.hasKey(AN5_KEY)) {
        parameters.An5 = configMap.getDouble(AN5_KEY);
    }
    if (configMap.hasKey(AN6_KEY)) {
        parameters.An6 = configMap.getDouble(AN6_KEY);
    }
    if (configMap.hasKey(AN7_KEY)) {
        parameters.An7 = configMap.getDouble(AN7_KEY);
    }
    if (configMap.hasKey(AN8_KEY)) {
        parameters.An8 = configMap.getDouble(AN8_KEY);
    }
    if (configMap.hasKey(AN9_KEY)) {
        parameters.An9 = configMap.getDouble(AN9_KEY);
    }
    if (configMap.hasKey(AP0_KEY)) {
        parameters.Ap0 = configMap.getDouble(AP0_KEY);
    }
    if (configMap.hasKey(AP1_KEY)) {
        parameters.Ap1 = configMap.getDouble(AP1_KEY);
    }
    if (configMap.hasKey(AP10_KEY)) {
        parameters.Ap10 = configMap.getDouble(AP10_KEY);
    }
    if (configMap.hasKey(AP11_KEY)) {
        parameters.Ap11 = configMap.getDouble(AP11_KEY);
    }
    if (configMap.hasKey(AP12_KEY)) {
        parameters.Ap12 = configMap.getDouble(AP12_KEY);
    }
    if (configMap.hasKey(AP2_KEY)) {
        parameters.Ap2 = configMap.getDouble(AP2_KEY);
    }
    if (configMap.hasKey(AP3_KEY)) {
        parameters.Ap3 = configMap.getDouble(AP3_KEY);
    }
    if (configMap.hasKey(AP4_KEY)) {
        parameters.Ap4 = configMap.getDouble(AP4_KEY);
    }
    if (configMap.hasKey(AP5_KEY)) {
        parameters.Ap5 = configMap.getDouble(AP5_KEY);
    }
    if (configMap.hasKey(AP6_KEY)) {
        parameters.Ap6 = configMap.getDouble(AP6_KEY);
    }
    if (configMap.hasKey(AP7_KEY)) {
        parameters.Ap7 = configMap.getDouble(AP7_KEY);
    }
    if (configMap.hasKey(AP8_KEY)) {
        parameters.Ap8 = configMap.getDouble(AP8_KEY);
    }
    if (configMap.hasKey(AP9_KEY)) {
        parameters.Ap9 = configMap.getDouble(AP9_KEY);
    }
    if (configMap.hasKey(RO_KEY)) {
        parameters.Ro = configMap.getDouble(RO_KEY);
    }
    if (configMap.hasKey(SN_KEY)) {
        parameters.Sn = configMap.getDouble(SN_KEY);
    }
    if (configMap.hasKey(SP_KEY)) {
        parameters.Sp = configMap.getDouble(SP_KEY);
    }
    if (configMap.hasKey(U0N_KEY)) {
        parameters.U0n = configMap.getDouble(U0N_KEY);
    }
    if (configMap.hasKey(U0P_KEY)) {
        parameters.U0p = configMap.getDouble(U0P_KEY);
    }
    if (configMap.hasKey(VEOD_KEY)) {
        parameters.VEOD = configMap.getDouble(VEOD_KEY);
    }
    if (configMap.hasKey(VOLSFRACTION_KEY)) {
        parameters.VolSFraction = configMap.getDouble(VOLSFRACTION_KEY);
    }
    if (configMap.hasKey(KN_KEY)) {
        parameters.kn = configMap.getDouble(KN_KEY);
    }
    if (configMap.hasKey(KP_KEY)) {
        parameters.kp = configMap.getDouble(KP_KEY);
    }
    if (configMap.hasKey(TDIFFUSION_KEY)) {
        parameters.tDiffusion = configMap.getDouble(TDIFFUSION_KEY);
    }
    if (configMap.hasKey(TO_KEY)) {
        parameters.to = configMap.getDouble(TO_KEY);
    }
    if (configMap.hasKey(TSN_KEY)) {
        parameters.tsn = configMap.getDouble(TSN_KEY);
    }
    if (configMap.hasKey(TSP_KEY)) {
        parameters.tsp = configMap.getDouble(TSP_KEY);
    }
    if (configMap.hasKey(XNMAX_KEY)) {
        parameters.xnMax = configMap.getDouble(XNMAX_KEY);
    }
    if (configMap.hasKey(XNMIN_KEY)) {
        parameters.xnMin = configMap.getDouble(XNMIN_KEY);
    }
    if (configMap.hasKey(XPMAX_KEY)) {
        parameters.xpMax = configMap.getDouble(XPMAX_KEY);
    }
    if (configMap.hasKey(XPMIN_KEY)) {
        parameters.xpMin = configMap.getDouble(XPMIN_KEY);
    }
}

// Battery State Equation
SystemModel::state_type BatteryModel::stateEqn(double,
                                               const state_type& x,
                                               const input_type& u,
                                               const noise_type& n,
                                               double dt) const {
    // Extract states
    double Tb = x[0];
    double Vo = x[1];
    double Vsn = x[2];
    double Vsp = x[3];
    double qnB = x[4];
    double qnS = x[5];
    double qpB = x[6];
    double qpS = x[7];

    // Extract inputs
    double P = u[0];

    // Constraints
    double Tbdot = 0;
    double CpBulk = qpB / parameters.VolB;
    double CpSurface = qpS / parameters.VolS;
    double CnSurface = qnS / parameters.VolS;
    double xSn = qnS / parameters.qSMax;
    double xnS = qnS / parameters.qSMax;
    auto xnS2 = xnS * xnS;
    auto xnS2_1 = 2 * xnS - 1;
    auto xnS2_xnS = xnS2 - xnS;
    double Ven11 =
        parameters.An11 * (22 * xnS2_1 * pow(xnS2_1, 10) + pow(xnS2_1, 12)) / parameters.F;
    double Ven1 = parameters.An1 * (2 * xnS2_1 + pow(xnS2_1, 2)) / parameters.F;
    double CnBulk = qnB / parameters.VolB;
    double Ven6 = parameters.An6 * (12 * xnS2_1 * pow(xnS2_1, 5) + pow(xnS2_1, 7)) / parameters.F;
    double xpS = qpS / parameters.qSMax;
    auto xpS2 = xpS * xpS;
    auto xpS2_1 = 2 * xpS - 1;
    auto xpS2_xpS = xpS2 - xpS;
    double xSp = qpS / parameters.qSMax;
    double qdotDiffusionBSp = (CpBulk - CpSurface) / parameters.tDiffusion;
    double Ven8 = parameters.An8 * (16 * xnS2_xnS * pow(xnS2_1, 7) + pow(xnS2_1, 9)) / parameters.F;
    double Ven7 = parameters.An7 * (14 * xnS2_xnS * pow(xnS2_1, 6) + pow(xnS2_1, 8)) / parameters.F;
    double Ven9 = parameters.An9 * (18 * xnS2_1 * pow(xnS2_1, 8) + pow(xnS2_1, 10)) / parameters.F;
    double Ven4 = parameters.An4 * (8 * xnS2_1 * pow(xnS2_1, 3) + pow(xnS2_1, 5)) / parameters.F;
    double Ven3 = parameters.An3 * (6 * xnS2_1 * pow(xnS2_1, 2) + pow(xnS2_1, 4)) / parameters.F;
    double Ven2 = parameters.An2 * (4 * xnS2_1 * (xnS2_1) + pow(xnS2_1, 3)) / parameters.F;
    double qdotDiffusionBSn = (CnBulk - CnSurface) / parameters.tDiffusion;
    double Ven5 = parameters.An5 * (10 * xnS2_1 * pow(xnS2_1, 4) + pow(xnS2_1, 6)) / parameters.F;
    double Vep4 = parameters.Ap4 * (8 * xpS2_xpS * pow(xpS2_1, 3) + pow(xpS2_1, 5)) / parameters.F;
    double Vep6 = parameters.Ap6 * (12 * xpS2_xpS * pow(xpS2_1, 5) + pow(xpS2_1, 7)) / parameters.F;
    double Vep0 = parameters.Ap0 * xpS2_1 / parameters.F;
    double Vep3 = parameters.Ap3 * (6 * xpS2_xpS * pow(xpS2_1, 2) + pow(xpS2_1, 4)) / parameters.F;
    double Vep10 =
        parameters.Ap10 * (20 * xpS2_xpS * pow(xpS2_1, 9) + pow(xpS2_1, 11)) / parameters.F;
    double Vep12 =
        parameters.Ap12 * (24 * xpS2_xpS * pow(xpS2_1, 11) + pow(xpS2_1, 13)) / parameters.F;
    double Jn0 = parameters.kn * pow(xSn, parameters.alpha) * pow(-xSn + 1, parameters.alpha);
    double Vep7 = parameters.Ap7 * (14 * xpS2_xpS * pow(xpS2_1, 6) + pow(xpS2_1, 8)) / parameters.F;
    double Vep2 = parameters.Ap2 * (4 * xpS2_xpS * (xpS2_1) + pow(xpS2_1, 3)) / parameters.F;
    double Vep11 =
        parameters.Ap11 * (22 * xpS2_xpS * pow(xpS2_1, 10) + pow(xpS2_1, 12)) / parameters.F;
    double Ven0 = parameters.An0 * xnS2_1 / parameters.F;
    double Ven12 =
        parameters.An12 * (24 * xnS2_1 * pow(xnS2_1, 11) + pow(xnS2_1, 13)) / parameters.F;
    double Ven10 =
        parameters.An10 * (20 * xnS2_1 * pow(xnS2_1, 9) + pow(xnS2_1, 11)) / parameters.F;
    double Vep9 =
        parameters.Ap9 * (18 * xpS2_xpS * pow(xpS2_1, 8) + pow(xpS2_1, 10)) / parameters.F;
    double Vep5 = parameters.Ap5 * (10 * xpS2_xpS * pow(xpS2_1, 4) + pow(xpS2_1, 6)) / parameters.F;
    double Vep8 = parameters.Ap8 * (16 * xpS2_xpS * pow(xpS2_1, 7) + pow(xpS2_1, 9)) / parameters.F;
    double Vep1 = parameters.Ap1 * (2 * xpS2_xpS + pow(xpS2_1, 2)) / parameters.F;
    double Jp0 = parameters.kp * pow(xSp, parameters.alpha) * pow(-xSp + 1, parameters.alpha);
    double Ven = parameters.U0n + Ven0 + Ven1 + Ven10 + Ven11 + Ven12 + Ven2 + Ven3 + Ven4 + Ven5 +
                 Ven6 + Ven7 + Ven8 + Ven9 +
                 parameters.R * Tb * log((-xnS + 1) / xnS) / parameters.F;
    double Vep = parameters.U0p + Vep0 + Vep1 + Vep10 + Vep11 + Vep12 + Vep2 + Vep3 + Vep4 + Vep5 +
                 Vep6 + Vep7 + Vep8 + Vep9 +
                 parameters.R * Tb * log((-xpS + 1) / xpS) / parameters.F;
    double V = Vep - Vo - Vsn - Vsp - Ven;
    double i = P / V;
    double qnSdot = qdotDiffusionBSn - i;
    double Jn = i / parameters.Sn;
    double VoNominal = parameters.Ro * i;
    double Jp = i / parameters.Sp;
    double qnBdot = -qdotDiffusionBSn;
    double qpBdot = -qdotDiffusionBSp;
    double qpSdot = i + qdotDiffusionBSp;
    double Vodot = (VoNominal - Vo) / parameters.to;
    double VsnNominal = static_cast<double>(parameters.R * Tb * asinh((1.0L / 2.0L) * Jn / Jn0) /
                                            (parameters.F * parameters.alpha));
    double VspNominal = static_cast<double>(parameters.R * Tb * asinh((1.0L / 2.0L) * Jp / Jp0) /
                                            (parameters.F * parameters.alpha));
    double Vsndot = (VsnNominal - Vsn) / parameters.tsn;
    double Vspdot = (VspNominal - Vsp) / parameters.tsp;

    // Update state
    auto x_new = getStateVector();
    x_new[0] = Tb + Tbdot * dt;
    x_new[1] = Vo + Vodot * dt;
    x_new[2] = Vsn + Vsndot * dt;
    x_new[3] = Vsp + Vspdot * dt;
    x_new[4] = qnB + qnBdot * dt;
    x_new[5] = qnS + qnSdot * dt;
    x_new[6] = qpB + qpBdot * dt;
    x_new[7] = qpS + qpSdot * dt;

    // Add process noise
    for (size_type it = 0; it <= 7; it++) {
        x_new[it] += dt * n[it];
    }
    return x_new;
}

// Battery Output Equation
SystemModel::output_type BatteryModel::outputEqn(double,
                                                 const state_type& x,
                                                 const noise_type& n) const {
    // Extract states
    const double& Tb = x[0];
    const double& Vo = x[1];
    const double& Vsn = x[2];
    const double& Vsp = x[3];
    // double qnB = x[4];
    const double& qnS = x[5];
    // double qpB = x[6];
    const double& qpS = x[7];

    // Constraints
    double xnS = qnS / parameters.qSMax;
    auto xnS2 = xnS * xnS;
    auto xnS2_1 = 2 * xnS - 1;
    auto xnS2_xnS = xnS2 - xnS;
    double Ven11 =
        parameters.An11 * (22 * xnS2_xnS * pow(xnS2_1, 10) + pow(xnS2_1, 12)) / parameters.F;
    double xpS = qpS / parameters.qSMax;
    auto xpS2 = xpS * xpS;
    auto xpS2_1 = 2 * xpS - 1;
    auto xpS2_xpS = xpS2 - xpS;

    double Vep6 = parameters.Ap6 * (12 * xpS2_xpS * pow(xpS2_1, 5) + pow(xpS2_1, 7)) / parameters.F;
    double Ven5 = parameters.An5 * (10 * xnS2_xnS * pow(xnS2_1, 4) + pow(xnS2_1, 6)) / parameters.F;
    double Ven7 = parameters.An7 * (14 * xnS2_xnS * pow(xnS2_1, 6) + pow(xnS2_1, 8)) / parameters.F;
    double Vep4 = parameters.Ap4 * (8 * xpS2_xpS * pow(xpS2_1, 3) + pow(xpS2_1, 5)) / parameters.F;
    double Ven9 =
        parameters.An9 * (18 * xnS2_xnS * pow(xnS2_1, 8) + pow(xnS2_1, 10)) / parameters.F;
    double Ven3 = parameters.An3 * (6 * xnS2_xnS * pow(xnS2_1, 2) + pow(xnS2_1, 4)) / parameters.F;
    double Vep7 = parameters.Ap7 * (14 * xpS2_xpS * pow(xpS2_1, 6) + pow(xpS2_1, 8)) / parameters.F;
    double Vep11 =
        parameters.Ap11 * (22 * xpS2_xpS * pow(xpS2_1, 10) + pow(xpS2_1, 12)) / parameters.F;
    double Ven12 =
        parameters.An12 * (24 * xnS2_xnS * pow(xnS2_1, 11) + pow(xnS2_1, 13)) / parameters.F;
    double Vep9 =
        parameters.Ap9 * (18 * xpS2_xpS * pow(xpS2_1, 8) + pow(xpS2_1, 10)) / parameters.F;
    double Vep5 = parameters.Ap5 * (10 * xpS2_xpS * pow(xpS2_1, 4) + pow(xpS2_1, 6)) / parameters.F;
    double Ven1 = parameters.An1 * (6 * xnS2_xnS + 1) /
                  parameters.F; // Was parameters.An1*(-2 * xnS*(-xnS + 1) + pow(xnS2_1, 2))
    double Vep8 = parameters.Ap8 * (16 * xpS2_xpS * pow(xpS2_1, 7) + pow(xpS2_1, 9)) / parameters.F;
    double Ven6 = parameters.An6 * (12 * xnS2_xnS * pow(xnS2_1, 5) + pow(xnS2_1, 7)) / parameters.F;
    double Ven8 = parameters.An8 * (16 * xnS2_xnS * pow(xnS2_1, 7) + pow(xnS2_1, 9)) / parameters.F;
    double Vep3 = parameters.Ap3 * (6 * xpS2_xpS * pow(xpS2_1, 2) + pow(xpS2_1, 4)) / parameters.F;
    double Vep10 =
        parameters.Ap10 * (20 * xpS2_xpS * pow(xpS2_1, 9) + pow(xpS2_1, 11)) / parameters.F;
    double Vep12 =
        parameters.Ap12 * (24 * xpS2_xpS * pow(xpS2_1, 11) + pow(xpS2_1, 13)) / parameters.F;
    double Ven4 = parameters.An4 * (8 * xnS2_xnS * pow(xnS2_1, 3) + pow(xnS2_1, 5)) / parameters.F;
    double Ven2 = parameters.An2 * (4 * xnS2_xnS * xnS2_1 + pow(xnS2_1, 3)) / parameters.F;
    double Vep2 = parameters.Ap2 * (4 * xpS2_xpS * xpS2_1 + pow(xpS2_1, 3)) / parameters.F;
    double Ven0 = parameters.An0 * (xnS2_1) / parameters.F;
    double Ven10 =
        parameters.An10 * (20 * xnS2_xnS * pow(xnS2_1, 9) + pow(xnS2_1, 11)) / parameters.F;
    double Vep1 = parameters.Ap1 * (6 * xpS2_xpS + 1) /
                  parameters.F; // was (-2 * xpS*(-xpS + 1) + pow(xpS2_1, 2)
    double Vep0 = parameters.Ap0 * (xpS2_1) / parameters.F;
    double Ven = parameters.U0n + Ven0 + Ven1 + Ven10 + Ven11 + Ven12 + Ven2 + Ven3 + Ven4 + Ven5 +
                 Ven6 + Ven7 + Ven8 + Ven9 +
                 parameters.R * Tb * log((-xnS + 1) / xnS) / parameters.F;
    double Vep = parameters.U0p + Vep0 + Vep1 + Vep10 + Vep11 + Vep12 + Vep2 + Vep3 + Vep4 + Vep5 +
                 Vep6 + Vep7 + Vep8 + Vep9 +
                 parameters.R * Tb * log((-xpS + 1) / xpS) / parameters.F;

    auto z_new = getOutputVector();
    // Set outputs
    z_new[OUT::TEMP] = Tb - 273.15;
    z_new[OUT::VOLTS] = -Ven + Vep - Vo - Vsn - Vsp;

    // Add noise
    z_new[OUT::TEMP] += n[OUT::TEMP];
    z_new[OUT::VOLTS] += n[OUT::VOLTS];
    return z_new;
}

// Battery Threshold Equation
bool BatteryModel::thresholdEqn(double t, const state_type& x) const {
    // Compute based on voltage, so use output equation to get voltage
    auto z = outputEqn(t, x, std::vector<double>(2));

    // Determine if voltage (second element in z) is below VEOD threshold
    return z[1] <= parameters.VEOD;
}

SystemModel::event_state_type BatteryModel::eventStateEqn(const state_type& x) const {
    // Compute "nominal" SOC
    double qnS = x[indices.states.qnS];
    double qnB = x[indices.states.qnB];
    return event_state_type({(qnS + qnB) / parameters.qnMax});
}

// Set model parameters, given qMobile
void BatteryModel::setParameters(const double qMobile, const double Vol) {
    // Set qMobile
    parameters.qMobile = qMobile;

    // Set min/max mole fraction and charges
    parameters.xnMax = 0.6; // maximum mole fraction (neg electrode)
    parameters.xnMin = 0; // minimum mole fraction (neg electrode)
    parameters.xpMax = 1.0; // maximum mole fraction (pos electrode)
    parameters.xpMin = 0.4; // minimum mole fraction (pos electrode) -> note xn+xp=1
    parameters.qMax =
        parameters.qMobile / (parameters.xnMax - parameters.xnMin); // note qMax = qn+qp
    parameters.Ro = 0.117215; // for ohmic drop (current collector resistances plus electrolyte
                              // resistance plus solid phase resistances at anode and cathode)

    // constants of nature
    parameters.R = 8.3144621; // universal gas constant, J/K/mol
    parameters.F = 96487; // Faraday's constant, C/mol

    // Li-ion parameters
    parameters.alpha = 0.5; // anodic/cathodic electrochemical transfer coefficient
    parameters.Sn = 0.000437545; // surface area (- electrode)
    parameters.Sp = 0.00030962; // surface area (+ electrode)
    parameters.kn = 2120.96; // lumped constant for BV (- electrode)
    parameters.kp = 248898; // lumped constant for BV (+ electrode)
    parameters.Vol = Vol; // total interior battery volume/2 (for computing concentrations)
    parameters.VolSFraction = 0.1; // fraction of total volume occupied by surface volume

    // Volumes (total volume is 2*parameters.Vol), assume volume at each electrode is the
    // same and the surface/bulk split is the same for both electrodes
    parameters.VolS = parameters.VolSFraction * parameters.Vol; // surface volume
    parameters.VolB = parameters.Vol - parameters.VolS; // bulk volume

    // Set up charges (Li ions)
    parameters.qpMin = parameters.qMax * parameters.xpMin; // min charge at pos electrode
    parameters.qpMax = parameters.qMax * parameters.xpMax; // max charge at pos electrode
    parameters.qpSMin =
        parameters.qpMin * parameters.VolS / parameters.Vol; // min charge at surface, pos electrode
    parameters.qpBMin =
        parameters.qpMin * parameters.VolB / parameters.Vol; // min charge at bulk, pos electrode
    parameters.qpSMax =
        parameters.qpMax * parameters.VolS / parameters.Vol; // max charge at surface, pos electrode
    parameters.qpBMax =
        parameters.qpMax * parameters.VolB / parameters.Vol; // max charge at bulk, pos electrode
    parameters.qnMin = parameters.qMax * parameters.xnMin; // max charge at neg electrode
    parameters.qnMax = parameters.qMax * parameters.xnMax; // max charge at neg electrode
    parameters.qnSMax =
        parameters.qnMax * parameters.VolS / parameters.Vol; // max charge at surface, neg electrode
    parameters.qnBMax =
        parameters.qnMax * parameters.VolB / parameters.Vol; // max charge at bulk, neg electrode
    parameters.qnSMin =
        parameters.qnMin * parameters.VolS / parameters.Vol; // min charge at surface, neg electrode
    parameters.qnBMin =
        parameters.qnMin * parameters.VolB / parameters.Vol; // min charge at bulk, neg electrode
    parameters.qSMax =
        parameters.qMax * parameters.VolS / parameters.Vol; // max charge at surface (pos and neg)
    parameters.qBMax =
        parameters.qMax * parameters.VolB / parameters.Vol; // max charge at bulk (pos and neg)

    // time constants
    parameters.tDiffusion =
        7e6; // diffusion time constant (increasing this causes decrease in diffusion rate)
    parameters.to = 6.08671;
    parameters.tsn = 1.00138e3;
    parameters.tsp = 46.4311;

    // Redlich-Kister parameters (positive electrode)
    parameters.U0p = 4.03;
    parameters.Ap0 = -31593.7;
    parameters.Ap1 = 0.106747;
    parameters.Ap2 = 24606.4;
    parameters.Ap3 = -78561.9;
    parameters.Ap4 = 13317.9;
    parameters.Ap5 = 307387;
    parameters.Ap6 = 84916.1;
    parameters.Ap7 = -1.07469e+06;
    parameters.Ap8 = 2285.04;
    parameters.Ap9 = 990894;
    parameters.Ap10 = 283920;
    parameters.Ap11 = -161513;
    parameters.Ap12 = -469218;

    // Redlich-Kister parameters (negative electrode)
    parameters.U0n = 0.01;
    parameters.An0 = 86.19;
    parameters.An1 = 0;
    parameters.An2 = 0;
    parameters.An3 = 0;
    parameters.An4 = 0;
    parameters.An5 = 0;
    parameters.An6 = 0;
    parameters.An7 = 0;
    parameters.An8 = 0;
    parameters.An9 = 0;
    parameters.An10 = 0;
    parameters.An11 = 0;
    parameters.An12 = 0;

    // End-of-discharge voltage threshold
    parameters.VEOD = 3.2;
}

// Initialize state, given an initial voltage, current, and temperature
SystemModel::state_type BatteryModel::initialize(const input_type& u, const output_type& z) const {
    std::stringstream ss;
    ss << "Inputs: ";
    for (std::size_t i = 0; i < u.size(); ++i) {
        ss << u[i] << ", ";
    }
    logFile.WriteLine(LOG_TRACE, "MODEL-BATT", ss.str());
    ss.clear();
    ss << "Outputs: ";
    for (std::size_t i = 0; i < z.size(); ++i) {
        ss << z[i] << ", ";
    }
    logFile.WriteLine(LOG_TRACE, "MODEL-BATT", ss.str());
    // This is solved via a search procedure
    // Start by setting up an xp and xn vectors
    std::vector<double> xp, xn;
    double xi = 0.4;
    while (xi <= 1.0) {
        xp.push_back(xi);
        xn.push_back(1 - xi);
        xi += 0.0001;
    }

    // Initialize mole fractions
    double xpo = 0.4;
    double xno = 0.6;

    // Compute temperature in K (needed for equations below)
    double Tb = z[indices.outputs.Tbm] + 273.15;

    // Account for voltage drop due to input current (assuming no concentration gradient)
    double voltage = z[indices.outputs.Vm];
    double current = u[indices.inputs.P] / voltage;
    double Vo = current * parameters.Ro;

    // Now, construct the equilibrium potential voltage for each value of xp and xn
    for (size_t i = 0; i < xp.size(); i++) {
        // For xp
        double xpS = xp[i];
        double Vep0 = parameters.Ap0 * (2 * xpS - 1) / parameters.F;
        double Vep1 = parameters.Ap1 * (-2 * xpS * (-xpS + 1) + pow(2 * xpS - 1, 2)) / parameters.F;
        double Vep2 = parameters.Ap2 *
                      (-4 * xpS * (-xpS + 1) * (2 * xpS - 1) + pow(2 * xpS - 1, 3)) / parameters.F;
        double Vep3 = parameters.Ap3 *
                      (-6 * xpS * (-xpS + 1) * pow(2 * xpS - 1, 2) + pow(2 * xpS - 1, 4)) /
                      parameters.F;
        double Vep4 = parameters.Ap4 *
                      (-8 * xpS * (-xpS + 1) * pow(2 * xpS - 1, 3) + pow(2 * xpS - 1, 5)) /
                      parameters.F;
        double Vep5 = parameters.Ap5 *
                      (-10 * xpS * (-xpS + 1) * pow(2 * xpS - 1, 4) + pow(2 * xpS - 1, 6)) /
                      parameters.F;
        double Vep6 = parameters.Ap6 *
                      (-12 * xpS * (-xpS + 1) * pow(2 * xpS - 1, 5) + pow(2 * xpS - 1, 7)) /
                      parameters.F;
        double Vep7 = parameters.Ap7 *
                      (-14 * xpS * (-xpS + 1) * pow(2 * xpS - 1, 6) + pow(2 * xpS - 1, 8)) /
                      parameters.F;
        double Vep8 = parameters.Ap8 *
                      (-16 * xpS * (-xpS + 1) * pow(2 * xpS - 1, 7) + pow(2 * xpS - 1, 9)) /
                      parameters.F;
        double Vep9 = parameters.Ap9 *
                      (-18 * xpS * (-xpS + 1) * pow(2 * xpS - 1, 8) + pow(2 * xpS - 1, 10)) /
                      parameters.F;
        double Vep10 = parameters.Ap10 *
                       (-20 * xpS * (-xpS + 1) * pow(2 * xpS - 1, 9) + pow(2 * xpS - 1, 11)) /
                       parameters.F;
        double Vep11 = parameters.Ap11 *
                       (-22 * xpS * (-xpS + 1) * pow(2 * xpS - 1, 10) + pow(2 * xpS - 1, 12)) /
                       parameters.F;
        double Vep12 = parameters.Ap12 *
                       (-24 * xpS * (-xpS + 1) * pow(2 * xpS - 1, 11) + pow(2 * xpS - 1, 13)) /
                       parameters.F;
        double Vep = parameters.U0p + Vep0 + Vep1 + Vep10 + Vep11 + Vep12 + Vep2 + Vep3 + Vep4 +
                     Vep5 + Vep6 + Vep7 + Vep8 + Vep9 +
                     parameters.R * Tb * log((-xpS + 1) / xpS) / parameters.F;
        // For xn
        double xnS = xn[i];
        double Ven0 = parameters.An0 * (2 * xnS - 1) / parameters.F;
        double Ven1 = parameters.An1 * (-2 * xnS * (-xnS + 1) + pow(2 * xnS - 1, 2)) / parameters.F;
        double Ven2 = parameters.An2 *
                      (-4 * xnS * (-xnS + 1) * (2 * xnS - 1) + pow(2 * xnS - 1, 3)) / parameters.F;
        double Ven3 = parameters.An3 *
                      (-6 * xnS * (-xnS + 1) * pow(2 * xnS - 1, 2) + pow(2 * xnS - 1, 4)) /
                      parameters.F;
        double Ven4 = parameters.An4 *
                      (-8 * xnS * (-xnS + 1) * pow(2 * xnS - 1, 3) + pow(2 * xnS - 1, 5)) /
                      parameters.F;
        double Ven5 = parameters.An5 *
                      (-10 * xnS * (-xnS + 1) * pow(2 * xnS - 1, 4) + pow(2 * xnS - 1, 6)) /
                      parameters.F;
        double Ven6 = parameters.An6 *
                      (-12 * xnS * (-xnS + 1) * pow(2 * xnS - 1, 5) + pow(2 * xnS - 1, 7)) /
                      parameters.F;
        double Ven7 = parameters.An7 *
                      (-14 * xnS * (-xnS + 1) * pow(2 * xnS - 1, 6) + pow(2 * xnS - 1, 8)) /
                      parameters.F;
        double Ven8 = parameters.An8 *
                      (-16 * xnS * (-xnS + 1) * pow(2 * xnS - 1, 7) + pow(2 * xnS - 1, 9)) /
                      parameters.F;
        double Ven9 = parameters.An9 *
                      (-18 * xnS * (-xnS + 1) * pow(2 * xnS - 1, 8) + pow(2 * xnS - 1, 10)) /
                      parameters.F;
        double Ven10 = parameters.An10 *
                       (-20 * xnS * (-xnS + 1) * pow(2 * xnS - 1, 9) + pow(2 * xnS - 1, 11)) /
                       parameters.F;
        double Ven11 = parameters.An11 *
                       (-22 * xnS * (-xnS + 1) * pow(2 * xnS - 1, 10) + pow(2 * xnS - 1, 12)) /
                       parameters.F;
        double Ven12 = parameters.An12 *
                       (-24 * xnS * (-xnS + 1) * pow(2 * xnS - 1, 11) + pow(2 * xnS - 1, 13)) /
                       parameters.F;
        double Ven = parameters.U0n + Ven0 + Ven1 + Ven10 + Ven11 + Ven12 + Ven2 + Ven3 + Ven4 +
                     Ven5 + Ven6 + Ven7 + Ven8 + Ven9 +
                     parameters.R * Tb * log((-xnS + 1) / xnS) / parameters.F;
        // Compute equilibrium voltage
        double Ve = Vep - Ven;
        // Compute what voltage would be for this xp,xn
        double V = Ve - Vo;
        // In the xp vector, it starts at 0.4 which is fully charged.
        // So, the direction we are searching in is from fully charged to fully discharged.
        // We want to find the first predicted voltage that is less than observed voltage, and this
        // is the xp,xn we want.
        if (V <= voltage) {
            // Set mole fractions
            xpo = xp[i];
            xno = xn[i];
            // Stop the loop
            break;
        }
    }

    // Now, we have found the xp,xn corresponding to the voltage
    // Compute corresponding qS values for these mole fractions
    double qpS0 = parameters.qMax * xpo * parameters.VolS / parameters.Vol;
    double qnS0 = parameters.qMax * xno * parameters.VolS / parameters.Vol;
    // Compute qB values assuming that concentrations are equal (no concentration gradient)
    double qpB0 = qpS0 * parameters.VolB / parameters.VolS;
    double qnB0 = qnS0 * parameters.VolB / parameters.VolS;

    // Set x
    auto x = getStateVector();
    x[indices.states.Tb] = Tb;
    x[indices.states.Vo] = Vo;
    x[indices.states.Vsn] = 0;
    x[indices.states.Vsp] = 0;
    x[indices.states.qnB] = qnB0;
    x[indices.states.qnS] = qnS0;
    x[indices.states.qpB] = qpB0;
    x[indices.states.qpS] = qpS0;
    return x;
}
