/**  Model-based Prognoser - Body
 *   @class     BenchmarkPrognoser BenchmarkPrognoser.h Benchmark.h
 *
 *   @brief     Benchmark Prognoser Class
 *
 *   BenchmarkPrognoser class.
 *
 *   @author    Micah Ricks
 *   @version   1.1.0
 *
 *
 *      Contact: Christopher Teubert (christopher.a.teubert@nasa.gov)
 *      Created: January 31, 2017
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <iostream>

#include "BenchmarkPrognoser.h"
#include "BenchmarkTimer.h"
#include "ConfigMap.h"

namespace PCOE {
    // Configuration Keys

    BenchmarkPrognoser::BenchmarkPrognoser(ConfigMap& configMap)
        : ModelBasedPrognoser(configMap) {}

    void BenchmarkPrognoser::step() {
        benchmark1.start();
        ModelBasedPrognoser::step();
        benchmark1.stop();
    }

    // destructor
    BenchmarkPrognoser::~BenchmarkPrognoser() {
        std::cout << "Runtime: [";
        std::cout << benchmark1.getMinStepTime().count();
        std::cout << ", ";
        std::cout << benchmark1.getAveStepTime().count();
        std::cout << ", ";
        std::cout << benchmark1.getMaxStepTime().count();
        std::cout << "] ns" << std::endl;
    }
} // namespace PCOE
