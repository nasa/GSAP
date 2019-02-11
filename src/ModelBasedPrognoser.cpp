/**  Model-based Prognoser - Body
 *   @class     ModelBasedPrognoser ModelBasedPrognoser.h
 *   @ingroup   GPIC++
 *   @ingroup   ProgLib
 *
 *   @brief     Model-based Prognoser Class
 *
 *   General model-based prognoser class. It gets created for a specified model,
 *   observer, and predictor.
 *
 *   @author    Matthew Daigle
 *   @version   1.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoster Configuration Files
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: March 16, 2016
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <memory>
#include <vector>

#include <iostream>

#include "Loading/LoadEstimatorFactory.h"
#include "ModelBasedPrognoser.h"
#include "Models/PrognosticsModelFactory.h"
#include "Observers/ObserverFactory.h"
#include "Predictors/PredictorFactory.h"
#include "ThreadSafeLog.h"
#include "UData.h"

namespace PCOE {
    // Configuration Keys
    const std::string MODEL_KEY = "model";
    const std::string OBSERVER_KEY = "observer";
    const std::string PREDICTOR_KEY = "predictor";
    const std::string LOAD_EST_KEY = "Predictor.loadEstimator";

    const std::string DEFAULT_LOAD_EST = "MovingAverage";

    std::string moduleName = "ModelBasedPrognoser";

    ModelBasedPrognoser::ModelBasedPrognoser(ConfigMap& configMap) : initialized(false) {
        // Check for required config parameters
        requireKeys(configMap, {MODEL_KEY, OBSERVER_KEY, PREDICTOR_KEY});
        /// TODO(CT): Move Model, Predictor subkeys into Model/Predictor constructor

        // Create Model
        log.WriteLine(LOG_DEBUG, moduleName, "Creating Model");
        PrognosticsModelFactory& pProgModelFactory = PrognosticsModelFactory::instance();
        model = pProgModelFactory.Create(configMap.getVector(MODEL_KEY)[0], configMap);

        // Create Observer
        log.WriteLine(LOG_DEBUG, moduleName, "Creating Observer");
        ObserverFactory& pObserverFactory = ObserverFactory::instance();
        observer = pObserverFactory.Create(configMap.getVector(OBSERVER_KEY)[0], *model, configMap);

        // Create Load Estimator
        log.WriteLine(LOG_DEBUG, moduleName, "Creating Load Estimator");
        LoadEstimatorFactory& loadEstFact = LoadEstimatorFactory::instance();
        if (configMap.hasKey(LOAD_EST_KEY)) {
            loadEstimator = loadEstFact.Create(configMap.getVector(LOAD_EST_KEY)[0], configMap);
        }
        else {
            // If not specified, use default
            loadEstimator = loadEstFact.Create(DEFAULT_LOAD_EST, configMap);
        }

        // Create Predictor
        log.WriteLine(LOG_DEBUG, moduleName, "Creating Predictor");
        PredictorFactory& pPredictorFactory = PredictorFactory::instance();
        predictor = pPredictorFactory.Create(configMap.getVector(PREDICTOR_KEY)[0],
                                             *model,
                                             *loadEstimator,
                                             trajectoryService,
                                             configMap);
    }

    ModelBasedPrognoser::ModelBasedPrognoser(PrognosticsModel& mdl,
                                             Observer& obs,
                                             Predictor& pred,
                                             LoadEstimator& ldest)
        : model(&mdl),
          observer(&obs),
          predictor(&pred),
          loadEstimator(&ldest),
          initialized(false) {}

    Prediction ModelBasedPrognoser::step(std::map<MessageId, Datum<double>> data) {
        // Get new time (convert to seconds)
        // @todo(MD): Add config for time units so conversion is not hard-coded
        double newT_s = data[model->getInputs()[0]].getTime() / 1.0e3;

        // Fill in input and output data
        log.WriteLine(LOG_DEBUG, moduleName, "Getting data in step");
        auto u = model->getInputVector();
        auto z = model->getOutputVector();
        auto inputNames = model->getInputs();
        for (unsigned int i = 0; i < model->getInputSize(); i++) {
            log.FormatLine(LOG_TRACE, "PROG-MBP", "Getting input %u", i);
            MessageId& input_name = inputNames[i];
            log.FormatLine(LOG_TRACE, "PROG-MBP", "Getting input %ull", input_name);
            Datum<double> input = data[input_name];

            log.FormatLine(LOG_TRACE,
                           "PROG-MBP",
                           "Got input (%f, %ul)",
                           input.get(),
                           input.getTime());
            log.WriteLine(LOG_TRACE, "PROG-MBP", "Checking whether input is set");
            if (!input.isSet()) {
                // Do nothing if data not yet available
                log.WriteLine(LOG_TRACE, "PROG-MBP", "Data not yet available. Returning.");
                return Prediction::EmptyPrediction();
            }
            log.WriteLine(LOG_TRACE, "PROG-MBP", "Reading data");
            u[i] = input;
            log.WriteLine(LOG_TRACE, "PROG-MBP", "Adding load");
            if (loadEstimator->canAddLoad()) {
                loadEstimator->addLoad(u.vec());
            }
        }
        for (unsigned int i = 0; i < model->getOutputSize(); i++) {
            log.WriteLine(LOG_TRACE, "PROG-MBP", "Reading data");
            Datum<double> output = data[model->getOutputs()[i]];
            if (!output.isSet()) {
                // Do nothing if data not yet available
                return Prediction::EmptyPrediction();
            }
            z[i] = output;
        }

        // If this is the first step, will want to initialize the observer and the predictor
        if (!initialized) {
            log.WriteLine(LOG_DEBUG, moduleName, "Initializing ModelBasedPrognoser");
            auto x = model->initialize(u, z);
            observer->initialize(newT_s, x, u);
            initialized = true;
            lastTime = newT_s;
        }
        else {
            // If time has not advanced, skip this step
            if (newT_s <= lastTime) {
                log.WriteLine(LOG_TRACE, moduleName, "Skipping step because time did not advance.");
                return Prediction::EmptyPrediction();
            }

            try {
                // Run observer
                log.WriteLine(LOG_DEBUG, moduleName, "Running Observer Step");
                observer->step(newT_s, u, z);
                log.WriteLine(LOG_DEBUG, moduleName, "Done Running Observer Step");

                // Run predictor
                log.WriteLine(LOG_DEBUG, moduleName, "Running Prediction Step");
                // Set up state
                std::vector<UData> stateEst = observer->getStateEstimate();
                Prediction prediction = predictor->predict(newT_s, stateEst);
                log.WriteLine(LOG_DEBUG, moduleName, "Done Running Prediction Step");

                // Set lastTime
                lastTime = newT_s;
                return prediction;
            }
            catch (...) {
                log.WriteLine(LOG_ERROR, moduleName, "Error in Step, skipping");
            }
        }
        return Prediction::EmptyPrediction();
    }
}
