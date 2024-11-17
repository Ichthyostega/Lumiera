/*
  IncidenceCount(Test)  -  observe and evaluate concurrent activations

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/

/** @file incidence-count-test.cpp
 ** unit test \ref IncidenceCount_test
 */


#include "lib/test/run.hpp"
#include "lib/test/microbenchmark.hpp"
#include "lib/incidence-count.hpp"
#include "lib/thread.hpp"
#include "lib/util.hpp"

#include <thread>


using util::isLimited;
using std::this_thread::sleep_for;
using std::chrono_literals::operator ""ms;
using std::chrono_literals::operator ""us;
using std::chrono::microseconds;


namespace lib {
namespace test{
  
  namespace {
    inline bool
    isNumEq (double d1, double d2)
    {
      return 0.001 > abs(d1-d2);
    };
  }
  
  
  
  /***************************************************************//**
   * @test verify recording and evaluation of concurrent invocations
   *       of a piece of code instrumented for investigation.
   * @see incidence-count.hpp
   * @see vault::gear::TestChainLoad::ScheduleCtx
   */
  class IncidenceCount_test
    : public Test
    {
      void
      run (Arg)
        {
          demonstrate_usage();
          verify_incidentCount();
          verify_concurrencyStatistic();
          perform_multithreadStressTest();
        }
      
      
      
      /** @test watch time spent in code bracketed by measurement calls.
       */
      void
      demonstrate_usage()
        {
          IncidenceCount watch;
          watch.markEnter();
          sleep_for (1ms);
          watch.markLeave();
          //
          sleep_for (5ms);
          //
          watch.markEnter();
          sleep_for (1ms);
          watch.markLeave();
          
          double time = watch.calcCumulatedTime();
          CHECK (time > 1900);
          CHECK (time < 2500);
        }
      
      
      /** @test verify proper counting of possibly overlapping incidences. */
      void
      verify_incidentCount()
        {
          IncidenceCount watch;
          watch.expectThreads(1)
               .expectIncidents(20);
          
          watch.markEnter(1);
          sleep_for (1ms);
          watch.markEnter(3);
          sleep_for (2ms);
          watch.markEnter(2);
          watch.markLeave(3);
          sleep_for (1ms);
          watch.markLeave(1);
          watch.markEnter(3);
          sleep_for (3ms);
          watch.markEnter(1);
          watch.markLeave(2);
          sleep_for (1ms);
          watch.markLeave(3);
          sleep_for (1ms);
          watch.markLeave(1);
          
          auto stat = watch.evaluate();
          
          CHECK (isLimited (15500, stat.cumulatedTime, 17800));   // ≈ 16ms
          CHECK (isLimited ( 8500, stat.coveredTime,   10000));   // ≈ 9ms
          CHECK (10== stat.eventCnt);
          CHECK (5 == stat.activationCnt);
          CHECK (0 == stat.cntCase(0));
          CHECK (2 == stat.cntCase(1));
          CHECK (1 == stat.cntCase(2));
          CHECK (2 == stat.cntCase(3));
          CHECK (0 == stat.cntCase(4));
          CHECK (0 == stat.timeCase(0));
          CHECK (isLimited ( 5500, stat.timeCase(1), 6800));      // ≈ 6ms
          CHECK (isLimited ( 3500, stat.timeCase(2), 4500));      // ≈ 4ms
          CHECK (isLimited ( 5500, stat.timeCase(3), 6800));      // ≈ 6ms
          CHECK (0 == stat.timeCase(4));
          CHECK (5 == stat.cntThread(0));
          CHECK (0 == stat.cntThread(1));
          CHECK (stat.activeTime == stat.timeThread(0));
          CHECK (0               == stat.timeThread(1));
          CHECK (isNumEq (stat.activeTime, stat.coveredTime));
          CHECK (isNumEq (stat.cumulatedTime , stat.timeCase(1) + stat.timeCase(2) + stat.timeCase(3)));
        }
      
      
      
      /** @test verify observation of concurrency degree. */
      void
      verify_concurrencyStatistic()
        {
          IncidenceCount watch;
          watch.expectThreads(2)
               .expectIncidents(2);
          
          auto act = [&]{ // two nested activities
                          watch.markEnter();
                          sleep_for (600us);
                          watch.markEnter(2);
                          sleep_for (200us);
                          watch.markLeave(2);
                          watch.markLeave();
                        };
          
          auto run_parallel = [&]
                        {
                          ThreadJoinable t1("test-1", act);
                          ThreadJoinable t2("test-2", act);
                          t1.join();
                          t2.join();
                        };
          
          double runTime = test::benchmarkTime (run_parallel);
          
          // join ensures visibility of all data changes from within threads,
          // which is a prerequisite for performing the data evaluation safely.
          auto stat = watch.evaluate();
          
          CHECK (runTime > stat.coveredTime);
          CHECK (stat.coveredTime < stat.cumulatedTime);
          CHECK (stat.activeTime <= stat.cumulatedTime);
          CHECK (8 == stat.eventCnt);
          CHECK (4 == stat.activationCnt);
          CHECK (2 == stat.cntCase(0));
          CHECK (0 == stat.cntCase(1));
          CHECK (2 == stat.cntCase(2));
          CHECK (0 == stat.cntCase(3));
          CHECK (2 == stat.cntThread(0));
          CHECK (2 == stat.cntThread(1));
          CHECK (0 == stat.cntThread(3));
          CHECK (isLimited(1, stat.avgConcurrency, 2));
          CHECK (0 == stat.timeAtConc(0));
          CHECK (0  < stat.timeAtConc(1));
          CHECK (0  < stat.timeAtConc(2));
          CHECK (0 == stat.timeAtConc(3));
          CHECK (stat.timeAtConc(1) < stat.coveredTime);
          CHECK (stat.timeAtConc(2) < stat.coveredTime);
          
          CHECK (isNumEq (stat.avgConcurrency, (1*stat.timeAtConc(1) + 2*stat.timeAtConc(2)) // average concurrency is a weighted mean
                                               / stat.coveredTime));                        //  of the times spent at each concurrency level
          
          CHECK (isNumEq (stat.cumulatedTime , stat.timeCase(0) + stat.timeCase(2)));       //  cumulated time compounds all cases, including overlap
          CHECK (isNumEq (stat.activeTime    , stat.timeThread(0) + stat.timeThread(1)));   //  while active time disregards overlapping activities per thread
          CHECK (isNumEq (stat.coveredTime   , stat.timeAtConc(1) + stat.timeAtConc(2)));   //  the covered time happens at any non-zero concurrency level
          
          CHECK (stat.timeCase(2) < stat.timeCase(0));                                      //  Note: case-2 is nested into case-0
          CHECK (isNumEq (stat.coveredTime   , stat.timeCase(0) - stat.timeAtConc(2)));     //  Thus, case-0 brackets all time, minus the overlapping segment
        }
      
      
      
      /** @test verify thread-safe operation under pressure. */
      void
      perform_multithreadStressTest()
        {
          constexpr size_t CONCURR = 16;
          const size_t REPETITIONS = 100;

          IncidenceCount watch;
          watch.expectThreads(CONCURR)
               .expectIncidents(10000);
          
          auto act = [&
                     ,gen = makeRandGen()]// local random generator per thread
                     () mutable
                        { // two nested activities with random delay
                          uint delay = 100 + gen.i(800);
                          watch.markEnter();
                          sleep_for (microseconds(delay));
                          watch.markEnter(2);
                          sleep_for (microseconds(delay));
                          watch.markLeave(2);
                          watch.markLeave();
                        };
          
          // Invoke these two nested activations numerous times in several threads
          auto [runTime, sum] = test::threadBenchmark<CONCURR> (act, REPETITIONS);
          
          CHECK (sum == CONCURR*REPETITIONS);      // each invocation contributes +1
          CHECK (isLimited (900, runTime, 1400));  // delay is 500µs on average
          
          // compute statistics over recorded events
          auto stat = watch.evaluate();
          
          // on average two times 500µs per invocation
          CHECK (isLimited (900*REPETITIONS, stat.coveredTime, 1400*REPETITIONS));
          CHECK (stat.activeTime > 900 * REPETITIONS*CONCURR);
          CHECK (stat.activationCnt == 2*REPETITIONS*CONCURR);
          CHECK (stat.cntCase(0) ==      REPETITIONS*CONCURR);
          CHECK (stat.cntCase(1) ==      0);
          CHECK (stat.cntCase(2) ==      REPETITIONS*CONCURR);
          
          CHECK (isLimited(CONCURR/2, stat.avgConcurrency, CONCURR));
          // if there are enough cores, ∅ concurrency should even be close to CONCURR
          
          for (uint i=0; i<CONCURR; ++i)
            CHECK (isLimited(REPETITIONS*900, stat.timeThread(i), REPETITIONS*1400));
          
          CHECK (0 == stat.timeThread(CONCURR));   // there should not be any idle time recorded
          CHECK (0 == stat.timeAtConc(CONCURR+1)); // there should be never more concurrency than number of threads
          // most of the time, concurrency should be close to the defined maximum
          CHECK (isLimited(REPETITIONS*900, stat.timeAtConc(CONCURR), REPETITIONS*1200));
        }
    };
  
  LAUNCHER (IncidenceCount_test, "unit common");
  
  
}} // namespace lib::test

