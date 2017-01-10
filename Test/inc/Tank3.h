// Copyright © 2016 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.  All Rights Reserved.
#ifndef TANK3_H
#define TANK3_H

#include <vector>
#include "Model.h"

class Tank3 final : public PCOE::Model {
public:
    // Constructor
    Tank3() {
        numStates = 3;
        numInputs = 3;
        numOutputs = 3;
        m_dt = 1;
    }

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

    void stateEqn(const double t, std::vector<double> & x, const std::vector<double> & u, const std::vector<double> & n, const double dt);
    void outputEqn(const double t, const std::vector<double> & x, const std::vector<double> & u, const std::vector<double> & n, std::vector<double> & z);
    void initialize(std::vector<double> & x, const std::vector<double> & u, const std::vector<double> & z);
};
#endif
