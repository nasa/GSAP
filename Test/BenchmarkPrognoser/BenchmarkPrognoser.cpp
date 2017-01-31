/**  Model-based Prognoser - Body
 *   @class     BenchmarkPrognoser BenchmarkPrognoser.h
 *   @ingroup   GPIC++
 *   @ingroup   ProgLib
 *
 *   @brief     Benchmark Prognoser Class
 *
 *   BenchmarkPrognoser class.
 *
 *   @author    Micah Ricks
 *   @version   0.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoster Configuration Files
 *
 *      Contact: Micah Ricks (mricks1@bulldogs.aamu.edu)
 *      Created: January 31, 2017
 *
 *   @copyright Copyright (c) 2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

 #include <stdio.h>
 #include <stdlib.h>

#include <memory>
#include <vector>

#include <iostream>
#include <fstream>

#include "SharedLib.h"
#include "BenchmarkPrognoser.h"
#include "ObserverFactory.h"
#include "PredictorFactory.h"
#include "PrognosticsModelFactory.h"
#include "UData.h"
#include "CommManager.h"
#include "GSAPConfigMap.h"
#include <ctime>

namespace PCOE {
    // Configuration Keys
    const std::string MODEL_KEY = "model";
    const std::string OBSERVER_KEY = "observer";
    const std::string PREDICTOR_KEY = "predictor";
    const std::string EVENT_KEY = "Model.event";
    const std::string NUMSAMPLES_KEY = "Predictor.numSamples";
    const std::string HORIZON_KEY = "Predictor.horizon";
    const std::string PREDICTEDOUTPUTS_KEY = "Model.predictedOutputs";
    const std::string INPUTS_KEY = "inputs";
    const std::string OUTPUTS_KEY = "outputs";
    adder a;
const unsigned long STEP_SIZE = 50;

    BenchmarkPrognoser::BenchmarkPrognoser(GSAPConfigMap & configMap) : CommonPrognoser(configMap), initialized(false) {
        // Check for required config parameters
        configMap.checkRequiredParams({ MODEL_KEY,OBSERVER_KEY,PREDICTOR_KEY,EVENT_KEY,NUMSAMPLES_KEY,HORIZON_KEY,PREDICTEDOUTPUTS_KEY,INPUTS_KEY,OUTPUTS_KEY });
        /// TODO(CT): Move Model, Predictor subkeys into Model/Predictor constructor

        // Create Model
        log.WriteLine(LOG_DEBUG, moduleName, "Creating Model");
        PrognosticsModelFactory & pProgModelFactory = PrognosticsModelFactory::instance();
        model = std::unique_ptr<PrognosticsModel>(pProgModelFactory.Create(configMap[MODEL_KEY][0], configMap));

        // Create Observer
        log.WriteLine(LOG_DEBUG, moduleName, "Creating Observer");
        ObserverFactory & pObserverFactory = ObserverFactory::instance();
        observer = std::unique_ptr<Observer>(pObserverFactory.Create(configMap[OBSERVER_KEY][0], configMap));

        // Create Predictor
        log.WriteLine(LOG_DEBUG, moduleName, "Creating Predictor");
        PredictorFactory & pPredictorFactory = PredictorFactory::instance();
        predictor = std::unique_ptr<Predictor>(pPredictorFactory.Create(configMap[PREDICTOR_KEY][0], configMap));

        // Set model for observer and predictor
        observer->setModel(model.get());
        predictor->setModel(model.get());

        // Set configuration parameters
        unsigned int numSamples = static_cast<unsigned int>(std::stoul(configMap[NUMSAMPLES_KEY][0]));
        unsigned int horizon = static_cast<unsigned int>(std::stoul(configMap[HORIZON_KEY][0]));
        std::string event = configMap[EVENT_KEY][0];
        std::vector<std::string> predictedOutputs = configMap[PREDICTEDOUTPUTS_KEY];

        // Set inputs and outputs
        inputs = configMap[INPUTS_KEY];
        outputs = configMap[OUTPUTS_KEY];

        // Create progdata
        results.setUncertainty(UType::Samples);             // @todo(MD): do not force samples representation
        results.addEvent(event);                            // @todo(MD): do not assume only a single event
        results.addSystemTrajectories(predictedOutputs);    // predicted outputs
        results.setPredictions(1, horizon);                 // interval, number of predictions
        results.setupOccurrence(numSamples);
        results.events[event].timeOfEvent.npoints(numSamples);
        results.sysTrajectories.setNSamples(numSamples);
    }

    void BenchmarkPrognoser::step() {
        //init time in nanoseconds
        unsigned long long  begin= nanosecondsNow();




        static double initialTime = comm.getValue(outputs[0]).getTime() / 1.0e3;

        // Get new relative time (convert to seconds)
        // @todo(MD): Add config for time units so conversion is not hard-coded
        double newT = comm.getValue(outputs[0]).getTime() / 1.0e3 - initialTime;

        // Fill in input and output data
        log.WriteLine(LOG_DEBUG, moduleName, "Getting data in step");
        std::vector<double> u(model->getNumInputs());
        std::vector<double> z(model->getNumOutputs());
        for (unsigned int i = 0; i < model->getNumInputs(); i++) {
            u[i] = comm.getValue(inputs[i]);
        }
        for (unsigned int i = 0; i < model->getNumOutputs(); i++) {
            z[i] = comm.getValue(outputs[i]);
        }

        // If this is the first step, will want to initialize the observer and the predictor
        if (!initialized) {
            log.WriteLine(LOG_DEBUG, moduleName, "Initializing BenchmarkPrognoser");
            std::vector<double> x(model->getNumStates());
            model->initialize(x, u, z);
            observer->initialize(newT, x, u);
            initialized = true;
            lastTime = newT;
        } else {
            // If time has not advanced, skip this step
            if (newT <= lastTime) {
                log.WriteLine(LOG_TRACE, moduleName, "Skipping step because time did not advance.");

            unsigned long long  end= nanosecondsNow();
            unsigned long long elapsed_secs = (end - begin);
            a.addNum(elapsed_secs);

                return;
            }

            // Run observer
            log.WriteLine(LOG_DEBUG, moduleName, "Running Observer Step");
            observer->step(newT, u, z);
            log.WriteLine(LOG_DEBUG, moduleName, "Done Running Observer Step");

            // Run predictor
            log.WriteLine(LOG_DEBUG, moduleName, "Running Prediction Step");
            // Set up state
            std::vector<UData> stateEst = observer->getStateEstimate();
            predictor->predict(newT, stateEst, results);
            log.WriteLine(LOG_DEBUG, moduleName, "Done Running Prediction Step");

            // Set lastTime
            lastTime = newT;

        }
    }

//destructor
BenchmarkPrognoser::~BenchmarkPrognoser() {
  std::ofstream file;
  file.open("test.txt");
  file <<"Average time: " << a.getAverage()<<std::endl
  <<"Number of times step function ran: "<<a.getCounter()<<std::endl
  <<"Highest time: "<<a.getMax()<<std::endl <<"Lowest time: "<<a.getMin()<<std::endl;
  file.close();
}

}
