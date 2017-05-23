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

#include "CommonPrognoser.h"
#include "PrognosticsModel.h"
#include "Observer.h"
#include "Predictor.h"
#include "BenchmarkTimer.h"
namespace PCOE {
  class BenchmarkPrognoser : public CommonPrognoser{
   private:
        std::unique_ptr<PrognosticsModel> model;
        std::unique_ptr<Observer> observer;
        std::unique_ptr<Predictor> predictor;
        std::vector<std::string> inputs;
        std::vector<std::string> outputs;
        bool initialized;
        double firstTime;
        double lastTime;
        BenchmarkTimer benchmark1;
        BenchmarkTimer benchmark2;
      nanoseconds t2 = nanoseconds::zero();
   public:
        /** @brief      Benchmark Prognoser Constructor
         *  @param      config Map of config parameters from the prognoser config file
         */
        BenchmarkPrognoser(GSAPConfigMap & config);
        ~BenchmarkPrognoser();  // destructor
        void step();
  };
    extern bool regModelProg;
}
#endif  // PCOE_BENCHMARKPROGNOSER_H
