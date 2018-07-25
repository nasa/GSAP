// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MONTECARLOPREDICTOR_H
#define PCOE_MONTECARLOPREDICTOR_H

#include <string>
#include <vector>

#include "Predictors/Predictor.h"

namespace PCOE {
    /**
     * A predictor that uses Monte Carlo sampling.
     *
     * @author Matthew Daigle
     * @author Jason Watkins
     * @since 1.0
     **/
    class MonteCarloPredictor final : public Predictor {
    public:
        /**
         * Initializes a new @{code MonteCarloPredictor}.
         *
         * @param m      The model used by the predictor.
         * @param le     The load estimator used by the predictor.
         * @param config Configuration map specifying predictor parameters.
         **/
        MonteCarloPredictor(const PrognosticsModel* m,
                            LoadEstimator* le,
                            const GSAPConfigMap& config);

        /**
         * Predict future events and values of system variables
         *
         * @param t     Time of prediction
         * @param state State of system at time of prediction
         **/
        Prediction predict(double t, const std::vector<UData>& state) override;

    private:
        double horizon; // time span of prediction
        std::size_t sampleCount;
        std::vector<double> processNoise; // variance vector (zero-mean assumed)
    };
}
#endif
