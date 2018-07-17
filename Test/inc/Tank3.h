// Copyright © 2018 United States Government as represented by the Administrator of the National
// Aeronautics and Space Administration.  All Rights Reserved.
#ifndef TANK3_H
#define TANK3_H

#include "Model.h"
#include <vector>

class Tank3 final : public PCOE::Model {
public:
    // Constructor
    Tank3() : PCOE::Model(3, {"u1", "u2", "u3"}, {"p1m", "p2m", "p3m"}) {}

    // State indices
    struct stateIndices {
        static const unsigned int m1 = 0;
        static const unsigned int m2 = 1;
        static const unsigned int m3 = 2;
    };
    // Input indices
    struct inputIndices {
        static const unsigned int u1 = 0;
        static const unsigned int u2 = 1;
        static const unsigned int u3 = 2;
    };
    // Output indices
    struct outputIndices {
        static const unsigned int p1m = 0;
        static const unsigned int p2m = 1;
        static const unsigned int p3m = 2;
    };
    // Indices
    struct allIndices {
        struct stateIndices states;
        struct inputIndices inputs;
        struct outputIndices outputs;
    } indices;

    // Parameters
    struct Parameters {
        double R1c2;
        double K3;
        double R3;
        double K1;
        double R2;
        double R1;
        double K2;
        double R2c3;
    } parameters;

    using Model::stateEqn;

    state_type stateEqn(const double t,
                        const state_type& x,
                        const input_type& u,
                        const noise_type& n,
                        const double dt) const override;

    output_type outputEqn(const double t,
                          const state_type& x,
                          const input_type& u,
                          const noise_type& n) const override;

    state_type initialize(const input_type& u, const output_type& z) const override;
};
#endif
