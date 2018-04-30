/**  MonteCarloPredictor - Header
 *   @file      MonteCarloPredictor Class
 *   @ingroup   GPIC++
 *   @ingroup    Predictors
 *
 *   @brief     MonteCarloPredictor Class - Class defining the MonteCarloPredictor
 *
 *   @author    Matthew Daigle
 *   @version   1.1.0
 *
 *   @pre       N/A
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: March 22, 2016
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <random>
#include <string>
#include <vector>

#include "Exceptions.h"
#include "Matrix.h"
#include "MonteCarloPredictor.h"

namespace PCOE {
    // Configuration Keys
    const std::string PROCESSNOISE_KEY = "Model.processNoise";
    const std::string NUMSAMPLES_KEY = "Predictor.numSamples";
    const std::string HORIZON_KEY = "Predictor.horizon";

    // Other string constants
    const std::string MODULE_NAME = "MonteCarloPredictor";

    // ConfigMap-based Constructor
    MonteCarloPredictor::MonteCarloPredictor(GSAPConfigMap& configMap) : Predictor(configMap) {
        // Check for required parameters:
        // model = model to be used for simulation
        // numSamples = number of samples used for prediction
        // horizon = prediction horizon (time units)
        // processNoise = list of variance values for process noise, one for each state (zero-mean
        // assumption) event = name of event to predict inputUncertainty = specification of
        // uncertainty associated with inputParameters in pModel->inputEqn
        configMap.checkRequiredParams({NUMSAMPLES_KEY, HORIZON_KEY, PROCESSNOISE_KEY});

        // Set configuration parameters
        numSamples = static_cast<unsigned int>(std::stoul(configMap[NUMSAMPLES_KEY][0]));
        horizon = std::stoul(configMap[HORIZON_KEY][0]);

        // Set up process noise
        std::vector<std::string> processNoiseStrings = configMap[PROCESSNOISE_KEY];
        for (unsigned int i = 0; i < processNoiseStrings.size(); i++) {
            processNoise.push_back(std::stod(processNoiseStrings[i]));
        }

        log.WriteLine(LOG_INFO, MODULE_NAME, "MonteCarloPredictor created");
    }

    void MonteCarloPredictor::setModel(PrognosticsModel* model) {
        pModel = model;

        // Check that process noise was set consistent with this model
        if (processNoise.size() != pModel->getNumStates()) {
            log.WriteLine(LOG_ERROR,
                          MODULE_NAME,
                          "Process noise size does not equal number of model states");
            throw std::range_error("Process noise size does not equal number of model states");
        }
    }

    UData::size_type getLowestTimestamp(const std::vector<UData>& data) {
        UData::size_type result = std::numeric_limits<UData::size_type>::max();
        for (const UData& entry : data) {
            if (entry.updated() < result) {
                result = entry.updated();
            }
        }
        return result;
    }

    // Predict function
    void MonteCarloPredictor::predict(const double tP,
                                      const std::vector<UData>& state,
                                      ProgData& data) {
        // @todo(MD): This is setup for only a single event to predict, need to extend to multiple
        // events

        // Check that model has been set
        if (pModel == NULL) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "MonteCarloPredictor does not have a model!");
            throw ConfigurationError("MonteCarloPredictor does not have a model!");
        }

        auto stateTimestamp = getLowestTimestamp(state);

        // Create a random number generator if operating sequentially
        #ifndef USING_OPENMP
          static std::random_device rDevice;
          static std::mt19937 generator(rDevice());
        #endif

        Matrix xMean(pModel->getNumStates(), 1);

        // Assume for now that UData is mean and covariance type, and so we are assuming
        // multivariate normal NOTE: Can check UData uncertainty type to see what it is and how to
        // handle. Perhaps it would be useful to have general code to deal with this, to get samples
        // from it directly? So don't have to check within here. First step is to construct the mean
        // vector and covariance matrix from the UDatas
        Matrix Pxx(pModel->getNumStates(), pModel->getNumStates());
        for (unsigned int xIndex = 0; xIndex < pModel->getNumStates(); xIndex++) {
            xMean[xIndex][0] = state[xIndex][MEAN];
            std::vector<double> covariance = state[xIndex].getVec(COVAR());
            Pxx.row(xIndex, state[xIndex].getVec(COVAR(0)));
        }
        auto PxxChol = Pxx.chol();

        /* OpenMP info
        * If the application is built with OpenMP, the predictor below operates in parallel.
        * The only shared memory between threads is data (ProgData). Writebacks are only done
        * on a per-sample basis (which each have their own thread) so there are no race
        * conditions presently. Future updates to the predict method will need to consider
        * if data must be updated atomically.
        *
        * std::mt19937 is not thread safe, so when OpenMP is used the generator is found
        * inside the loop, otherwise it can be left outside.
        */
        // For each sample
        #pragma omp parallel for shared(data)
        for (unsigned int sample = 0; sample < numSamples; sample++) {
            // 0. Create random number generator if operating in parallel
            #ifdef USING_OPENMP
              std::random_device rDevice;
              std::mt19937 generator(rDevice());
            #endif

            // 1. Sample the state
            // Create state vector
            // Now we have mean vector (x) and covariance matrix (Pxx). We can use that to sample a
            // realization of the state. I need to generate a vector of random numbers, size of the
            // state vector Create standard normal distribution
            Matrix xRandom(pModel->getNumStates(), 1);
            static std::normal_distribution<> standardDistribution(0, 1);
            for (unsigned int xIndex = 0; xIndex < pModel->getNumStates(); xIndex++) {
                xRandom[xIndex][0] = standardDistribution(generator);
            }
            // Update with mean and covariance
            xRandom = xMean + PxxChol * xRandom;
            std::vector<double> x = static_cast<std::vector<double>>(xRandom.col(0));

            // 3. Simulate until time limit reached
            std::vector<double> u(pModel->getNumInputs());
            std::vector<double> z(pModel->getNumPredictedOutputs());
            double t = tP;
            unsigned int timeIndex = 0;
            std::string event = pModel->events[0];
            data.events[event].getTOE()[sample] = INFINITY;
            while (t <= tP + horizon) {
                // Get inputs for time t
                std::vector<double> loadEstimate = loadEstFcn(t, sample);
                pModel->inputEqn(t, loadEstimate, u);

                // Check threshold at time t and set timeOfEvent if reaching for first time
                // If timeOfEvent is not set to INFINITY that means we already encountered the
                // event, and we don't want to overwrite that.
                auto& theEvent = data.events[event];
                theEvent.occurrenceMatrix[timeIndex][sample] = pModel->thresholdEqn(t, x, u);
                if (theEvent.occurrenceMatrix[timeIndex][sample] &&
                    theEvent.getTOE()[sample] == INFINITY) {
                    theEvent.getTOE()[sample] = t;
                    theEvent.getTOE().updated(stateTimestamp);
                    continue;
                }

                // Write to system trajectory (model variables for which we are interested in
                // predicted values)
                pModel->predictedOutputEqn(t, x, u, z);
                for (unsigned int p = 0; p < pModel->getNumPredictedOutputs(); p++) {
                    data.sysTrajectories[pModel->predictedOutputs[p]][timeIndex][sample] = z[p];
                }

                // Sample process noise - for now, assuming independent
                std::vector<double> noise(pModel->getNumStates());
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
