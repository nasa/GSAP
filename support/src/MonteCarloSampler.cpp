//
//  MonteCarloSampler.cpp
//  GSAP
//
//  Created by Teubert, Christopher (ARC-TI)[SGT, INC] on 10/20/17.
//
//

#include "MonteCarloSampler.h"
#include "GSAPConfigMap.h"

const std::string NUMBER_OF_STATES_KEY = "nStates";
const std::string FILE_NAME = "MonteCarloSampler";

namespace PCOE {
    MonteCarloSampler::MonteCarloSampler(GSAPConfigMap & config): Sampler(),
      generator(rDevice()),
      standardDistribution(0, 1) {
        if (!config.includes(NUMBER_OF_STATES_KEY)) {
            log.FormatLine(LOG_ERROR, FILE_NAME, "Error: Missing key %s", NUMBER_OF_STATES_KEY.c_str());
            throw "Error: Missing Key";
        }
        
        unsigned long long nStates = std::stoull(config[NUMBER_OF_STATES_KEY][0]);
        
        xMean.resize(nStates, 1);
        Pxx.resize(nStates, nStates);
        xRandom.resize(nStates, 1);
        
    }
    
    void MonteCarloSampler::sample(const ustate_vec & state, const uload_vec & load, const size_t nSamples, std::vector<state_vec> & stateSamples, std::vector<load_vec> & loadSamples) {
        const auto nStates = xMean.rows();
        for (unsigned int xIndex = 0; xIndex < nStates; xIndex++) {
            xMean[xIndex][0] = state[xIndex][MEAN];
            Pxx.row(xIndex, state[xIndex].getVec(COVAR(0)));
        }
        auto PxxChol = Pxx.chol();
        //std::vector<double> inputParameters(load.size()+1);
        const auto nLoads = load.size();
        loadSamples.resize(nSamples, std::vector<double>(nLoads+1));
        stateSamples.resize(nSamples);

        std::vector<std::normal_distribution<> > loadDists(nLoads);
        for (size_t loadIndex = 0; loadIndex < load.size(); loadIndex++) {
            // Create distribution for this input parameter
            loadDists[loadIndex] = std::move(std::normal_distribution<>(load[loadIndex][MEAN],load[loadIndex][SD]));
        }
        
        // For each sample
        for (size_t sample = 0; sample < nSamples; sample++) {
            // 1. Sample the state
            // Create state vector
            // Now we have mean vector (x) and covariance matrix (Pxx). We can use that to sample a realization of the state.
            // I need to generate a vector of random numbers, size of the state vector
            for (unsigned int xIndex = 0; xIndex < nStates; xIndex++) {
                xRandom[xIndex][0] = standardDistribution(generator);
            }
            
            // Update with mean and covariance
            xRandom = xMean + PxxChol*xRandom;
            stateSamples[sample] = static_cast<std::vector<double>>(xRandom.col(0));
            
            // 2. Sample the input parameters
            // For now, hard-code and assume Gaussian, but these should be specified somehow in the configMap
            // Assuming that for each input parameter, we have specified mean and standard deviation
            // We have a list of pairs (mean,stddev) for each input parameter
            // The order must correspond to the order of the input parameters in the model:
            //   mean_ip1, stddev_ip1, mean_ip2, stddev_ip2, ...
            for (unsigned int ipIndex = 0; ipIndex < nLoads; ipIndex++) {
                // Sample a value for distribution
                loadSamples[sample][ipIndex] = loadDists[ipIndex](generator);
            }
        }
    }
}
