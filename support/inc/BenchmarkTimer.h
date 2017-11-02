/**Benchmark Timer
 *  class- Benchmark Timer Header File
 *  @author Micah Ricks, Chris Teubert
 *  @Version 1.0.0
 *  @brief Timer Class for benchmarking scripts. Use start and stop functions to run timer. Class defined in header  for ease of use.
 *
 *   Contact: Chris Teubert (christopher.a.teubert@nasa.gov)
 *
 *  @copyright Copyright (c) 2017 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_BENCHMARKTIMER_H
#define PCOE_BENCHMARKTIMER_H

#include <stdio.h>
#include <chrono>

// Ram calc
#ifdef _WIN32
#else
#include <sys/time.h>
#include <sys/resource.h>
#endif

namespace PCOE {
    using Steps = size_t;
    using std::chrono::nanoseconds;
    
    class BenchmarkTimer {
    public:
        /** getTimeSinceEpoch
         *  @return     The number of nanoseconds since epoch.
         *  @example    
         *      using std::chrono::seconds;
         *      nanoseconds time = BenchmarkTimer::getTimeSinceEpoch();
         *      printf("Time since epoch in seconds is %lld\n", time/seconds(1));
         **/
        inline static nanoseconds getTimeSinceEpoch() {
            using std::chrono::high_resolution_clock;
            return static_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch() / nanoseconds(1));
        }
        
        /** getRam
         *  @return     Kilo-bytes of ram used
         *  @note       Does not work for windows
         **/
        static long getRam() {
#ifdef _WIN32
            printf("Currently not supported for windows\n");
            return -1l;
#else
            struct rusage usage;
            getrusage(RUSAGE_SELF, &usage);
            return usage.ru_maxrss;
#endif
        }
        
        /** start
         *  @brief      Start timer for step. Calling more than once will reset
         *  @see        stop
         **/
        inline void start() {
            begin = getTimeSinceEpoch();
        }
        
        /** stop
         *  @brief      Stop timer for step, after step has ben called
         *  @return     Number of nanoseconds since start() was last called
         *  @note       One start-stop pair constitues a "step"
         *  @see        start
         **/
        nanoseconds stop() {
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
        
        /** isrunning
         *  @return     If timer is running (start has been called since the last stop call).
         *  @see        start, stop
         **/
        inline bool isRunning() const {
            return (begin != nanoseconds::zero());
        }
        
        /** getTotalRunTime
         *  @return     The total run time in nanoseconds. The sum of all start-stop step times
         **/
        inline nanoseconds getTotalRunTime() const { // In nsec
            return runTime;
        }
        
        /** getAveStepTime
         *  @return     The average step time in nanoseconds
         **/
        inline nanoseconds getAveStepTime() const {
            if (steps <= 0) {
                return std::chrono::nanoseconds(-1);
            }
            return runTime/steps;
        }
        
        /** getMinStepTime
         *  @return     The minimum step time in nanoseconds
         **/
        inline nanoseconds getMinStepTime() const {
            return minStep;
        }
        
        /** getMaxStepTime
         *  @return     The maximum step time in nanoseconds
         **/
        inline nanoseconds getMaxStepTime() const {
            return maxStep;
        }
        
        /** getNSteps
         *  @return     The number of steps completed
         **/
        inline Steps getNSteps() const {
            return steps;
        }
        
    private:
        nanoseconds begin = nanoseconds::zero();   // Beginning time for step
        nanoseconds runTime = nanoseconds::zero(); // Total Runtime
        nanoseconds maxStep = nanoseconds::min();  // Running maximum steptime
        nanoseconds minStep = nanoseconds::max();  // Running minimum steptime
        Steps steps = 0;                           // Number of steps completed
    }; // End Class
} // end namespace PCOE
#endif  // PCOE_BENCHMARKTIMER_H
