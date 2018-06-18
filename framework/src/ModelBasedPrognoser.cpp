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

#include "GSAPConfigMap.h"
#include "LoadEstimatorFactory.h"
#include "ModelBasedPrognoser.h"
#include "ObserverFactory.h"
#include "PredictorFactory.h"
#include "PrognosticsModelFactory.h"
#include "UData.h"

namespace PCOE {
    // Configuration Keys
    const std::string MODEL_KEY         = "model";
    const std::string OBSERVER_KEY      = "observer";
    const std::string PREDICTOR_KEY     = "predictor";
    const std::string STEPSIZE_KEY      = "Model.stepSize";
    const std::string NUMSAMPLES_KEY    = "Predictor.numSamples";
    const std::string HORIZON_KEY       = "Predictor.horizon";
    const std::string LOAD_EST_KEY      = "Predictor.loadEstimator";

    const std::string DEFAULT_LOAD_EST  = "movingAverage";
    const double DEFAULT_STEPSIZE_S     = 1; // seconds

    ModelBasedPrognoser::ModelBasedPrognoser(GSAPConfigMap& configMap)
        : CommonPrognoser(configMap), initialized(false) {
        // Check for required config parameters
        configMap.checkRequiredParams({MODEL_KEY,
                                       OBSERVER_KEY,
                                       PREDICTOR_KEY,
                                       NUMSAMPLES_KEY,
                                       HORIZON_KEY});
        /// TODO(CT): Move Model, Predictor subkeys into Model/Predictor constructor

        // Create Model
        log.WriteLine(LOG_DEBUG, moduleName, "Creating Model");
        PrognosticsModelFactory& pProgModelFactory = PrognosticsModelFactory::instance();
        model                                      = std::unique_ptr<PrognosticsModel>(
            pProgModelFactory.Create(configMap[MODEL_KEY][0], configMap));

        // Create Observer
        log.WriteLine(LOG_DEBUG, moduleName, "Creating Observer");
        ObserverFactory& pObserverFactory = ObserverFactory::instance();
        observer                          = std::unique_ptr<Observer>(
            pObserverFactory.Create(configMap[OBSERVER_KEY][0], configMap));

        // Create Predictor
        log.WriteLine(LOG_DEBUG, moduleName, "Creating Predictor");
        PredictorFactory& pPredictorFactory = PredictorFactory::instance();
        predictor                           = std::unique_ptr<Predictor>(
            pPredictorFactory.Create(configMap[PREDICTOR_KEY][0], configMap));

        // Create Load Estimator
        log.WriteLine(LOG_DEBUG, moduleName, "Creating Load Estimator");
        LoadEstimatorFactory& loadEstFact = LoadEstimatorFactory::instance();
        if (configMap.includes(LOAD_EST_KEY)) {
            loadEstimator = std::unique_ptr<LoadEstimator>(
                loadEstFact.Create(configMap[LOAD_EST_KEY][0], configMap));
        }
        else {
            // If not specified, use default
            loadEstimator =
                std::unique_ptr<LoadEstimator>(loadEstFact.Create(DEFAULT_LOAD_EST, configMap));
        }

        // Set model stepsize
        if (configMap.includes(STEPSIZE_KEY)) {
            model->setDt(std::stod(configMap[STEPSIZE_KEY][0]));
        }
        else {
            model->setDt(DEFAULT_STEPSIZE_S);
        }

        // Set load estimator
        using std::placeholders::_1;
        using std::placeholders::_2;
        predictor->setLoadEst(std::bind(&LoadEstimator::estimateLoad, loadEstimator.get(), _1, _2));

        // Set model for observer and predictor
        observer->setModel(model.get());
        loadEstimator->setModel(model.get());
        predictor->setModel(model.get());

        for (auto&& input : model->inputs) {
            comm.registerKey(input);
        }
        for (auto&& output : model->outputs) {
            comm.registerKey(output);
        }

        // Set configuration parameters
        unsigned int numSamples =
            static_cast<unsigned int>(std::stoul(configMap[NUMSAMPLES_KEY][0]));
        unsigned int horizon = static_cast<unsigned int>(std::stoul(configMap[HORIZON_KEY][0]));

        // Create progdata
        results.setUncertainty(UType::Samples); // @todo(MD): do not force samples representation
        for (std::string & event : model->events) {
            results.addEvent(event);
            results.events[event].getTOE().npoints(numSamples);
        }
        results.addSystemTrajectories(model->predictedOutputs); // predicted outputs
        results.setPredictions(1, horizon); // interval, number of predictions
        results.setupOccurrence(numSamples);
        results.sysTrajectories.setNSamples(numSamples);
    }

    void ModelBasedPrognoser::step() {
        // Get new time (convert to seconds)
        // @todo(MD): Add config for time units so conversion is not hard-coded
        double newT_s = getValue(model->outputs[0]).getTime() / 1.0e3;

        // Fill in input and output data
        log.WriteLine(LOG_DEBUG, moduleName, "Getting data in step");
        std::vector<double> u(model->getNumInputs());
        std::vector<double> z(model->getNumOutputs());
        for (unsigned int i = 0; i < model->getNumInputs(); i++) {
            log.FormatLine(LOG_TRACE, "PROG-MBP", "Getting input %u", i);
            const std::string& input_name = model->inputs[i];
            log.FormatLine(LOG_TRACE, "PROG-MBP", "Getting input %s", input_name.c_str());
            Datum<double> input = getValue(input_name);

            log.FormatLine(LOG_TRACE,
                           "PROG-MBP",
                           "Got input (%f, %ul)",
                           input.get(),
                           input.getTime());
            log.WriteLine(LOG_TRACE, "PROG-MBP", "Checking whether input is set");
            if (!input.isSet()) {
                // Do nothing if data not yet available
                log.WriteLine(LOG_TRACE, "PROG-MBP", "Data not yet available. Returning.");
                return;
            }
            log.WriteLine(LOG_TRACE, "PROG-MBP", "Reading data");
            u[i] = getValue(model->inputs[i]);
            log.WriteLine(LOG_TRACE, "PROG-MBP", "Adding load");
            if (loadEstimator->usesHistoricalLoading()) {
                loadEstimator->addLoad(u);
            }
        }
        for (unsigned int i = 0; i < model->getNumOutputs(); i++) {
            log.WriteLine(LOG_TRACE, "PROG-MBP", "Checking whether output is set");
            if (!getValue(model->outputs[i]).isSet()) {
                // Do nothing if data not yet available
                return;
            }
            log.WriteLine(LOG_TRACE, "PROG-MBP", "Reading data");
            z[i] = getValue(model->outputs[i]);
        }

        // If this is the first step, will want to initialize the observer and the predictor
        if (!initialized) {
            log.WriteLine(LOG_DEBUG, moduleName, "Initializing ModelBasedPrognoser");
            std::vector<double> x(model->getNumStates());
            model->initialize(x, u, z);
            observer->initialize(newT_s, x, u);
            initialized = true;
            lastTime    = newT_s;
        }
        else {
            // If time has not advanced, skip this step
            if (newT_s <= lastTime) {
                log.WriteLine(LOG_TRACE, moduleName, "Skipping step because time did not advance.");
                return;
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
                predictor->predict(newT_s, stateEst, results);
                log.WriteLine(LOG_DEBUG, moduleName, "Done Running Prediction Step");

                // Set lastTime
                lastTime = newT_s;
            }
            catch (...) {
                log.WriteLine(LOG_ERROR, moduleName, "Error in Step, skipping");
            }
        }
    }
}
