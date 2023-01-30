// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#ifndef PCOE_MODELBASEDPROGNOSER_H
#define PCOE_MODELBASEDPROGNOSER_H

#include <map>
#include <memory>

#include "Models/PrognosticsModel.h"
#include "Observers/Observer.h"
#include "Prognoser.h"

namespace PCOE {
    /**
     * @brief Model-based Prognoser Class
     *
     * @author Matthew Daigle
     * @author Christopher Teubert
     * @author Jason Watkins
     */
    class ModelBasedPrognoser : public Prognoser {
    private:
        std::unique_ptr<PrognosticsModel> model;
        std::unique_ptr<Observer> observer;
        std::unique_ptr<Predictor> predictor;
        std::unique_ptr<LoadEstimator> loadEstimator;
        bool initialized;
        double lastTime;
        TrajectoryService trajectoryService;

    public:
        /**
         * Create a model based prognoser from configuration
         *
         * @param config Map of config parameters from the prognoser config file
         */
        ModelBasedPrognoser(ConfigMap& config);

        /**
         * Create a model based prognoser from parts
         *
         * @param mdl   Prognostics model
         * @param obs   Observer
         * @param pred  Predictor
         * @param ldest Load Estimator
         **/
        ModelBasedPrognoser(PrognosticsModel& mdl,
                            Observer& obs,
                            Predictor& pred,
                            LoadEstimator& ldest);

        /**
         * Produce a new prediction based on the provided data.
         */
        Prediction step(std::map<MessageId, Datum<double>> data);

        const PrognosticsModel& getModel() {
            return *model;
        }
    };
}

#endif
