/**Benchmark header file
 *class- Benchmark
 *@author Micah Ricks
 *@Version 0.1.0
 *@brief Benchmark testing tool
 *
 *   Contact: Micah Ricks (mricks1@bulldogs.aamu.edu)
 *
 *   Generalized class that can be called when needed.
 *
 *@copyright Copyright (c) 2017 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */
#ifndef PCOE_BENCHMARK_H
#define PCOE_BENCHMARK_H

#include <climits>
#include <iomanip>
#include <chrono>
#include <iostream>

#ifdef _WIN32
#include <ctime>
#include <windows.h>
#include<tchar.h>
#else
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#endif

namespace PCOE {
  using TimeType = unsigned long long;
  TimeType const INIT_TIME = 0;

class Benchmark {
 public:
    int const INIT_TOTAL = 0;
    unsigned long long const INIT_MAX = 0;
    int counter;
    unsigned long long min = ULLONG_MAX;
    unsigned long long max = INIT_MAX;
    unsigned long long begin, end, elapsed_secs;
    double average, total;
    long sharedMemoryTotal = 0;
    long kilo, phrame;

// constructor
Benchmark() {
      counter = INIT_TOTAL;
      total = INIT_TOTAL;
    }

// adds value to running total
void addNum(double number) {
      total += number;
      counter++;
      if (number > max)
        {max = number;}
      if (number < min)
        {min = number;}
      }

// getter
double getTotal() {
      return total;
    }

// getter
int getCounter() {
     return counter;
}

// getter
double getAverage() {
     average = (total / counter);
     return average;
    }

// getter
double getMax() {
     return max;
    }

// getter
double getMin() {
     return min;
    }

// getter
unsigned long long getInit() {
     return begin;
    }

// getter
unsigned long long getFinal() {
     return end;
    }

// getter
unsigned long long getElapsedTime() {
     return elapsed_secs;
    }

// initialize timer
TimeType nanosecondsBegin() {
     #ifdef __linux__
       using std::chrono::high_resolution_clock;
       using std::chrono::nanoseconds;
      return begin = static_cast<TimeType>(
        high_resolution_clock::now().time_since_epoch() / nanoseconds(1));
     #endif

     #if _WIN32
  FILETIME createTime, exitTime, kernelTime, userTime;
if (GetProcessTimes( GetCurrentProcess( ),
 &createTime, &exitTime, &kernelTime, &userTime) != -1) {
    SYSTEMTIME userSystemTime;
    if ( FileTimeToSystemTime( &userTime, &userSystemTime ) != -1 )
      return begin = (TimeType)userSystemTime.wHour * 3600.0 +
      (TimeType)userSystemTime.wMinute * 60.0 +
      (TimeType)userSystemTime.wSecond +
      (TimeType)userSystemTime.wMilliseconds / 1000.0;
    }
#endif
}
// end timer
void nanosecondsEnd(TimeType begin_) {
#ifdef __linux__
using std::chrono::high_resolution_clock;
using std::chrono::nanoseconds;
end = static_cast<TimeType>(
  high_resolution_clock::now().time_since_epoch() / nanoseconds(1));
elapsed_secs = (end - begin_);
addNum(elapsed_secs);
#endif

#if _WIN32
FILETIME createTime2;
FILETIME exitTime2;
FILETIME kernelTime2;
FILETIME userTime2;
if (GetProcessTimes (GetCurrentProcess(),
 &createTime2, &exitTime2, &kernelTime2, &userTime2) != -1) {
       SYSTEMTIME userSystemTime2;
       if (FileTimeToSystemTime(&userTime2, &userSystemTime2) != -1)
       end = (TimeType)userSystemTime2.wHour * 3600.0 +
       (TimeType)userSystemTime2.wMinute * 60.0 +
       (TimeType)userSystemTime2.wSecond +
       (TimeType)userSystemTime2.wMilliseconds / 1000.0;
       elapsed_secs = (end - begin_);
       addNum(elapsed_secs);
}
     #endif
}

// prints the template to the console
void printTemp() {
     printf("\t\t%-25s%-28s%-25s\n", "Min", "Avg", "Max");
}

// prints results to console
void printScreen() {
     printf("%-15s%-25llu%-28f%-26llu\n\n", "Timer:", min, getAverage(), max);
}

// delete file content and resets template
void clearFile() {
     FILE * pFile;
     pFile = fopen("benchmarkResults.txt", "w");
     fclose(pFile);

     FILE * qFile;
     qFile = fopen("benchmarkResults.txt", "a+");
     fprintf(qFile, "\t\t%-25s%-28s%-25s\n", "Min", "Avg", "Max");
     fclose(qFile);
                     }

// Writes results to txt file
void writeFile() {
     FILE * pFile;
     pFile = fopen("benchmarkResults.txt", "a+");
     fprintf(pFile, "%-15s%-25llu%-28f%-26llu\n\n", "Timer:", min,
     getAverage(), max);
     fclose(pFile);
                   }

void calRamUsage() {
     #ifdef __linux__
      struct rusage usage;
      getrusage(RUSAGE_SELF, &usage);
      kilo = usage.ru_maxrss;
      phrame = usage.ru_utime.tv_usec;
     #endif

     #if _WIN32
     #define WIDTH 7
     MEMORYSTATUSEX statex;
     statex.dwLength = sizeof(statex);
     GlobalMemoryStatusEx(&statex);
      _tprintf(TEXT("There is %*ld percent of memory in use.\n"),
      WIDTH, statex.dwMemoryLoad);
      #endif
    }

 private:
                };
}       // namespace PCOE
#endif  // PCOE_BENCHMARK_H
