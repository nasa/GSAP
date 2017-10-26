//
//  ModelBasedPredictor.h
//  TestMCSampler
//
//  Created by Teubert, Christopher (ARC-TI)[SGT, INC] on 10/26/17.
//  Copyright © 2017 NASA. All rights reserved.
//

#ifndef ModelBasedPredictor_h
#define ModelBasedPredictor_h

#include <vector>
#include <string>

#include "Model.h"
#include "Predictor2.h"
#include "GSAPConfigMap.h"

namespace PCOE {
    class ModelBasedPredictor : public Predictor2 {
    public:
        /** @brief    Constructor for a MonteCarloPredictor based on a configMap
         *   @param  configMap Configuration map specifying predictor parameters
         **/
        explicit ModelBasedPredictor(GSAPConfigMap & configMap);
        
        /** @brief Set model pointer
         *   @param model given model pointer
         **/
        void setModel(PrognosticsModel * model);
        
        /** @brief    Predict function for a Predictor
         *   @param    tP Time of prediction
         *    @param    state state of system at time of prediction
         *   @param  data ProgData object, in which prediction results are stored
         **/
        void predict(const double tP, const std::vector<state_vec> & stateSamples, const std::vector<load_vec> & loadSamples, ProgData & data);
        
    private:
        std::vector<double> processNoise;  // variance vector (zero-mean assumed)
        std::string event;                 // name of event to predict
    };
    
}
#endif /* ModelBasedPredictor_h */
