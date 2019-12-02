//
//  ECBatteryModel.h
//  sws-test
//
//  Created by Sierra Paez, Gina K. (ARC-TI)[SGT, INC] on 11/26/19.

// Copyright (c) 2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_ECBATTERYMODEL_H
#define PCOE_ECBATTERYMODEL_H

#include "NumericTypes.h"

#include <array>

namespace PCOE {
    class ECBatteryModel {
        struct Parameters {
            // model parameters
            f64 vL;
            f64 gamma;
            f64 beta;
            f64 lambda;
            f64 mu;
            // initial state mean
            struct X0 {
                f64 Rint;
                f64 SOC;
                f64 Ecrit;
            }x0;
            // initial state variance
            struct X0Variance {
                f64 Rint;
                f64 SOC;
                f64 Ecrit;
            }x0Variance;
            // process noice
            struct V {
                f64 Rint;
                f64 SOC;
                f64 Ecrit;
            }v;
            // measurement noise
            struct N {
                f64 Vm;
            }n;
            //  minimum state values
            struct Min {
                f64 Rint;
                f64 SOC;
                f64 Ecrit;
            }min;
            //  maximum state values
            struct Max {
                f64 Rint;
                f64 SOC;
                f64 Ecrit;
            }max;
            // Voltage cutoff
            f64 VEOD;
        } parameters;
        
    public:
        const static usize state_size = 3;
        const static usize input_size = 1;
        const static usize output_size = 1;
        
        ECBatteryModel();
        
        ~ECBatteryModel();
        
        std::array<f64, state_size> stateEqn(std::array<f64, state_size> x,
                                             std::array<f64, input_size> u,
                                             std::array<f64, state_size> n,
                                             f64 dt);
        
        std::array<f64, output_size> outputEqn(std::array<f64, state_size> x,
                                               std::array<f64, input_size> u,
                                               std::array<f64, output_size> n);
        
        std::array<f64, state_size> initialize(const std::array<f64, input_size>& u,
                                               const std::array<f64, output_size>& z);
        
        std::array<bool, 1> thresholdEqn(const std::array<f64, state_size>& x,
                                         const std::array<f64, input_size>& u);
    };
}
#endif // ECBATTERYMODEL_H

