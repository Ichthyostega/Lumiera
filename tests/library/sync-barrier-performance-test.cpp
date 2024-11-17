/*
  SyncBarrierPerformance(Test)  -  investigate performance of yield-waiting synchronisation

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file sync-barrier-performance-test.cpp
 ** Assess the performance characteristics of lib::SyncBarrier
 ** Helpers and setup for the \ref SyncBarrierPerformance_test
 */


#include "lib/test/run.hpp"
#include "lib/sync-barrier.hpp"
#include "lib/test/microbenchmark.hpp"
#include "lib/format-cout.hpp"
#include "lib/sync.hpp"

using test::Test;
using std::array;


namespace lib {
namespace test {
  
  namespace {// Test setup....
    
    const uint NUM_STAGES = 1024;
    
    /**
     * Empty placeholder implementation.
     * Used for measurement of test setup overhead.
     */
    class FakeBarrier
      {
        public:
          FakeBarrier(uint=0) { /* be happy */ }
          void sync()         { /* indulge */  }
      };
    
    
    /**
     * A Monitor based reference implementation,
     * using Mutex + Condition Variable for sleeping wait.
     */
    class MonitorSync
      : public Sync<NonrecursiveLock_Waitable>
      {
        int latch_;
        
        bool allPassed() { return latch_ <= 0; }
        
        public:
          MonitorSync (uint nFold =2)
            : latch_{int(nFold)}
            { }
          
          void
          sync()
            {
              Lock sync{this};
              --latch_;
              sync.wait ([this]{ return allPassed(); });
              sync.notify_all();
            }
          
        private:
      };
  }//(End)Test setup
  
  
  
  
  /*******************************************************************//**
   * @test investigate performance of N-fold thread synchronisation.
   *       - use the [multithreaded Microbenchmark](\ref lib::test::threadBenchmark() )
   *       - use an array of consecutively used barriers, one for each per-thread repetition
   *       - test function is parametrised for comparison of different barrier implementations
   * @warning for actually be useful, this test should be compiled with `-O3` and be invoked
   *         stand-alone several times, while otherwise system load is low
   * @see lib::SyncBarrier
   * @see steam::control::DispatcherLoop
   */
  class SyncBarrierPerformance_test : public Test
    {
      template<class BAR, size_t nThreads>
      double
      performanceTest()
        {
          BAR barrier[NUM_STAGES];
          for (uint i=0; i<NUM_STAGES; ++i)
            new(&barrier[i]) BAR{nThreads};
          
          auto testSubject = [&](size_t i) -> size_t
                                {
                                  barrier[i].sync();
                                  return i;       // prevent empty loop optimisation
                                };
          
          auto [micros, cnt] = threadBenchmark<nThreads> (testSubject, NUM_STAGES);
          CHECK (cnt == nThreads * NUM_STAGES*(NUM_STAGES-1)/2);
          return micros;
        }
      
      
      
      /** @test performance investigation of N-fold synchronisation barrier
       * @remark typical values observed with release-build on a 8-core machine
       *         - emptySetup             : 0.6ns
       *         - SyncBarrier (2 Thr)    : 280ns
       *         - SyncBarrier (4 Thr)    : 700ns
       *         - SyncBarrier (8 Thr)    : 2µs
       *         - SyncBarrier (16 Thr)   : 9µs
       *         - SyncBarrier (32 Thr)   : 21µs
       *         - SyncBarrier (48 Thr)   : 30µs
       *         - SyncBarrier (64 Thr)   : 50µs
       *         - SyncBarrier (80 Thr)   : 80µs
       *         - MonitorWait (2 Thr)    : 7µs
       *         - MonitorWait (4 Thr)    : 12µs
       *         - MonitorWait (8 Thr)    : 27µs
       *         - MonitorWait (16 Thr)   : 75µs
       * @note what we are measuring here is actually the *time to catch up*
       *       for all threads involved, implying we are observing the _operational_
       *       delay introduced by synchronisation, and not an overhead of the
       *       implementation technique as such. However — the classical implementation
       *       based on Mutex + ConditionVar, which enters a thread sleep state on wait,
       *       is slower by orders of magnitude. 
       */
      virtual void
      run (Arg)
        {
          cout<<"\n\n■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■"<<endl;
          
          double time_yieldWait_80 = performanceTest<SyncBarrier, 80>();
          double time_yieldWait_64 = performanceTest<SyncBarrier, 64>();
          double time_yieldWait_48 = performanceTest<SyncBarrier, 48>();
          double time_yieldWait_32 = performanceTest<SyncBarrier, 32>();
          double time_yieldWait_16 = performanceTest<SyncBarrier, 16>();
          double time_yieldWait_8  = performanceTest<SyncBarrier,  8>();
          double time_yieldWait_4  = performanceTest<SyncBarrier,  4>();
          double time_yieldWait_2  = performanceTest<SyncBarrier,  2>();
          //
          double time_emptySetup   = performanceTest<FakeBarrier,  5>();
          //
          double time_sleepWait_16 = performanceTest<MonitorSync, 16>();
          double time_sleepWait_8  = performanceTest<MonitorSync,  8>();
          double time_sleepWait_4  = performanceTest<MonitorSync,  4>();
          double time_sleepWait_2  = performanceTest<MonitorSync,  2>();
          
          cout<<"\n___Microbenchmark_______  (µs)"
              <<"\nemptySetup             : "<<time_emptySetup
              <<"\n                       : "
              <<"\nSyncBarrier (2 Thr)    : "<<time_yieldWait_2
              <<"\nSyncBarrier (4 Thr)    : "<<time_yieldWait_4
              <<"\nSyncBarrier (8 Thr)    : "<<time_yieldWait_8
              <<"\nSyncBarrier (16 Thr)   : "<<time_yieldWait_16
              <<"\nSyncBarrier (32 Thr)   : "<<time_yieldWait_32
              <<"\nSyncBarrier (48 Thr)   : "<<time_yieldWait_48
              <<"\nSyncBarrier (64 Thr)   : "<<time_yieldWait_64
              <<"\nSyncBarrier (80 Thr)   : "<<time_yieldWait_80
              <<"\n                       : "
              <<"\nMonitorWait (2 Thr)    : "<<time_sleepWait_2
              <<"\nMonitorWait (4 Thr)    : "<<time_sleepWait_4
              <<"\nMonitorWait (8 Thr)    : "<<time_sleepWait_8
              <<"\nMonitorWait (16 Thr)   : "<<time_sleepWait_16
              <<"\n________________________\n"
              <<"\nbarriers..... "<<NUM_STAGES
              <<endl;
          
          // Unable to assert more than a sanity check here....
          CHECK (time_emptySetup < time_yieldWait_4);
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (SyncBarrierPerformance_test, "function common");
  
  
  
}} // namespace lib::test
