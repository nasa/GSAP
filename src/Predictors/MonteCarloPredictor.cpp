// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <random>
#include <string>
#include <vector>

#include "Contracts.h"
#include "Exceptions.h"
#include "Matrix.h"
#include "Predictors/MonteCarloPredictor.h"
#include "ThreadSafeLog.h"

namespace PCOE {
    static const Log& log = Log::Instance();

    // Configuration Keys
    const std::string PROCESSNOISE_KEY = "Model.ProcessNoise";
    const std::string NUMSAMPLES_KEY = "Predictor.SampleCount";
    const std::string HORIZON_KEY = "Predictor.Horizon";

    // Other string constants
    const std::string MODULE_NAME = "PRED-MC";

    MonteCarloPredictor::MonteCarloPredictor(const PrognosticsModel& m,
                                             LoadEstimator& le,
                                             const TrajectoryService& trajService,
                                             const ConfigMap& config)
        : Predictor(m, le, trajService, config) {
        // Check for required parameters:
        // model = model to be used for simulation
        // sampleCount = number of samples used for prediction
        // horizon = prediction horizon (time units)
        // processNoise = list of variance values for process noise, one for each state (zero-mean
        // assumption) event = name of event to predict inputUncertainty = specification of
        // uncertainty associated with inputParameters in model.inputEqn
        requireKeys(config, {NUMSAMPLES_KEY, HORIZON_KEY, PROCESSNOISE_KEY});

        // Set configuration parameters
        sampleCount = static_cast<unsigned int>(config.getUInt64(NUMSAMPLES_KEY));
        horizon = config.getUInt64(HORIZON_KEY);

        // Set up process noise
        std::vector<std::string> processNoiseStrings = config.getVector(PROCESSNOISE_KEY);
        for (unsigned int i = 0; i < processNoiseStrings.size(); i++) {
            processNoise.push_back(std::stod(processNoiseStrings[i]));
        }

        Ensure(horizon > 0, "Non-positive horizon");
        Ensure(sampleCount > 0, "Non-positive sample count");
        Ensure(processNoise.size() == model.getStateSize(),
               "Process noise size not equal to model state size");
        log.WriteLine(LOG_INFO, MODULE_NAME, "MonteCarloPredictor created");
    }

    UData::time_ticks getLowestTimestamp(const std::vector<UData>& data) {
        UData::time_ticks result = std::numeric_limits<UData::time_ticks>::max();
        for (const UData& entry : data) {
            if (entry.updated() < result) {
                result = entry.updated();
            }
        }
        return result;
    }

    Prediction MonteCarloPredictor::predict(double time_s, const std::vector<UData>& state) {
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Starting prediction");
        // TODO (MD): This is setup for only a single event to predict, need to extend to multiple
        //            events

        // TODO (JW): Contract has been changed so that this is checked in the
        //            constructor. Shouldn't be possible here.

        auto savePts = savePointProvider.getSavePts();

        UData eventToe(UType::Samples);
        eventToe.npoints(sampleCount);
        std::vector<UData> eventState(savePts.size());
        for (auto&& elem : eventState) {
            elem.uncertainty(UType::Samples);
            elem.npoints(sampleCount);
        }
        std::vector<DataPoint> sysTrajectories(model.getPredictedOutputs().size());
        for (auto& trajectory : sysTrajectories) {
            trajectory.setUncertainty(UType::Samples);
            trajectory.setNumTimes(savePts.size());
            trajectory.setNPoints(sampleCount);
        }
        auto stateTimestamp = getLowestTimestamp(state);

// Create a random number generator if operating sequentially
#ifndef USING_OPENMP
        static std::random_device rDevice;
        static std::mt19937 generator(rDevice());
#endif

        Matrix xMean(model.getStateSize(), 1);

        // Assume for now that UData is mean and covariance type, and so we are assuming
        // multivariate normal NOTE: Can check UData uncertainty type to see what it is and how to
        // handle. Perhaps it would be useful to have general code to deal with this, to get samples
        // from it directly? So don't have to check within here. First step is to construct the mean
        // vector and covariance matrix from the UDatas
        Matrix Pxx(model.getStateSize(), model.getStateSize());
        for (unsigned int xIndex = 0; xIndex < model.getStateSize(); xIndex++) {
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
#pragma omp parallel for shared(data, loadEstimatorSavePts)
        for (unsigned int sample = 0; sample < sampleCount; sample++) {
            log.FormatLine(LOG_TRACE, MODULE_NAME, "Prediction sample %ull", sample);
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
            Matrix xRandom(model.getStateSize(), 1);
            static std::normal_distribution<> standardDistribution(0, 1);
            for (unsigned int xIndex = 0; xIndex < model.getStateSize(); xIndex++) {
                xRandom[xIndex][0] = standardDistribution(generator);
            }
            // Update with mean and covariance
            xRandom = xMean + PxxChol * xRandom;
            auto x = SystemModel::state_type(static_cast<std::vector<double>>(xRandom.col(0)));

            // 3. Simulate until time limit reached
            std::vector<double> inputParams(model.getInputParameterCount());
            eventToe[sample] = INFINITY;

            std::vector<double>::size_type savePtIndex = 0;
            double timeOfCurrentSavePt = std::numeric_limits<double>::infinity();
            auto currentSavePt = savePts.begin();
            if (currentSavePt != savePts.end()) {
                timeOfCurrentSavePt = seconds(*currentSavePt);
            }

            for (double t_s = time_s; t_s <= time_s + horizon; t_s += model.getDefaultTimeStep()) {
                // Get inputs for time t
                // TODO (JW): Consider per-sample load estimator
                PrognosticsModel::input_type loadEstimate =
                    static_cast<PrognosticsModel::input_type>(loadEstimator.estimateLoad(t_s));

                // Check threshold at time t and set timeOfEvent if reaching for first time
                // If timeOfEvent is not set to INFINITY that means we already encountered the
                // event, and we don't want to overwrite that.
                if (model.thresholdEqn(t_s, x)) {
                    eventToe[sample] = t_s;
                    eventToe.updated(stateTimestamp);
                    break;
                }

                if (savePtIndex < savePts.size() && t_s > timeOfCurrentSavePt) {
                    // Write to system trajectory (model variables for which we are interested in
                    // predicted values)
                    ++currentSavePt;
                    if (currentSavePt != savePts.end()) {
                        timeOfCurrentSavePt = seconds(*currentSavePt);
                    }

                    auto predictedOutput = model.predictedOutputEqn(t_s, x);

                    for (unsigned int p = 0; p < predictedOutput.size(); p++) {
                        sysTrajectories[p][savePtIndex][sample] = predictedOutput[p];
                    }

                    // Write to eventState property
                    eventState[savePtIndex][sample] = model.eventStateEqn(x);

                    // Update time index
                    savePtIndex++;
                }

                // Sample process noise - for now, assuming independent
                std::vector<double> noise(model.getStateSize());
                for (unsigned int xIndex = 0; xIndex < model.getStateSize(); xIndex++) {
                    std::normal_distribution<> noiseDistribution(0, sqrt(processNoise[xIndex]));
                    noise[xIndex] = noiseDistribution(generator);
                }

                // Update state for t to t+dt
                x = model.stateEqn(t_s, x, loadEstimate, noise, model.getDefaultTimeStep());
            }
        }

        log.WriteLine(LOG_TRACE, MODULE_NAME, "Prediction complete");
        return Prediction({ProgEvent(model.getEvents()[0],
                                     std::move(eventState),
                                     std::move(eventToe))},
                          std::move(sysTrajectories));
    }
}
