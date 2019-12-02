//
//  ECBatteryModel.cpp
//  sws-metrics
//
//  Created by Sierra Paez, Gina K. (ARC-TI)[SGT, INC] on 11/26/19.
//
// Copyright (c) 2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Models/ECBatteryModel.h"

#include "MathUtils.h"

using namespace PCOE;

const usize state_size = ECBatteryModel::state_size;
const usize input_size = ECBatteryModel::input_size;
const usize output_size = ECBatteryModel::output_size;

ECBatteryModel::ECBatteryModel() {
    
    parameters.vL     = 11.1484939314367;
    parameters.gamma  = 3.35528174473004;
    parameters.beta   = 8.48265208876828;
    parameters.lambda = 0.0463517799905509;
    parameters.mu     = 2.75931102946793;
    
    parameters.x0.Rint   = 0.0273193836397481;
    parameters.x0.SOC    = 1;
    parameters.x0.Ecrit  = 202426.858437571;
    
    
    parameters.x0Variance.Rint    = 1.2000e-06;
    parameters.x0Variance.SOC     = 1.6333e-06;
    parameters.x0Variance.Ecrit   = 1763.333;
    
    parameters.v.Rint    = 1.2000e-07;
    parameters.v.SOC     = 1.6333e-07;
    parameters.v.Ecrit   = 176.3333;
    
    parameters.n.Vm      = 5e-3;
    
    parameters.min.Rint      =  0;
    parameters.min.SOC       =  0;
    parameters.min.Ecrit     =  0;
   
    parameters.max.Rint      =  std::numeric_limits<double>::infinity();;
    parameters.max.SOC       =  1;
    parameters.max.Ecrit     =  203796;
    
    parameters.VEOD = 10.3;
}

ECBatteryModel:: ~ECBatteryModel() {}

std::array<f64, state_size> ECBatteryModel::stateEqn(std::array<f64, state_size> x,
                                                              std::array<f64, input_size> u,
                                                              std::array<f64, state_size> n,
                                                              f64 dt) {
    // Extract states
    f64 Rint    = x[0];
    f64 SOC     = x[1];
    f64 Ecrit   = x[2];

    // Extract inputs
    f64 P = u[0];
    
    // Constraints
    
    // Update state
    std::array<f64, state_size> x_new;
    x_new[0] = Rint;
    x_new[1] = SOC - P*dt/Ecrit;
    x_new[2] = Ecrit;

    
    // Add process noise
    for (usize it = 0; it < state_size; it++) {
        x_new[it] += dt * n[it];
    }
    
    x_new[0] = std::min(std::max(x_new[0], parameters.min.Rint),parameters.max.Rint);
    x_new[1] = std::min(std::max(x_new[1], parameters.min.SOC),parameters.max.SOC);
    x_new[2] = std::min(std::max(x_new[2], parameters.min.Ecrit),parameters.max.Ecrit);
    
    return x_new;
}

std::array<f64, output_size> ECBatteryModel::outputEqn( std::array<f64, state_size> x,
                                                        std::array<f64, input_size> u,
                                                        std::array<f64, output_size> v) {
    // Extract states
    f64 Rint    = x[0];
    f64 SOC     = x[1];
    f64 Ecrit   = x[2];
    
    
    // Extract inputs
    f64 P = u[0];
    
    // Constraints
    f64 Voc = parameters.vL + parameters.lambda*exp(parameters.gamma*SOC)- parameters.mu*exp(-parameters.beta*sqrt(SOC));
    f64 a=Rint;
    f64 b=-Voc;
    f64 c=P;
    f64 I=-(b + sqrt(pow(b,2) - 4.*a*c))/(2*a);
    f64 Vm = Voc - I*Rint;
    
    // Set outputs
    std::array<f64, output_size> z_new;
    z_new[0] = Vm;
    
    // Add noise
    for (std::size_t it = 0; it < output_size; it++) {
        z_new[it] += v[it];
    }
    return z_new;
}

std::array<f64, state_size> ECBatteryModel::initialize(const std::array<f64, input_size>& u,
                                                       const std::array<f64, output_size>& z) {
    
    // Extract inputs
    f64 P = u[0];
    
    //Extract output
    f64 Vm = z[0];
    
    f64 I = P/Vm;
    
    f64 Rinto = parameters.x0.Rint;
    f64 Ecrito = parameters.x0.Ecrit;
    f64 SOCo = 1;
    f64 Vo = I * Rinto;
  
    std::vector<f64> SOCi;
    f64 j = 0;
    while (j <= 1.0) {
        SOCi.push_back(1-j);
        j += 1e-6;
    }
    
     for (size_t i = 0; i < SOCi.size(); i++) {
         f64 SOC = SOCi[i];
         f64 Voc = parameters.vL + parameters.lambda*exp(parameters.gamma*SOC)- parameters.mu*exp(-parameters.beta*sqrt(SOC));
         f64 V = Voc - Vo;
         
         if (V <= Vm){
             SOCo = SOC;
             break;
         }
     }
    
    std::array<f64, state_size> x0;
    
    x0[0] = Rinto;
    x0[1] = SOCo;
    x0[2] = Ecrito;
 
    return x0;
}

// Battery Threshold Equation
std::array<bool, 1> ECBatteryModel::thresholdEqn(const std::array<f64, state_size>& x,
                                                 const std::array<f64, input_size>& u) {
    // Compute based on voltage, so use output equation to get voltage
    auto z = outputEqn(x, u, std::array<f64, output_size>());
    
    // Determine if voltage (second element in z) is below VEOD threshold
    return {z[0] <= parameters.VEOD};
}
