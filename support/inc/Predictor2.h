//
//  Predictor2.h
//  TestMCSampler
//
//  Created by Teubert, Christopher (ARC-TI)[SGT, INC] on 10/26/17.
//  Copyright © 2017 NASA. All rights reserved.
//

#ifndef Predictor2_h
#define Predictor2_h

#include <vector>
#include <string>
#include <chrono>
#include <map>

#include "Model.h"
#include "PrognosticsModel.h"
#include "ProgData.h"
#include "ThreadSafeLog.h"

namespace PCOE {
    using ms_rep = std::chrono::milliseconds::rep;
    using state_vec = std::vector<double>;
    using load_profile = std::map<ms_rep, double>;
    using load_vec = std::vector<load_profile>;
    
    class Predictor2 {
    public:
        Predictor2() : pModel(NULL), log(Log::Instance()) {}
        
        virtual ~Predictor2() = default;
        
        /** @brief Set model pointer
         * @param model given model pointer
         **/
        virtual void setModel(PrognosticsModel *model) = 0;
        /** @brief    Predict future events and values of system variables
         *   @param    tP Time of prediction
         *    @param    state state of system at time of prediction
         *   @param  data ProgData object, in which prediction results are stored
         **/
        virtual void predict(const double tP, const std::vector<state_vec> & stateSamples, const std::vector<load_vec> & loadSamples, ProgData & data) = 0;
        
    protected:
        PrognosticsModel * pModel;  // model used for prediction
        double horizon;            // time span of prediction
        std::vector<std::string> predictedOutputs;  // list of variables for which to compute future values of
        Log &log;  ///> Logger (Defined in ThreadSafeLog.h)
    };
}

#endif /* Predictor2_h */
