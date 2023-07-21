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
#include "vault/thread-wrapper.hpp"

#include <chrono>
#include <vector>



namespace lib {
namespace test{
  
  namespace {
    constexpr size_t DEFAULT_RUNS = 10'000'000;
    constexpr double SCALE = 1e6;                  // Results are in µ sec
  }
  
  
  /**
   * Helper to invoke a functor or λ to observe its running time.
   * @param invokeTestLoop the test (complete including loop) invoked once
   * @param repeatCnt number of repetitions to divide the timing measurement
   * @return averaged time for one repetition, in nanoseconds
   */
  template<class FUN>
  inline double
  benchmarkTime (FUN const& invokeTestLoop, const size_t repeatCnt = DEFAULT_RUNS)
  {
    using std::chrono::system_clock;
    using Dur = std::chrono::duration<double>;
    const double SCALE = 1e9; // Results are in ns
    
    auto start = system_clock::now();
    invokeTestLoop();
    Dur duration = system_clock::now () - start;
    return duration.count()/(repeatCnt) * SCALE;
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
    ASSERT_VALID_SIGNATURE (decltype(testSubject), size_t&(size_t));
    
    size_t checksum{0};
    for (size_t i=0; i<repeatCnt; ++i)
      checksum += testSubject(i);
    return checksum;
  }
  
  
  /** perform a simple looped microbenchmark.
   * @param testSubject the operation to test as functor or λ
   * @return a pair `(nanoseconds, checksum)`
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
    double nanos = benchmarkTime (invokeTestLoop, repeatCnt);
    return std::make_tuple (nanos, checksum);
  }
  
  
  
  
  /** perform a multithreaded microbenchmark.
   * This function fires up a number of threads
   * and invokes the given test subject repeatedly.
   * @tparam number of threads to run in parallel
   * @param subject `void(void)` function to be timed
   * @return the averaged invocation time in _microseconds_
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
  threadBenchmark(FUN const& subject, const size_t nRepeat = DEFAULT_RUNS)
  {
    using vault::ThreadJoinable;
    using std::chrono::system_clock;
    
    using Dur = std::chrono::duration<double>;
    
    struct Thread
      : ThreadJoinable
      {
        Thread(FUN const& subject, size_t loopCnt)
          : ThreadJoinable("Micro-Benchmark"
                          ,[=]()                   // local copy of the test-subject-Functor
                             {
                               syncPoint();        // block until all threads are ready
                               auto start = system_clock::now();
                               for (size_t i=0; i < loopCnt; ++i)
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
      threads.emplace_back (subject, nRepeat);

    for (auto& thread : threads)
      thread.sync();                               // start timing measurement
    
    Dur sumDuration{0.0};
    for (auto& thread : threads)
      {
        thread.join();                             // block on measurement end
        sumDuration += thread.duration;
      }
    
    return sumDuration.count() / (nThreads * nRepeat) * SCALE;
  }
  
  
  
}} // namespace lib::test
#endif /*LIB_TEST_MICROBENCHMARK_H*/
