/*
  SchedulerStress(Test)  -  verify scheduler performance characteristics

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file scheduler-usage-test.cpp
 ** unit test \ref SchedulerStress_test
 */


#include "lib/test/run.hpp"
#include "test-chain-load.hpp"
#include "stress-test-rig.hpp"
#include "vault/gear/scheduler.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/test/diagnostic-output.hpp"//////////////////////////TODO work in distress
//#include "lib/format-string.hpp"
#include "lib/test/transiently.hpp"
//#include "lib/test/microbenchmark.hpp"
//#include "lib/util.hpp"

//#include <utility>
//#include <vector>
#include <array>

using test::Test;
//using std::move;
//using util::isSameObject;


namespace vault{
namespace gear {
namespace test {
  
//  using lib::time::FrameRate;
//  using lib::time::Offset;
//  using lib::time::Time;
  using util::_Fmt;
//  using std::vector;
  using std::array;
  
  namespace { // Test definitions and setup...
    
  }
  
  
  
  
  
  /***************************************************************************//**
   * @test Investigate and verify non-functional characteristics of the Scheduler.
   * @see SchedulerActivity_test
   * @see SchedulerInvocation_test
   * @see SchedulerCommutator_test
   */
  class SchedulerStress_test : public Test
    {
      
      virtual void
      run (Arg)
        {
           //smokeTest();
           setup_systematicSchedule();
           search_breaking_point();
           generalFuckup();
           walkingDeadline();
        }
      
      
      /** @test TODO demonstrate sustained operation under load
       *      - TODO this is a placeholder and works now, but need a better example
       *      - it should not produce so much overload, rather some stretch of steady-state processing
       * @todo WIP 12/23 🔁 define ⟶ implement
       */
      void
      smokeTest()
        {
          MARK_TEST_FUN
          TestChainLoad testLoad{512};
          testLoad.configureShape_chain_loadBursts()
                  .buildToplolgy()
//                .printTopologyDOT()
                  ;
          
          auto stats = testLoad.computeGraphStatistics();
          cout << _Fmt{"Test-Load: Nodes: %d  Levels: %d  ∅Node/Level: %3.1f  Forks: %d  Joins: %d"}
                      % stats.nodes
                      % stats.levels
                      % stats.indicators[STAT_NODE].pL
                      % stats.indicators[STAT_FORK].cnt
                      % stats.indicators[STAT_JOIN].cnt
               << endl;
          
          // while building the calculation-plan graph
          // node hashes were computed, observing dependencies
          size_t expectedHash = testLoad.getHash();
          
          // some jobs/nodes are marked with a weight-step
          // these can be instructed to spend some CPU time
          auto LOAD_BASE = 500us;
          testLoad.performGraphSynchronously(LOAD_BASE);
          CHECK (testLoad.getHash() == expectedHash);
          
          double referenceTime = testLoad.calcRuntimeReference(LOAD_BASE);
          cout << "refTime(singleThr): "<<referenceTime/1000<<"ms"<<endl;
          
          
          // Perform through Scheduler----------
          BlockFlowAlloc bFlow;
          EngineObserver watch;
          Scheduler scheduler{bFlow, watch};
          
          double performanceTime =
            testLoad.setupSchedule(scheduler)
                    .withLoadTimeBase(LOAD_BASE)
                    .withJobDeadline(150ms)
                    .withPlanningStep(200us)
                    .withChunkSize(20)
                    .launch_and_wait();
          
          cout << "runTime(Scheduler): "<<performanceTime/1000<<"ms"<<endl;

          // invocation through Scheduler has reproduced all node hashes
          CHECK (testLoad.getHash() == expectedHash);
        }
      
      
      
