/*
  MICROBENCHMARK.hpp  -  multithreaded timing measurement

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/


/** @file microbenchmark.hpp
 ** A function to perform multithreaded timing measurement on a given functor.
 ** This helper simplifies micro benchmarks of isolated implementation details.
 ** The test subject, given as function object or lambda, is copied into N threads
 ** and invoked numerous times within a tight loop. After waiting on termination of
 ** the test threads, results are summed up and then averaged into milliseconds
 ** per single invocation. The actual timing measurement relies on `chrono::duration`,
 ** which means to count micro ticks of the OS.
 ** @warning care has to bee taken when optimisation is involved!
 **     Optimisation usually has quite some impact on the results, but since
 **     this function is inline, the lambda can typically be inlined and the
 **     loop possibly be optimised away entirely. A simple workaround is to
 **     define a _volatile_ variable in the call context, close the lambda
 **     by reference, and perform a comparison with that volatile variable
 **     in each invocation. The compiler is required actually to access the
 **     value of the volatile each time.
 ** @remarks some interesting observations (in my setup, 8 core AMD FX-8350)
 **     - if we replace the global volatile by a local variable within the
 **       test subject, the initialisation of that local typically costs +5ns
 **       per invocation.
 **     - incrementing the volatile costs +10ns
 **     - multithreaded (unlocked) incrementing of the _global_ volatile
 **       creates massive overhead and increases the running time by factor 100.
 **       This nicely confirms that the x86_64 platform has strong cache coherence.
 ** 
 */


#ifndef LIB_TEST_MICROBENCHMARK_H
#define LIB_TEST_MICROBENCHMARK_H


#include "backend/thread-wrapper.hpp"

#include <chrono>
#include <vector>



namespace lib {
namespace test{
  
  namespace {
    constexpr size_t NUM_MEASUREMENTS = 10000000;
    constexpr double SCALE = 1e6;                  // Results are in µ sec
  }
  
  
  /** perform a multithreaded microbenchmark.
   * This function fires up a number of threads
   * and invokes the given test subject repeatedly.
   * @tparam number of threads to run in parallel
   * @param subject `void(void)` function to be timed
   * @return the averaged invocation time in _mircroseconds_
   * @remarks - the subject function will be _copied_ into each thread
   *          - so `nThreads` copies of this function will run in parallel
   *          - consider locking if this function accesses a shared closure.
   *          - if you pass a lambda, it is eligible for inlining followed
   *            by loop optimisation -- be sure to include some action, like
   *            e.g. accessing a volatile variable, to prevent the compiler
   *            from optimising it away entirely.
   */
  template<size_t nThreads, class FUN>
  inline double
  microbenchmark(FUN const& subject)
  {
    using backend::ThreadJoinable;
    using std::chrono::system_clock;
    
    using Dur = std::chrono::duration<double>;
    
    struct Thread
      : ThreadJoinable
      {
        Thread(FUN const& subject)
          : ThreadJoinable("Micro-Benchmark"
                          ,[subject, this]()       // local copy of the test-subject-Functor
                                    {
                                      syncPoint(); // block until all threads are ready
                                      auto start = system_clock::now();
                                      for (size_t i=0; i < NUM_MEASUREMENTS; ++i)
                                        subject();
                                      duration = system_clock::now () - start;
                                    })
          { }
        /** measured time within thread */
        Dur duration{};
      };
    
    std::vector<Thread> threads;
    threads.reserve(nThreads);
    for (size_t n=0; n<nThreads; ++n)              // create test threads
      threads.emplace_back (subject);

    for (auto& thread : threads)
      thread.sync();                               // start timing measurement
    
    Dur sumDuration{0.0};
    for (auto& thread : threads)
      {
        thread.join();                             // block on measurement end
        sumDuration += thread.duration;
      }
    
    return sumDuration.count() / (nThreads * NUM_MEASUREMENTS) * SCALE;
  }
  
  
  
}} // namespace lib::test
#endif /*LIB_TEST_MICROBENCHMARK_H*/
