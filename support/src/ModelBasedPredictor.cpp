//
//  Predictor2.cpp
//  TestMCSampler
//
//  Created by Teubert, Christopher (ARC-TI)[SGT, INC] on 10/26/17.
//  Copyright © 2017 NASA. All rights reserved.
//

#include <stdio.h>
#include <cassert>
#include <random>
#include <string>
#include <vector>

#include "Exceptions.h"
#include "ModelBasedPredictor.h"

namespace PCOE {
    // Configuration Keys
    const std::string EVENT_KEY = "Model.event";
    const std::string PREDICTEDOUTPUTS_KEY = "Model.predictedOutputs";
    const std::string PROCESSNOISE_KEY = "Model.processNoise";
    const std::string HORIZON_KEY = "Predictor.horizon";
    
    // Other string constants
    const std::string MODULE_NAME = "MonteCarloPredictor";
    // ConfigMap-based Constructor
    ModelBasedPredictor::ModelBasedPredictor(GSAPConfigMap & configMap) : Predictor2() {
        // Check for required parameters:
        // model = model to be used for simulation
        // numSamples = number of samples used for prediction
        // horizon = prediction horizon (time units)
        // processNoise = list of variance values for process noise, one for each state (zero-mean assumption)
        // event = name of event to predict
        // inputUncertainty = specification of uncertainty associated with inputParameters in pModel->inputEqn
        configMap.checkRequiredParams({ EVENT_KEY, HORIZON_KEY, PREDICTEDOUTPUTS_KEY, PROCESSNOISE_KEY });
        
        // Set configuration parameters
        horizon = std::stoul(configMap[HORIZON_KEY][0]);
        event = configMap[EVENT_KEY][0];
        
        // Set up process noise
        std::vector<std::string> processNoiseStrings = configMap[PROCESSNOISE_KEY];
        for (unsigned int i = 0; i < processNoiseStrings.size(); i++) {
            processNoise.push_back(std::stod(processNoiseStrings[i]));
        }
        
        // Set up predicted outputs
        predictedOutputs = configMap[PREDICTEDOUTPUTS_KEY];
        
        log.WriteLine(LOG_INFO, MODULE_NAME, "ModelBasedPredictor created");
    }
    
    // Set model
    void ModelBasedPredictor::setModel(PrognosticsModel * model) {
        pModel = model;
        
        // Check that process noise was set consistent with this model
        if (processNoise.size() != pModel->getNumStates()) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "Process noise size does not equal number of model states");
            throw std::range_error("Process noise size does not equal number of model states");
        }
        
        // Check that there are the correct number of predicted outputs
        if (predictedOutputs.size() != pModel->getNumPredictedOutputs()) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "Number of predicted outputs does not equal number of model's predicted outputs");
            throw std::range_error("Number of predicted outputs does not equal number of model's predicted outputs");
        }
    }
    
    // Predict function
    void ModelBasedPredictor::predict(const double tP, const std::vector<state_vec> & stateSamples, const std::vector<load_vec> & loadSamples, ProgData & data) {
        // @todo(MD): This is setup for only a single event to predict, need to extend to multiple events
        
        static std::random_device rDevice;
        static std::mt19937 generator(rDevice());
        
        // Check that model has been set
        if (pModel == NULL) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "ModelBasedPredictor does not have a model!");
            throw ConfigurationError("ModelBasedPredictor does not have a model!");
        }
        
        // @TODO(CT): Move Check
        assert(loadSamples.size() > 0);
        
        std::vector<double> inputParameters(loadSamples[0].size());
        std::vector<double> u(pModel->getNumInputs());
        std::vector<double> z(pModel->getNumPredictedOutputs());
        std::vector<double> noise(pModel->getNumStates());
        
        
        auto & theEvent = data.events[event];
        theEvent.setNumOccurrenceSamples(stateSamples.size());
        
        // For each sample
        for (unsigned int sample = 0; sample < stateSamples.size(); sample++) {
            auto x = stateSamples[sample];
            auto && loadSample = loadSamples[sample];
            // @TODO(CT): Optimize using pointer to current

            // Simulate until time limit reached
            double t = tP;
            unsigned int timeIndex = 0;
            data.events[event].timeOfEvent[sample] = INFINITY;
            while (t <= tP + horizon) {
                // Calculate load at that timeindex
                for (unsigned int loadIndex = 0; loadIndex < loadSample.size(); loadIndex ++) {
                    // For each load
                    for (auto && loadTimepoint : loadSample[loadIndex]) {
                        inputParameters[loadIndex] = loadTimepoint.second; // Todo(CT): Optimize
                        if (t <= loadTimepoint.first) {
                            break; // this is the one to use
                        }
                    }
                }
                
                pModel->inputEqn(t, inputParameters, u);
                
                // Check threshold at time t and set timeOfEvent if reaching for first time
                // If timeOfEvent is not set to INFINITY that means we already encountered the event,
                // and we don't want to overwrite that.
                theEvent.occurrenceMatrix[timeIndex][sample] = pModel->thresholdEqn(t, x, u);
                if (theEvent.occurrenceMatrix[timeIndex][sample] && theEvent.timeOfEvent[sample] == INFINITY) {
                    theEvent.timeOfEvent[sample] = t;
                    continue;
                }
                
                // Write to system trajectory (model variables for which we are interested in predicted values)
                pModel->predictedOutputEqn(t, x, u, z);
                for (unsigned int p = 0; p < pModel->getNumPredictedOutputs(); p++) {
                    data.sysTrajectories[predictedOutputs[p]][timeIndex][sample] = z[p];
                }
                
                // Sample process noise - for now, assuming independent
                for (unsigned int xIndex = 0; xIndex < pModel->getNumStates(); xIndex++) {
                    std::normal_distribution<> noiseDistribution(0, sqrt(processNoise[xIndex]));
                    noise[xIndex] = noiseDistribution(generator);
                }
                
                // Update state for t to t+dt
                pModel->stateEqn(t, x, u, noise);
                
                // Update time
                t += pModel->getDt();
                timeIndex++;
            }
        }
    }
}