      /** @test TODO build a scheme to adapt the schedule to expected runtime.
       * @todo WIP 12/23 🔁 define ⟶ implement
       */
      void
      setup_systematicSchedule()
        {
          TestChainLoad testLoad{64};
          testLoad.configureShape_chain_loadBursts()
                  .buildToplolgy()
//                .printTopologyDOT()
//                .printTopologyStatistics()
                  ;
          
          auto LOAD_BASE = 500us;
          ComputationalLoad cpuLoad;
          cpuLoad.timeBase = LOAD_BASE;
          cpuLoad.calibrate();
          
          double micros = cpuLoad.invoke();
SHOW_EXPR(micros);
          CHECK (micros < 550);
          CHECK (micros > 450);
          
          // build a schedule sequence based on
          // summing up weight factors, with example concurrency ≔ 4
          uint concurrency = 4;
          auto stepFactors = testLoad.levelScheduleSequence(concurrency).effuse();
          CHECK (stepFactors.size() == 1+testLoad.topLevel());
          CHECK (stepFactors.size() == 27);
          
          
          // Build-Performance-test-setup--------
          BlockFlowAlloc bFlow;
          EngineObserver watch;
          Scheduler scheduler{bFlow, watch};
          
          auto testSetup =
            testLoad.setupSchedule(scheduler)
                    .withLoadTimeBase(LOAD_BASE)
                    .withJobDeadline(50ms)
                    .withUpfrontPlanning();
          
          auto schedule = testSetup.getScheduleSeq().effuse();
          CHECK (schedule.size() == testLoad.topLevel() + 2);
          CHECK (schedule[ 0] == _uTicks(0ms));
          CHECK (schedule[ 1] == _uTicks(1ms));
          CHECK (schedule[ 2] == _uTicks(2ms));
          //     ....
          CHECK (schedule[25] == _uTicks(25ms));
          CHECK (schedule[26] == _uTicks(26ms));
          CHECK (schedule[27] == _uTicks(27ms));
          
          // Adapted Schedule----------
          double stressFac = 1.0;
          testSetup.withAdaptedSchedule (stressFac, concurrency);
          schedule =  testSetup.getScheduleSeq().effuse();
          CHECK (schedule.size() == testLoad.topLevel() + 2);
          CHECK (schedule[ 0] == _uTicks(0ms));
          CHECK (schedule[ 1] == _uTicks(0ms));

            // verify the numbers in detail....
          _Fmt stepFmt{"lev:%-2d  stepFac:%-6.3f schedule:%6.3f"};
          auto stepStr = [&](uint i){ return string{stepFmt % i % stepFactors[i>0?i-1:0] % (_raw(schedule[i])/1000.0)}; };
          
          CHECK (stepStr( 0) == "lev:0   stepFac:0.000  schedule: 0.000"_expect);
          CHECK (stepStr( 1) == "lev:1   stepFac:0.000  schedule: 0.000"_expect);
          CHECK (stepStr( 2) == "lev:2   stepFac:0.000  schedule: 0.000"_expect);
          CHECK (stepStr( 3) == "lev:3   stepFac:2.000  schedule: 1.000"_expect);
          CHECK (stepStr( 4) == "lev:4   stepFac:2.000  schedule: 1.000"_expect);
          CHECK (stepStr( 5) == "lev:5   stepFac:2.000  schedule: 1.000"_expect);
          CHECK (stepStr( 6) == "lev:6   stepFac:2.000  schedule: 1.000"_expect);
          CHECK (stepStr( 7) == "lev:7   stepFac:3.000  schedule: 1.500"_expect);
          CHECK (stepStr( 8) == "lev:8   stepFac:5.000  schedule: 2.500"_expect);
          CHECK (stepStr( 9) == "lev:9   stepFac:7.000  schedule: 3.500"_expect);
          CHECK (stepStr(10) == "lev:10  stepFac:8.000  schedule: 4.000"_expect);
          CHECK (stepStr(11) == "lev:11  stepFac:8.000  schedule: 4.000"_expect);
          CHECK (stepStr(12) == "lev:12  stepFac:8.000  schedule: 4.000"_expect);
          CHECK (stepStr(13) == "lev:13  stepFac:9.000  schedule: 4.500"_expect);
          CHECK (stepStr(14) == "lev:14  stepFac:10.000 schedule: 5.000"_expect);
          CHECK (stepStr(15) == "lev:15  stepFac:12.000 schedule: 6.000"_expect);
          CHECK (stepStr(16) == "lev:16  stepFac:12.000 schedule: 6.000"_expect);
          CHECK (stepStr(17) == "lev:17  stepFac:13.000 schedule: 6.500"_expect);
          CHECK (stepStr(18) == "lev:18  stepFac:16.000 schedule: 8.000"_expect);
          CHECK (stepStr(19) == "lev:19  stepFac:16.000 schedule: 8.000"_expect);
          CHECK (stepStr(20) == "lev:20  stepFac:20.000 schedule:10.000"_expect);
          CHECK (stepStr(21) == "lev:21  stepFac:22.500 schedule:11.250"_expect);
          CHECK (stepStr(22) == "lev:22  stepFac:24.167 schedule:12.083"_expect);
          CHECK (stepStr(23) == "lev:23  stepFac:26.167 schedule:13.083"_expect);
          CHECK (stepStr(24) == "lev:24  stepFac:28.167 schedule:14.083"_expect);
          CHECK (stepStr(25) == "lev:25  stepFac:30.867 schedule:15.433"_expect);
          CHECK (stepStr(26) == "lev:26  stepFac:31.867 schedule:15.933"_expect);
          CHECK (stepStr(27) == "lev:27  stepFac:32.867 schedule:16.433"_expect);
          
          
          // Adapted Schedule with lower stress level and higher concurrency....
          stressFac = 0.3;
          concurrency = 6;
          stepFactors = testLoad.levelScheduleSequence(concurrency).effuse();
          
          testSetup.withAdaptedSchedule (stressFac, concurrency);
          schedule =  testSetup.getScheduleSeq().effuse();
          
          CHECK (stepStr( 0) == "lev:0   stepFac:0.000  schedule: 0.000"_expect);
          CHECK (stepStr( 1) == "lev:1   stepFac:0.000  schedule: 0.000"_expect);
          CHECK (stepStr( 2) == "lev:2   stepFac:0.000  schedule: 0.000"_expect);
          CHECK (stepStr( 3) == "lev:3   stepFac:2.000  schedule: 3.333"_expect);
          CHECK (stepStr( 4) == "lev:4   stepFac:2.000  schedule: 3.333"_expect);
          CHECK (stepStr( 5) == "lev:5   stepFac:2.000  schedule: 3.333"_expect);
          CHECK (stepStr( 6) == "lev:6   stepFac:2.000  schedule: 3.333"_expect);
          CHECK (stepStr( 7) == "lev:7   stepFac:3.000  schedule: 5.000"_expect);
          CHECK (stepStr( 8) == "lev:8   stepFac:5.000  schedule: 8.333"_expect);
          CHECK (stepStr( 9) == "lev:9   stepFac:7.000  schedule:11.666"_expect);
          CHECK (stepStr(10) == "lev:10  stepFac:8.000  schedule:13.333"_expect);
          CHECK (stepStr(11) == "lev:11  stepFac:8.000  schedule:13.333"_expect);
          CHECK (stepStr(12) == "lev:12  stepFac:8.000  schedule:13.333"_expect);
          CHECK (stepStr(13) == "lev:13  stepFac:9.000  schedule:15.000"_expect);
          CHECK (stepStr(14) == "lev:14  stepFac:10.000 schedule:16.666"_expect);
          CHECK (stepStr(15) == "lev:15  stepFac:12.000 schedule:20.000"_expect);
          CHECK (stepStr(16) == "lev:16  stepFac:12.000 schedule:20.000"_expect);
          CHECK (stepStr(17) == "lev:17  stepFac:13.000 schedule:21.666"_expect);
          CHECK (stepStr(18) == "lev:18  stepFac:16.000 schedule:26.666"_expect);
          CHECK (stepStr(19) == "lev:19  stepFac:16.000 schedule:26.666"_expect);
          CHECK (stepStr(20) == "lev:20  stepFac:18.000 schedule:30.000"_expect);  // note: here the higher concurrency allows to process all 5 concurrent nodes at once
          CHECK (stepStr(21) == "lev:21  stepFac:20.500 schedule:34.166"_expect);
          CHECK (stepStr(22) == "lev:22  stepFac:22.167 schedule:36.944"_expect);
          CHECK (stepStr(23) == "lev:23  stepFac:23.167 schedule:38.611"_expect);
          CHECK (stepStr(24) == "lev:24  stepFac:24.167 schedule:40.277"_expect);
          CHECK (stepStr(25) == "lev:25  stepFac:25.967 schedule:43.277"_expect);
          CHECK (stepStr(26) == "lev:26  stepFac:26.967 schedule:44.944"_expect);
          CHECK (stepStr(27) == "lev:27  stepFac:27.967 schedule:46.611"_expect);
          
          // perform a Test with this low stress level (0.3)
          double runTime = testSetup.launch_and_wait();
          double expected = testSetup.getExpectedEndTime();
          CHECK (fabs (runTime-expected) < 5000);
        }    //  Scheduler should able to follow the expected schedule
      
      
      
