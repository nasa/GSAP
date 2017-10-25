//
//  Sampler.h
//  GSAP
//
//  Created by Teubert, Christopher (ARC-TI)[SGT, INC] on 10/20/17.
//
//

#ifndef Sampler_h
#define Sampler_h

#include <chrono>
#include <vector>

#include "ProgData.h"
#include "UData.h"
#include "ThreadSafeLog.h"

namespace PCOE {
    using ms_rep = std::chrono::milliseconds::rep;
    using ustate_vec = std::vector<UData>;
    using state_vec = std::vector<double>;
    using load_profile = std::map<ms_rep, double>;
    using load_vec = std::vector<load_profile>;
    using uload_profile = std::map<ms_rep, UData>;
    using uload_vec = std::vector<uload_profile>;
    
    class Sampler {
     public:
        Sampler(): log(Log::Instance()) {}
        
        /** @brief  sample from state and futureLoading
         *  @param  state   State distributions
         *  @param  load    Future Loading Distributions
         *  @param  stateSamples    Samples of States - filled by sample() (#samples x #vars)
         *  @param  loadSamples     Samples of future Loading - filled by sample() (#samples x #vars)
         **/
        virtual void sample(const ustate_vec & state, const uload_vec & load, const size_t nSamples, std::vector<state_vec> & stateSamples, std::vector<load_vec> & loadSamples) = 0;
        
        /** @brief  postprocess results of of prediction
         *  @param  result  Result of prediction
         *  @return If resampling should occur (used for adaptive sampling methods)
         **/
        virtual bool postProcess(ProgData & result) { return false;};
        
    protected:
        Log &log;       ///> Logger (Defined in ThreadSafeLog.h)
    };
}

#endif /* Sampler_h */
