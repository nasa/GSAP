/**  Benchmark Prognoser - Header
*   @class     BenchmarkPrognoser BenchmarkPrognoser.h
*
*   @brief     Benchmark Prognoser Class
*
*   BenchmarkPrognoser class.
*
*   @author    Micah Ricks
*   @version   0.1.0
*
*
*      Contact: Micah Ricks (mricks1@bulldogs.aamu.edu)
*      Created: March 16, 2016
*
*   @copyright Copyright (c) 2017 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/
#ifndef PCOE_BENCHMARKPROGNOSER_H
#define PCOE_BENCHMARKPROGNOSER_H

#include "ModelBasedPrognoser.h"
#include "PrognosticsModel.h"
#include "Observer.h"
#include "Predictor.h"
#include "BenchmarkTimer.h"
namespace PCOE {
  class BenchmarkPrognoser : public ModelBasedPrognoser {
   private:
        BenchmarkTimer benchmark1;
   public:
        /** @brief      Benchmark Prognoser Constructor
         *  @param      config Map of config parameters from the prognoser config file
         */
        BenchmarkPrognoser(GSAPConfigMap & config);
        ~BenchmarkPrognoser();  // destructor
        void step();
  };
}
#endif  // PCOE_BENCHMARKPROGNOSER_H