      /** @test TODO determine the breaking point towards scheduler overload
       * @todo WIP 1/24 🔁 define ⟶ implement
       */
      void
      search_breaking_point()
        {
          MARK_TEST_FUN
          TestChainLoad testLoad{64};
          testLoad.configureShape_chain_loadBursts()
                  .buildToplolgy()
//                .printTopologyDOT()
//                .printTopologyStatistics()
                  ;
          
          // Adapted Schedule----------
          TRANSIENTLY(work::Config::COMPUTATION_CAPACITY) = 4;
          auto LOAD_BASE = 500us;
          uint concurrency = 4;
          
          // Build-Performance-test-setup--------
          BlockFlowAlloc bFlow;
          EngineObserver watch;
          Scheduler scheduler{bFlow, watch};
          
          auto testSetup =
            testLoad.setupSchedule(scheduler)
                    .withLoadTimeBase(LOAD_BASE)
                    .withJobDeadline(50ms)
                    .withUpfrontPlanning();
          
          auto sqr = [](auto n){ return n*n; };
          _Fmt pointFmt{"....·%-2d: t=%4.1f %s  Δ=%4.1f %s"};
          _Fmt rowFmt  {"%4.2f|  : ∅=%4.1f ±%4.2f    ∅Δ=%4.1f %%%3.1f -- expect:%4.1fms"};
          double refTime = testLoad.calcRuntimeReference(LOAD_BASE);
SHOW_EXPR(refTime);
          const uint REPETITIONS{30};
          for (double stress=0.2; stress < 0.7; stress+=0.03)
            {
              testSetup.withAdaptedSchedule(stress, concurrency);
              array<double, REPETITIONS> runTime;
              array<double, REPETITIONS> delta;
              for (uint i=0; i<REPETITIONS; ++i)
                {
                  runTime[i] = testSetup.launch_and_wait();
                }
              double expTime = testSetup.getExpectedEndTime();
              double avg = lib::explore(runTime).resultSum() / REPETITIONS;
              double avgd = fabs (avg-expTime);
              double sdev{0};
              uint misses{0};
              for (uint i=0; i<REPETITIONS; ++i)
                {
                  sdev += sqr (runTime[i] - avg);
                  delta[i] = fabs (runTime[i] - expTime);
                  if (delta[i] > 2000)
                    ++misses;
                  cout << pointFmt % i % (runTime[i]/1000) % (runTime[i]>avg?"+":"-") % (delta[i]/1000) % (delta[i] > 2000? "●":"○") <<endl;
                }
              sdev = sqrt (sdev/REPETITIONS);
              cout << rowFmt % stress % (avg/1000) % (sdev/1000) % (avgd/1000) % (double(misses)/REPETITIONS) % (expTime/1000) <<endl;
            }
///////////////////////////////////////////////////////////////////////////////////////////////////WIP : draft for testbench-DSL          
            struct Setup : StressRig
              {
                usec LOAD_BASE = 500us;
                uint CONCURRENCY = 4;
                
                auto testLoad() { return TestChainLoad<>{64}.configureShape_chain_loadBursts(); }
              };
            
            auto [stress,delta,time] = StressRig::with<Setup>().searchBreakingPoint();
///////////////////////////////////////////////////////////////////////////////////////////////////WIP : draft for testbench-DSL
SHOW_EXPR(stress)
SHOW_EXPR(delta)
SHOW_EXPR(time)
        }
      
      
      
      /** @test TODO
       * @todo WIP 1/24 🔁 define ⟶ implement
       */
      void
      generalFuckup()
        {
          UNIMPLEMENTED("tbd");
        }
      
      
      
      /** @test TODO
       * @todo WIP 1/24 🔁 define ⟶ implement
       */
      void
      walkingDeadline()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SchedulerStress_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
