//
//  MonteCarloSampler.h
//  GSAP
//
//  Created by Teubert, Christopher (ARC-TI)[SGT, INC] on 10/20/17.
//
//

#ifndef MonteCarloSampler_h
#define MonteCarloSampler_h
#include <random>
#include <chrono>

#include "Sampler.h"
#include "Matrix.h"

namespace PCOE {
    // Including class prototype to avoid including header
    class GSAPConfigMap;
    
    class MonteCarloSampler : public Sampler {
    public:
        MonteCarloSampler(GSAPConfigMap & configMap);
        
        void sample(const ustate_vec & state, const uload_vec & load, const size_t nSamples, std::vector<state_vec> & stateSamples, std::vector<load_vec> & loadSamples) final;
    private:
        std::random_device rDevice;
        std::mt19937 generator;
        std::normal_distribution<> standardDistribution;
        
        Matrix xMean, xRandom, Pxx;
    };
}

#endif /* MonteCarloSampler_h */
