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
#include <climits>
#include <iomanip>
#include <chrono>

namespace PCOE {

  class Benchmark {
  public:
   int const INIT_TOTAL=0;
   unsigned long long const INIT_MAX=0;
   int counter;
   unsigned long long min= ULLONG_MAX;
   unsigned long long max= INIT_MAX;
   unsigned long long begin,end, elapsed_secs;
   double average,total;
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
   void nanosecondsBegin()
   {
       using std::chrono::high_resolution_clock;
       using std::chrono::nanoseconds;
       begin= static_cast<unsigned long long>(high_resolution_clock::now().time_since_epoch() / nanoseconds(1));
   }

   //end timer
   void nanosecondsEnd()
   {
       using std::chrono::high_resolution_clock;
       using std::chrono::nanoseconds;
       end= static_cast<unsigned long long>(high_resolution_clock::now().time_since_epoch() / nanoseconds(1));
   }

   //calculate elasped time then pass result to running total function
   void findElapsedTime()
   {
       elapsed_secs = (end-begin);
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
     fclose (pFile);
   }

  private:
   // hidden data from outside world
  };
}
