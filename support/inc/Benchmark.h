/**Benchmark header file
 *class- Benchmark
 *@author Micah Ricks
 *@Versio 0.1.0
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
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <iostream>

namespace PCOE {
  using TimeType=unsigned long long;
  TimeType const INIT_TIME=0;

  class Benchmark {
  public:
   int const INIT_TOTAL=0;
   unsigned long long const INIT_MAX=0;
   int counter;
   unsigned long long min= ULLONG_MAX;
   unsigned long long max= INIT_MAX;
   unsigned long long begin,end, elapsed_secs;
   double average,total;
   long sharedMemoryTotal=0;
   long kilo;


   // constructor
    Benchmark() {
      counter=INIT_TOTAL;
      total = INIT_TOTAL;
                }

   // adds value to running total
   void addNum(double number) {
      total += number;
      counter++;
      if(number > max)
        {max = number;}
      if(number < min)
        {min = number;}

   }

   // getter
   double getTotal()
   {
      return total;
   }

   //getter
   int getCounter()
   {
     return counter;
   }

   //getter
   double getAverage()
   {
     average= (total/counter);
     return average;
   }

   //getter
   double getMax()
   {
     return max;
   }

   //getter
   double getMin(){
     return min;
   }

   //getter
   unsigned long long getInit()
   {
     return begin;
   }

   //getter
   unsigned long long getFinal()
   {
     return end;
   }

   //getter
   unsigned long long getElapsedTime()
   {
     return elapsed_secs;
   }

   //initialize timer
   TimeType nanosecondsBegin()
   {
       using std::chrono::high_resolution_clock;
       using std::chrono::nanoseconds;
      return begin= static_cast<TimeType>(high_resolution_clock::now().time_since_epoch() / nanoseconds(1));
   }

   //end timer
   void nanosecondsEnd(TimeType begin_)
   {
       using std::chrono::high_resolution_clock;
       using std::chrono::nanoseconds;
       end= static_cast<TimeType>(high_resolution_clock::now().time_since_epoch() / nanoseconds(1));
       elapsed_secs = (end-begin_);
       addNum(elapsed_secs);
   }

   // prints the template to the console
   void printTemp()
   {
     printf("\t\t%-25s%-28s%-25s\n", "Min", "Avg", "Max");
   }

   //prints results to console
   void printScreen()
   {
     printf("%-15s%-25llu%-28f%-26llu\n\n", "Timer:", min, getAverage(),max);
   }

   //delete file content and resets template
   void clearFile()
   {
     FILE * pFile;
     pFile=fopen("benchmarkResults.txt","w");
     fclose(pFile);

     FILE * qFile;
     qFile=fopen("benchmarkResults.txt","a+");
     fprintf(qFile,"\t\t%-25s%-28s%-25s\n", "Min", "Avg", "Max");
     fclose(qFile);
   }

   //Writes results to txt file
   void writeFile()
   {
     FILE * pFile;
     pFile = fopen ("benchmarkResults.txt","a+");
     fprintf(pFile,"%-15s%-25llu%-28f%-26llu\n\n", "Timer:", min, getAverage(),max);
     //std::cout<<kilo;
     fclose (pFile);
   }

   void calRamUsage()
   {
     #ifdef __linux__
      struct rusage usage;
      getrusage(RUSAGE_SELF, &usage);
      kilo=usage.ru_maxrss;
      //int phrame=usage.ru_utime.Microseconds;
     #endif

     #if _WIN32
      #include<windows.h>
      #include<stdio.h>
      #include<tchar.h>
      #define WIDTH 7
     MEMORYSTATUSEX statex;
     statex.dwLength = sizeof (statex);
     GlobalMemoryStatusEx (&statex);
      _tprintf (TEXT("There is  %*ld percent of memory in use.\n"),WIDTH, statex.dwMemoryLoad);

     #endif


   }

  private:
  };

}


#endif // PCOE_BENCHMARK_H
