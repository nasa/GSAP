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

#include <vector>
#include <iostream>
#include <functional>
#include <random>
#include <list>
#include <limits>

#include "ParticleSwarmOptimizer.h"

namespace PCOE {
    // Default constructor
    ParticleSwarmOptimizer::ParticleSwarmOptimizer(): Optimizer(), numIterations(0), maxNumIterations(1000), numSamples(50) {
        //...
    }
    
    // Evaluate stopping criterion
    bool ParticleSwarmOptimizer::stoppingCriterion() {
        // First check that improvement in e is significant
        if (fabs(best.e-lastBest.e)/lastBest.e < 0.0001) {
            return true;
        }
        else {
            lastBest = best;
        }
        // Else check number of iterations
        return numIterations >= maxNumIterations;
    }
    
    // Minimization algorithm
    // Implements particle swarm optimization
    double ParticleSwarmOptimizer::minimize(const std::vector<double> & x0,
                                    std::function<double(std::vector<double>)> evaluate,
                                    std::vector<double> & xOptimal) {
        // Initialize solution
        numIterations = 0;
        xOptimal = x0;
        
        // Create random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distribution(-1, 1);
        std::uniform_real_distribution<> U(0,1);
        
        // Create initial solutions
        std::vector<solution> solutions(numSamples);
        for (unsigned int i=0; i<numSamples; i++) {
            std::vector<double> x = x0;
            std::vector<double> v = x0;
            for (unsigned int j=0; j<x0.size(); j++) {
                x[j] += distribution(gen)*0.1*x0[j];
                v[j] = distribution(gen)*0.1*x0[j];
                // TODO: Initial distributions (bounds of search space) should be specifiable.
            }
            // Create solution
            solutions[i].x = x;
            solutions[i].e = evaluate(x);
            if (std::isnan(solutions[i].e)) {
                solutions[i].e = std::numeric_limits<double>::infinity();
            }
            solutions[i].eBest =solutions[i].e;
            solutions[i].v = v;
            solutions[i].xBest = x;
        }
        
        // Find best solution
        best = solutions[0];
        lastBest = best;
        for (std::vector<solution>::iterator i=solutions.begin(); i!=solutions.end(); i++) {
            if (i->e < best.e) {
                best = *i;
            }
        }
        
        // Algorithm parameters
        // TODO: Make these tunable (below values can be default, but research this a little more)
        double omega = 0.7;
        double phig = 1.2;
        double phip = 1.2;
        
        // Loop until stopping criterion
        while (!stoppingCriterion()) {
            numIterations++;
            // For each sample
            for (unsigned int i=0; i<numSamples; i++) {
                std::vector<double> x = x0;
                // For each dimension
                for (unsigned int j=0; j<x0.size(); j++) {
                    double rp = U(gen);
                    double rg = U(gen);
                    // Update velocity
                    solutions[i].v[j] = omega*solutions[i].v[j] + phip*rp*(solutions[i].xBest[j]-solutions[i].x[j]) + phig*rg*(best.x[j]-solutions[i].x[j]);
                    // Update position
                    solutions[i].x[j] += solutions[i].v[j];
                }
                // Evaluate new solution
                solutions[i].e = evaluate(solutions[i].x);
                if (std::isnan(solutions[i].e)) {
                    solutions[i].e = std::numeric_limits<double>::infinity();
                }
                // Check if best local solution
                if (solutions[i].e < solutions[i].eBest) {
                    // Update local best
                    solutions[i].xBest = solutions[i].x;
                    solutions[i].eBest = solutions[i].e;
                    // Check if best global
                    if (solutions[i].e < best.e) {
                        best = solutions[i];
                    }
                }
            }
            //std::cout << numIterations << ": e is " << best.e << std::endl;
        }
        
        xOptimal = best.x;
        return best.e;
    }
}
