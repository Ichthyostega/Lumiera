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
 ** Functions to perform (multithreaded) timing measurement on a given functor.
 ** This helper simplifies micro benchmarks of isolated implementation details.
 ** The test subject, given as function object or lambda, is invoked numerous times
 ** within a tight loop. In the [multithreaded variant](\ref threadBenchmark()),
 ** the lambda is copied into N threads and performed in each thread in parallel;
 ** after waiting on termination of the test threads, results are summed up and then
 ** averaged into milliseconds per single invocation. The actual timing measurement
 ** relies on `chrono::duration`, which means to count micro ticks of the OS.
 ** @warning care has to bee taken when optimisation is involved!
 **     Optimisation usually has quite some impact on the results, but since
 **     this function is inline, the lambda can typically be inlined and the
 **     loop possibly be optimised away altogether. A simple workaround is
 **     to define a _volatile_ variable in the call context, close the lambda
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


#include "lib/meta/function.hpp"
#include "lib/scoped-collection.hpp"
#include "lib/sync-barrier.hpp"
#include "lib/thread.hpp"

#include "lib/test/microbenchmark-adaptor.hpp"

#include <chrono>



namespace lib {
namespace test{
  
  namespace {
    constexpr size_t DEFAULT_RUNS = 10'000'000;
    using CLOCK_SCALE = std::micro;          // Results are in µ-sec
  }
  
  
  /**
   * Helper to invoke a functor or λ to observe its running time.
   * @param invokeTestLoop the test (complete including loop) invoked once
   * @param repeatCnt number of repetitions to divide the timing measurement
   * @return averaged time for one repetition, in microseconds
   */
  template<class FUN>
  inline double
  benchmarkTime (FUN const& invokeTestLoop, const size_t repeatCnt = DEFAULT_RUNS)
  {
    using std::chrono::steady_clock;
    using Dur = std::chrono::duration<double, CLOCK_SCALE>;
    
    auto start = steady_clock::now();
    invokeTestLoop();
    Dur duration = steady_clock::now () - start;
    return duration.count() / repeatCnt;
  };
  
  
  /**
   * Benchmark building block to invoke a functor or λ in a tight loop,
   * passing the current loop index and capturing a result checksum value.
   * @return sum of all individual invocation results as checksum
   */
  template<class FUN>
  inline size_t
  benchmarkLoop (FUN const& testSubject, const size_t repeatCnt = DEFAULT_RUNS)
  {
    // the test subject gets the current loop-index and returns a checksum value
    auto subject4benchmark = microbenchmark::adapted4benchmark (testSubject);
    
    size_t checksum{0};
    for (size_t i=0; i<repeatCnt; ++i)
      checksum += subject4benchmark(i);
    return checksum;
  }
  
  
  /** perform a simple looped microbenchmark.
   * @param testSubject the operation to test as functor or λ
   * @return a pair `(microseconds, checksum)`
   * @warning this setup is only usable under strong optimisation;
   *          moreover, the scaffolding without actual operation should also
   *          be tested for comparison, to get a feeling for the setup overhead.
   *          For very small test subjects (single operations) it is recommended
   *          to use a direct loop without any lambdas and building blocks.
   */
  template<class FUN>
  inline auto
  microBenchmark (FUN const& testSubject, const size_t repeatCnt = DEFAULT_RUNS)
  {
    size_t checksum{0};
    auto invokeTestLoop = [&]{ checksum = benchmarkLoop (testSubject, repeatCnt); };
    double micros = benchmarkTime (invokeTestLoop, repeatCnt);
    return std::make_tuple (micros, checksum);
  }
  
  
  
  
  /** perform a multithreaded microbenchmark.
   * This function fires up a number of threads
   * and invokes the given test subject repeatedly.
   * @tparam number of threads to run in parallel
   * @param subject function to be timed in parallel
   * @param repeatCnt loop-count _within each thread_
   * @return a pair `(microseconds, checksum)` combining the averaged
   *         invocation time and a compounded checksum from all threads.
   * @remarks - the subject function will be _copied_ into each thread
   *          - so `nThreads` copies of this function will run in parallel
   *          - consider locking if this function accesses a shared closure.
   *          - if you pass a lambda, it is eligible for inlining followed
   *            by loop optimisation -- be sure to include some action, like
   *            e.g. accessing a volatile variable, to prevent the compiler
   *            from entirely optimising it away altogether.
   */
  template<size_t nThreads, class FUN>
  inline auto
  threadBenchmark(FUN const& subject, const size_t repeatCnt = DEFAULT_RUNS)
  {
    using std::chrono::steady_clock;
    using Dur = std::chrono::duration<double, CLOCK_SCALE>;
    
    // the test subject gets the current loop-index and returns a checksum value
    auto subject4benchmark = microbenchmark::adapted4benchmark (subject);
    using Subject = decltype(subject4benchmark);
    
    struct Thread
      : lib::ThreadJoinable<>
      {
        Thread(Subject const& testSubject, size_t loopCnt, SyncBarrier& testStart)
          : ThreadJoinable{"Micro-Benchmark"
                          ,[=, &testStart]()       // local copy of the test-subject-Functor
                             {
                               testStart.sync();   // block until all threads are ready
                               auto start = steady_clock::now();
                               for (size_t i=0; i < loopCnt; ++i)
                                 checksum += testSubject(i);
                               duration = steady_clock::now () - start;
                             }}
          { }
                             // Note: barrier at begin and join at end both ensure data synchronisation
        Dur duration{};      // measured time within thread
        size_t checksum{0};  // collected checksum
      };
    
    SyncBarrier testStart{nThreads + 1};           // coordinated start of timing measurement
    lib::ScopedCollection<Thread> threads(nThreads);
    for (size_t n=0; n<nThreads; ++n)              // create test threads
      threads.emplace (subject4benchmark, repeatCnt, testStart);

    testStart.sync();                              // barrier until all threads are ready
    
    size_t checksum{0};
    Dur sumDuration{0.0};
    for (auto& thread : threads)
      {
        thread.join();                             // block on measurement end (fence)
        sumDuration += thread.duration;
        checksum    += thread.checksum;
      }
    
    double micros = sumDuration.count() / (nThreads * repeatCnt);
    return std::make_tuple (micros, checksum);
  }
  
  
  
}} // namespace lib::test
#endif /*LIB_TEST_MICROBENCHMARK_H*/
