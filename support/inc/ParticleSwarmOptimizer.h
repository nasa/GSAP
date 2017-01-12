/**  ParticleSwarmOptimizer - Header
 *   @file      ParticleSwarmOptimizer Class
 *   @ingroup   GPIC++
 *   @ingroup    Predictors
 *
 *   @brief     ParticleSwarmOptimizer Class - Class defining the ParticleSwarmOptimizer
 *
 *   @author    Matthew Daigle
 *   @version   0.1.0
 *
 *   @pre       N/A
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: March 22, 2016
 *
 *   @copyright Copyright (c) 2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_ParticleSwarmOptimizer_H
#define PCOE_ParticleSwarmOptimizer_H

#include <vector>
#include <functional>

#include "Optimizer.h"
#include "GSAPConfigMap.h"

namespace PCOE {
    // Minimization problem solution struct
    struct solution {
        std::vector<double> x;  // value
        double e;               // error
        std::vector<double> v;  // velocity
        std::vector<double> xBest;  // best (local) value
        double eBest;  // error for best (local) value
    };
    
    class ParticleSwarmOptimizer final : public PCOE::Optimizer {
    private:
        // Number of iterations the minimize function has gone through
        unsigned int numIterations;
        // Max number
        unsigned int maxNumIterations;
        // Number of samples for optimization
        unsigned int numSamples;
        // best solution
        solution best;
        // previous best solution
        solution lastBest;
    public:
        // Default constructor
        ParticleSwarmOptimizer();
        
        /** @brief    Constructor for a ParticleSwarmOptimizer based on a configMap
        *   @param  configMap Configuration map specifying predictor parameters
        **/
        explicit ParticleSwarmOptimizer(GSAPConfigMap & configMap);
        
        /** @brief    Find x that minimizes evaluation function given starting x0
         *  @param    x0 Starting point of minimization
         *  @param    evaluate Solution evaluation function
         *  @param    xOptimal Optimized value
         **/
        double minimize(const std::vector<double> & x0,
                      std::function<double(std::vector<double>)> evaluate,
                      std::vector<double> & xOptimal);
        
        /** @brief    Determine whether optimization is finished
         **/
        bool stoppingCriterion();
        
    };
}

#endif  // PCOE_ParticleSwarmOptimizer_H
