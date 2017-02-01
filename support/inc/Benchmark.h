#include <memory>
#include <climits>
#include <iomanip>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <chrono>

namespace PCOE {

  class adder {
  public:
   int counter=0;
   unsigned long long min= ULLONG_MAX;
   unsigned long long max=0;
   unsigned long long begin,end, elapsed_secs;
   double average,total;
   // constructor
    adder(double i=0) {

      total = i;
   }



   // adds value to running total
   void addNum(double number) {
      total += number;
      counter++;
      if(number>max)
        {max=number;}
      if(number<min)
        {min=number;}
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
   };

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
   unsigned long long getElasped_Time()
   {
     return elapsed_secs;
   }

   void nanosecondsBegin()
   {
       using std::chrono::high_resolution_clock;
       using std::chrono::nanoseconds;
       begin= static_cast<unsigned long long>(high_resolution_clock::now().time_since_epoch() / nanoseconds(1));
   }

   void nanosecondsEnd()
   {
       using std::chrono::high_resolution_clock;
       using std::chrono::nanoseconds;
       end= static_cast<unsigned long long>(high_resolution_clock::now().time_since_epoch() / nanoseconds(1));
   }

   void elapsed_time()
   {
       elapsed_secs=(end-begin);
       addNum(elapsed_secs);
   }

   void printTemp()
   {
     printf("\t\t%-25s%-28s%-25s\n", "Min", "Avg", "Max");
   }
   void printScreen()
   {
     printf("%-15s%-25llu%-28f%-26llu\n\n", "Timer:", min, getAverage(),max);
   //printf("%-15s%-25llu%-28f%-26llu\n\n", "Timer 2", min, getAverage(),max);
   }
   void clearFile()
   {
     FILE * pFile;
     pFile=fopen("test.txt","w");
     fclose(pFile);

     FILE * qFile;
     qFile=fopen("test.txt","a+");
     fprintf(qFile,"\t\t%-25s%-28s%-25s\n", "Min", "Avg", "Max");
     fclose(qFile);


   }
   void writeFile()
   {
     FILE * pFile;
     pFile = fopen ("test.txt","a+");
     fprintf(pFile,"%-15s%-25llu%-28f%-26llu\n\n", "Timer:", min, getAverage(),max);
     fclose (pFile);
   }

  private:
   // hidden data from outside world
  };
}
