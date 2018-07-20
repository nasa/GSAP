// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_PREDICTOR_H
#define PCOE_PREDICTOR_H

#include <string>
#include <vector>

#include "Assert.h"
#include "LoadEstimator.h"
#include "ProgData.h"
#include "PrognosticsModel.h"

namespace PCOE {
    struct Prediction {
        std::vector<ProgEvent> events;
        std::vector<DataPoint> sysTrajectories;
    };

    /**
     * Represents a model-based predictor.
     *
     * @author Matthew Daigle
     * @author Jason Watkins
     **/
    class Predictor {
    public:
        /**
         * Constructs a new predictor using configuration options specified in
         * the given config map.
         *
         * @param m      The model used by the predictor.
         * @param le     The load estimator used by the predictor.
         * @param config A config map containing configuration parameters for
         *               the predictor.
         **/
        Predictor(const PrognosticsModel* m, LoadEstimator* le, GSAPConfigMap& config)
            : model(m), loadEstimator(le) {
            Expect(m != nullptr, "Model is null");
            Expect(le != nullptr, "Load Estimator is null");
            // Note (JW):
            // Want to keep config param in case we ever add optional config values
            // This cast suppresses warnings about the unused variable.
            static_cast<void>(config);
        }

        /**
         * Default destructor. A virtual default destructor is necessary to
         * ensure that resources in classes inherting from this class are
         * cleaned up, even if the destructor is called through a pointer of the
         * base class type.
         **/
        virtual ~Predictor() = default;

        /**
         * Predict future events and values of system variables
         *
         * @param t     Time of prediction
         * @param state State of system at time of prediction
         **/
        virtual Prediction predict(double t, const std::vector<UData>& state) = 0;

        /**
         * Gets a list of the events predicted by the current predictor.
         **/
        inline const std::vector<std::string>& getEvents() {
            return events;
        }

        /**
         * Gets a list of the outputs (system trajectories) predicted by the
         * current predictor.
         **/
        inline const std::vector<std::string>& getPredictedOutputs() {
            return predictedOutputs;
        }

    protected:
        /**
         * Sets the events predicted by the current predictor.
         **/
        inline void setEvents(std::vector<std::string> value) {
            events = std::move(value);
        }

        /**
         * Sets the outputs (system trajectories) predicted by the current
         * predictor.
         **/
        inline void PredictedOutputs(std::vector<std::string> value) {
            predictedOutputs = std::move(value);
        }

        LoadEstimator* loadEstimator;
        const PrognosticsModel* model;

    private:
        std::vector<std::string> events;
        std::vector<std::string> predictedOutputs;
    };
}

#endif
