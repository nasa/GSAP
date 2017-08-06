/**  Model-based Prognoser - Body
 *   @class     BenchmarkPrognoser BenchmarkPrognoser.h Benchmark.h
 *
 *   @brief     Benchmark Prognoser Class
 *
 *   BenchmarkPrognoser class.
 *
 *   @author    Micah Ricks
 *   @version   0.1.0
 *
 *
 *      Contact: Christopher Teubert (christopher.a.teubert@nasa.gov)
 *      Created: January 31, 2017
 *
 *   @copyright Copyright (c) 2017 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <stdio.h>

#include "BenchmarkTimer.h"
#include "BenchmarkPrognoser.h"
#include "GSAPConfigMap.h"

namespace PCOE {
    // Configuration Keys

    BenchmarkPrognoser::BenchmarkPrognoser(GSAPConfigMap & configMap) : ModelBasedPrognoser(configMap) {

    }

    void BenchmarkPrognoser::step() {
        benchmark1.start();
        ModelBasedPrognoser::step();
        benchmark1.stop();
    }

    // destructor
    BenchmarkPrognoser::~BenchmarkPrognoser() {
        printf("Runtime: [%lld, %lld, %lld] ns\n", benchmark1.getMinStepTime()/nanoseconds(1), benchmark1.getAveStepTime()/nanoseconds(1), benchmark1.getMaxStepTime()/nanoseconds(1));
    }
}   // namespace PCOE
