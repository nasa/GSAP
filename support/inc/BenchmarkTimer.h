/**Benchmark header file
 *  class- Benchmark
 *  @author Micah Ricks, Chris Teubert
 *  @Version 0.1.0
 *  @brief Benchmark testing tool
 *
 *   Contact: Micah Ricks (mricks1@bulldogs.aamu.edu)
 *   Generalized class that can be called when needed.
 *
 *  @copyright Copyright (c) 2017 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_BENCHMARKTIMER_H
#define PCOE_BENCHMARKTIMER_H

#include <stdio.h>

#include <chrono>
#include <sys/time.h>
#include <sys/resource.h>

namespace PCOE {
    using Steps = size_t;
    using std::chrono::nanoseconds;
    
    class BenchmarkTimer {
    public:
        inline static nanoseconds getTimeSinceEpoch() {
            using std::chrono::high_resolution_clock;
            return static_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch() / nanoseconds(1));
        }
        
#if _WIN32
        static void calRam() {
            //#define WIDTH 7
            //        MEMORYSTATUSEX statex;
            //        statex.dwLength = sizeof(statex);
            //        GlobalMemoryStatusEx(&statex);
            //        _tprintf(TEXT("There is %*ld percent of memory in use.\n"),
            //                 WIDTH, statex.dwMemoryLoad);
            printf("Currently not supported for windows\n")
#else
        static struct rusage calRam() {
            struct rusage usage;
            getrusage(RUSAGE_SELF, &usage);
            return usage;
#endif
        }
        
        inline void tic() {
            begin = getTimeSinceEpoch();
        }
        
        nanoseconds toc() {
            nanoseconds stepTime = nanoseconds::zero();
            if (begin != nanoseconds::zero()) {
                stepTime = (getTimeSinceEpoch() - begin);
                runTime += stepTime;
                begin = nanoseconds::zero();
                steps++;
                if (stepTime > maxStep) {
                    maxStep = stepTime;
                }
                if (stepTime < minStep) {
                    minStep = stepTime;
                }
            }
            return stepTime;
        }
        
        inline bool isRunning() const {
            return (begin != nanoseconds::zero());
        }
        
        inline nanoseconds getTotalRunTime() const { // In nsec
            return runTime;
        }
        
        inline nanoseconds getAveStepTime() const {
            return runTime/steps;
        }
        
        inline nanoseconds getMinStepTime() const {
            return minStep;
        }
        
        inline nanoseconds getMaxStepTime() const {
            return maxStep;
        }
        
        inline Steps getNSteps() const {
            return steps;
        }
        
    private:
        nanoseconds begin = nanoseconds::zero();
        nanoseconds runTime = nanoseconds::zero();
        nanoseconds maxStep = nanoseconds::min();
        nanoseconds minStep = nanoseconds::max();
        
        Steps steps = 0;
    }; // End Class
}       // namespace PCOE
#endif  // PCOE_BENCHMARKTIMER_H
