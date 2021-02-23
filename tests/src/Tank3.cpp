// Copyright (c) 2016-2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Tank3.h"
#include <vector>

using namespace std;

// Tank3 State Equation
PCOE::PrognosticsModel::state_type Tank3::stateEqn(const double,
                                        const state_type& x,
                                        const input_type& u,
                                        const double dt) const {
    // Extract states
    double m1 = x[0];
    double m2 = x[1];
    double m3 = x[2];

    // Extract inputs
    double u1 = u[0];
    double u2 = u[1];
    double u3 = u[2];

    // Constraints
    double p3 = m3 / parameters.K3;
    double p1 = m1 / parameters.K1;
    double q3 = p3 / parameters.R3;
    double p2 = m2 / parameters.K2;
    double q2c3 = (p2 - p3) / parameters.R2c3;
    double q2 = p2 / parameters.R2;
    double q1c2 = (p1 - p2) / parameters.R1c2;
    double m3dot = q2c3 - q3 + u3;
    double q1 = p1 / parameters.R1;
    double m2dot = q1c2 - q2 - q2c3 + u2;
    double m1dot = -q1 - q1c2 + u1;

    // Update state
    auto x_out = getStateVector();
    x_out[0] = m1 + m1dot * dt;
    x_out[1] = m2 + m2dot * dt;
    x_out[2] = m3 + m3dot * dt;

    return x_out;
}

// Tank3 Output Equation
PCOE::PrognosticsModel::output_type
Tank3::outputEqn(const double, const state_type& x) const {
    // Extract states
    double m1 = x[0];
    double m2 = x[1];
    double m3 = x[2];

    // Constraints
    double p1 = m1 / parameters.K1;
    double p2 = m2 / parameters.K2;
    double p3 = m3 / parameters.K3;
    double p2m = p2;
    double p3m = p3;
    double p1m = p1;

    auto z_out = getOutputVector();
    // Set outputs
    z_out[0] = p1m;
    z_out[1] = p2m;
    z_out[2] = p3m;

    return z_out;
}

PCOE::PrognosticsModel::state_type Tank3::initialize(const input_type&, const output_type&) const {
    auto x = getStateVector();
    x[0] = 0;
    x[1] = 0;
    x[2] = 0;
    return x;
}
