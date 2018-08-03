/**  Benchmark Prognoser - Header
 *   @class     BenchmarkPrognoser BenchmarkPrognoser.h
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
 *      Created: March 16, 2016
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */
#ifndef PCOE_BENCHMARKPROGNOSER_H
#define PCOE_BENCHMARKPROGNOSER_H

#include "BenchmarkTimer.h"
#include "ModelBasedPrognoser.h"
#include "Observers/Observer.h"
#include "Predictors/Predictor.h"
#include "PrognosticsModel.h"

namespace PCOE {
    class BenchmarkPrognoser : public ModelBasedPrognoser {
    private:
        BenchmarkTimer benchmark1;

    public:
        /**
         * @brief Benchmark Prognoser Constructor
         * @param config Map of config parameters from the prognoser config file
         */
        BenchmarkPrognoser(ConfigMap& config);
        ~BenchmarkPrognoser(); // destructor
        void step();
    };
}
#endif // PCOE_BENCHMARKPROGNOSER_H
