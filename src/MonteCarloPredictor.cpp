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
    const std::string EVENTS_KEY = "Predictor.events";

    // Other string constants
    const std::string MODULE_NAME = "MonteCarloPredictor";

    // ConfigMap-based Constructor
    MonteCarloPredictor::MonteCarloPredictor(const PrognosticsModel* m,
                                             LoadEstimator* le,
                                             GSAPConfigMap& configMap)
        : Predictor(m, le, configMap) {
        // Check for required parameters:
        // model = model to be used for simulation
        // numSamples = number of samples used for prediction
        // horizon = prediction horizon (time units)
        // processNoise = list of variance values for process noise, one for each state (zero-mean
        // assumption) event = name of event to predict inputUncertainty = specification of
        // uncertainty associated with inputParameters in this->model->inputEqn
        configMap.checkRequiredParams({NUMSAMPLES_KEY, HORIZON_KEY, PROCESSNOISE_KEY});

        // Set configuration parameters
        numSamples = static_cast<unsigned int>(std::stoul(configMap[NUMSAMPLES_KEY][0]));
        horizon = std::stoul(configMap[HORIZON_KEY][0]);

        // Set up process noise
        std::vector<std::string> processNoiseStrings = configMap[PROCESSNOISE_KEY];
        for (unsigned int i = 0; i < processNoiseStrings.size(); i++) {
            processNoise.push_back(std::stod(processNoiseStrings[i]));
        }

        events = configMap.at(EVENTS_KEY);

        log.WriteLine(LOG_INFO, MODULE_NAME, "MonteCarloPredictor created");
    }

    void MonteCarloPredictor::setModel(PrognosticsModel* value) {
        this->model = value;

        // Check that process noise was set consistent with this model
        if (processNoise.size() != this->model->getStateSize()) {
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
    Prediction MonteCarloPredictor::predict(const double time_s,
                                            const std::vector<UData>& state,
                                            ProgData& data) {
        // @todo(MD): This is setup for only a single event to predict, need to extend to multiple
        // events

        // Check that model has been set
        if (this->model == NULL) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "MonteCarloPredictor does not have a model!");
            throw ConfigurationError("MonteCarloPredictor does not have a model!");
        }

        Prediction prediction;
        prediction.events.push_back(ProgEvent());
        for (auto& output : this->model->getPredictedOutputs()) {
            DataPoint sysTrajToAdd = DataPoint();
            sysTrajToAdd.setMeta(output, "");
            prediction.sysTrajectories.push_back(sysTrajToAdd);
        }

        ProgEvent& predictionEvent = prediction.events[0];
        predictionEvent.setUncertainty(UType::Samples);
        predictionEvent.getTOE().npoints(numSamples);

        DataPoint& predictionSysTraj = prediction.sysTrajectories[0];
        predictionSysTraj.setUncertainty(UType::Samples);
        predictionSysTraj.setNumTimes(ceil(horizon / this->model->getDefaultTimeStep()));
        predictionSysTraj.setNPoints(numSamples);

        auto stateTimestamp = getLowestTimestamp(state);

// Create a random number generator if operating sequentially
#ifndef USING_OPENMP
        static std::random_device rDevice;
        static std::mt19937 generator(rDevice());
#endif

        Matrix xMean(this->model->getStateSize(), 1);

        // Assume for now that UData is mean and covariance type, and so we are assuming
        // multivariate normal NOTE: Can check UData uncertainty type to see what it is and how to
        // handle. Perhaps it would be useful to have general code to deal with this, to get samples
        // from it directly? So don't have to check within here. First step is to construct the mean
        // vector and covariance matrix from the UDatas
        Matrix Pxx(this->model->getStateSize(), this->model->getStateSize());
        for (unsigned int xIndex = 0; xIndex < this->model->getStateSize(); xIndex++) {
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
            Matrix xRandom(this->model->getStateSize(), 1);
            static std::normal_distribution<> standardDistribution(0, 1);
            for (unsigned int xIndex = 0; xIndex < this->model->getStateSize(); xIndex++) {
                xRandom[xIndex][0] = standardDistribution(generator);
            }
            // Update with mean and covariance
            xRandom = xMean + PxxChol * xRandom;
            auto x = Model::state_type(static_cast<std::vector<double>>(xRandom.col(0)));

            // 3. Simulate until time limit reached
            std::vector<double> inputParams(model->getInputParameterCount());
            unsigned int timeIndex = 0;
            std::string event = this->events[0];
            data.events[event].getTOE()[sample] = INFINITY;
            predictionEvent.setMeta(event, "");
            predictionEvent.getTOE()[sample] = INFINITY;

            for (double t_s = time_s; t_s <= time_s + horizon;
                 t_s += this->model->getDefaultTimeStep()) {
                // Get inputs for time t
                std::vector<double> loadEstimate = loadEstimator->estimateLoad(t_s, sample);
                auto u = this->model->inputEqn(t_s, inputParams, loadEstimate);

                // Check threshold at time t and set timeOfEvent if reaching for first time
                // If timeOfEvent is not set to INFINITY that means we already encountered the
                // event, and we don't want to overwrite that.
                auto& theEvent = data.events[event];
                if (this->model->thresholdEqn(t_s, x, u)) {
                    theEvent.getTOE()[sample] = t_s;
                    theEvent.getTOE().updated(stateTimestamp);

                    predictionEvent.getTOE()[sample] = t_s;
                    predictionEvent.getTOE().updated(stateTimestamp);
                    break;
                }

                // Write to system trajectory (model variables for which we are interested in
                // predicted values)
                auto z = model->getOutputVector();
                auto predictedOutput = this->model->predictedOutputEqn(t_s, x, u, z);
                for (unsigned int p = 0; p < predictedOutput.size(); p++) {
                    data.sysTrajectories[this->model->getPredictedOutputs()[p]][timeIndex][sample] =
                        z[p];
                    predictionSysTraj[timeIndex][sample] = z[p];
                }

                // Sample process noise - for now, assuming independent
                std::vector<double> noise(this->model->getStateSize());
                for (unsigned int xIndex = 0; xIndex < this->model->getStateSize(); xIndex++) {
                    std::normal_distribution<> noiseDistribution(0, sqrt(processNoise[xIndex]));
                    noise[xIndex] = noiseDistribution(generator);
                }

                // Update state for t to t+dt
                this->model->stateEqn(t_s, x, u, noise, this->model->getDefaultTimeStep());

                // Update time index
                timeIndex++;
            }
        }

        return prediction;
    }
}
