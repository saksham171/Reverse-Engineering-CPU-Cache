#include <iostream>
#include <climits>
using namespace std;

/******************************************************************
 * USAGE: this gives the time taken to access elements
 * in the same set of the cache. At some point depending
 * on the associativity of the cache, there will be a spike
 * in the cache latencies, giving us the assosciativty of the cache
*******************************************************************/


//global variables to be used for calculating the time
static unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;
static  uint64_t start, endi;

//initialises the cycles_high and cycles_low counter
static inline void time_start(){
  asm volatile ("CPUID\n\t" 
      "RDTSC\n\t" 
      "mov %%edx, %0\n\t" 
      "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low):: 
    "%rax", "%rbx", "%rcx", "%rdx");
}

//initialises the cycles_high1 and cycles_low1 counter
//it then calculates the start time and the end time and
//outputs the time difference to the standard output.
//NOTE: this is technically not the time difference but
//the number of clock cycles between time_start and time_stop
static inline void time_stop(){
  asm volatile("RDTSCP\n\t" 
     "mov %%edx, %0\n\t" 
     "mov %%eax, %1\n\t" 
     "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: 
  "%rax", "%rbx", "%rcx", "%rdx");
    start = ( ((uint64_t) cycles_high << 32) | cycles_low );
    endi = ( ((uint64_t) cycles_high1 << 32) | cycles_low1 ); 
    printf("%llu\n", endi - start);
}

int main(){
  //an array of char, to allow us to access individual addresses
  //size of the array is significantly bigger than the l1, l2 and l3 caches
  //so as to not have the whole array in the cache already
  static volatile char *a = new char[12582912*4];

  int sum = 0;

  //hopefully random values of a[i] so that compiler cannot somehow optimise the loading calls
  for(int i=0; i<12582912*4; i++) a[i] = 'a' + (i*i)%26;
  char t;
  
  //we ping address from 0 to 409600, incrementing 4096 at a time, as that is the size of L1
  //cache for our computer. 
  for(long long int i = 0 ; i < 409600; i+=4096){
      time_start();
      t = a[i]+1;
      time_stop();

      //sum uses the value of t that we have gotten so that the compiler can't optimise away t
      //for being an unused variable.
      sum += t*i;
  }
  //std::cout<<sum<<"\n";
  return 0;
}
