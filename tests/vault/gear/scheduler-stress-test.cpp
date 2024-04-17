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
#include "lib/util.hpp"

using test::Test;


namespace vault{
namespace gear {
namespace test {
  
  using util::_Fmt;
  using util::isLimited;
  
  
  
  /***************************************************************************//**
   * @test Investigate and verify non-functional characteristics of the Scheduler.
   * @see SchedulerActivity_test
   * @see SchedulerInvocation_test
   * @see SchedulerCommutator_test
   * @see stress-test-rig.hpp
   */
  class SchedulerStress_test : public Test
    {
      
      virtual void
      run (Arg)
        {
           smokeTest();
           setup_systematicSchedule();
           verify_instrumentation();
           search_breaking_point();
           watch_expenseFunction();
           investigateWorkProcessing();
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
                  .buildTopology()
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
      
      
      
      /** @test build a scheme to adapt the schedule to expected runtime.
       *      - as in many other tests, use the massively forking load pattern
       *      - demonstrate how TestChainLoad computes an idealised level expense
       *      - verify how schedule times are derived from this expense sequence
       * @todo WIP 12/23 ✔ define ⟶ ✔ implement
       */
      void
      setup_systematicSchedule()
        {
          TestChainLoad testLoad{64};
          testLoad.configureShape_chain_loadBursts()
                  .buildTopology()
//                .printTopologyDOT()
//                .printTopologyStatistics()
                  ;
          
          auto LOAD_BASE = 500us;
          ComputationalLoad cpuLoad;
          cpuLoad.timeBase = LOAD_BASE;
          cpuLoad.calibrate();
          
          double micros = cpuLoad.invoke();
          CHECK (micros < 550);
          CHECK (micros > 450);
          
          // build a schedule sequence based on
          // summing up weight factors, with example concurrency ≔ 4
          uint concurrency = 4;
          auto stepFactors = testLoad.levelScheduleSequence(concurrency).effuse();
          CHECK (stepFactors.size() == 1+testLoad.topLevel());
          CHECK (stepFactors.size() == 26);
          
          
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
          CHECK (schedule[24] == _uTicks(24ms));
          CHECK (schedule[25] == _uTicks(25ms));
          CHECK (schedule[26] == _uTicks(26ms));
          
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
          CHECK (stepStr(26) == "lev:26  stepFac:32.200 schedule:16.100"_expect);
          
          
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
          CHECK (stepStr(26) == "lev:26  stepFac:27.300 schedule:45.500"_expect);
          
          // perform a Test with this low stress level (0.3)
          double runTime = testSetup.launch_and_wait();
          double expected = testSetup.getExpectedEndTime();
          CHECK (fabs (runTime-expected) < 5000);
        }    //  Scheduler should able to follow the expected schedule
      
      
      
      /** @test verify capability for instrumentation of job invocations
       * @see IncidenceCount_test
       * @todo WIP 2/24 ✔ define ⟶ ✔ implement
       */
      void
      verify_instrumentation()
        {
          const size_t NODES = 20;
          const size_t CORES = work::Config::COMPUTATION_CAPACITY;
          auto LOAD_BASE = 5ms;
          
          TestChainLoad testLoad{NODES};
          
          BlockFlowAlloc bFlow;
          EngineObserver watch;
          Scheduler scheduler{bFlow, watch};
          
          auto testSetup =
                 testLoad.setWeight(1)
                  .setupSchedule(scheduler)
                  .withLoadTimeBase(LOAD_BASE)
                  .withJobDeadline(50ms)
                  .withInstrumentation()                             // activate an instrumentation bracket around each job invocation
                  ;
          double runTime = testSetup.launch_and_wait();
          
          auto stat = testSetup.getInvocationStatistic();            // retrieve observed invocation statistics
          
          CHECK (runTime < stat.activeTime);
          CHECK (isLimited (4900, stat.activeTime/NODES, 8000));     // should be close to 5000
          CHECK (stat.coveredTime < runTime);
          CHECK (NODES == stat.activationCnt);                       // each node activated once
          CHECK (isLimited (CORES/2, stat.avgConcurrency, CORES));   // should ideally come close to hardware concurrency
          CHECK (0 == stat.timeAtConc(0));
          CHECK (0 == stat.timeAtConc(CORES+1));
          CHECK (runTime/2 < stat.timeAtConc(CORES-1)+stat.timeAtConc(CORES));
        }                                                            // should ideally spend most of the time at highest concurrency levels
      
      
      using StressRig = StressTestRig<16>;
      
      /** @test determine the breaking point towards scheduler overload
       *      - use the integrated StressRig
       *      - demonstrate how parameters can be tweaked
       *      - perform a run, leading to a binary search for the breaking point
       * @remark this stress-test setup uses instrumentation internally to deduce
       *   some systematic deviations from the theoretically established behaviour.
       *   For example, on my machine, the ComputationalLoad performs slower within the
       *   Scheduler environment compared to its calibration, which is done in a tight loop.
       *   This may be due to internals of the processor, which show up under increased
       *   contention combined with more frequent cache misses. In a similar vein, the
       *   actually observed concurrency turns out to be consistently lower than the value
       *   computed by accounting for the work units in isolation, without considering
       *   dependency constraints. These observed deviations are cast into an empirical
       *   »form factor«, which is then used to correct the applied stress factor.
       *   Only with taking these corrective steps, the observed stress factor at
       *   _breaking point_ comes close to the theoretically expected value of 1.0
       * @see stress-test-rig.hpp
       * @todo WIP 1/24 ✔ define ⟶ ✔ implement
       */
      void
      search_breaking_point()
        {
          MARK_TEST_FUN
          
            struct Setup : StressRig
              {
                uint CONCURRENCY = 4;
                bool showRuns = true;
                
                auto testLoad()
                  { return TestLoad{64}.configureShape_chain_loadBursts(); }
                
                auto testSetup (TestLoad& testLoad)
                  {
                    return StressRig::testSetup(testLoad)
                                     .withLoadTimeBase(500us);
                  }
                
              };
            
          auto [stress,delta,time] = StressRig::with<Setup>()
                                               .perform<bench::BreakingPoint>();
          CHECK (delta > 2.5);
          CHECK (1.15 > stress and stress > 0.85);
        }
      
      
      
      /** @test Investigate the relation of run time (expense) to input length.
       *      - again use the integrated StressRig
       *      - this time overload the scheduler with a peak of uncorrelated jobs
       *        and watch the time and load required to work through this challenge
       *      - conduct a series of runs with random number of jobs (within bounds)
       *      - collect the observed data (as CSV), calculate a **linear regression model**
       *      - optionally generate a **Gnuplot** script for visualisation
       * @see vault::gear::bench::ParameterRange
       * @see gnuplot-gen.hpp
       * @todo WIP 4/24 ✔ define ⟶ ✔ implement
       */
      void
      watch_expenseFunction()
        {
          MARK_TEST_FUN
          
            struct Setup
              : StressRig, bench::LoadPeak_ParamRange_Evaluation
              {
                uint CONCURRENCY = 4;
                uint REPETITIONS = 50;
                
                auto testLoad(Param nodes)
                  {
                    TestLoad testLoad{nodes};
                    return testLoad.configure_isolated_nodes();
                  }
                
                auto testSetup (TestLoad& testLoad)
                  {
                    return StressRig::testSetup(testLoad)
                                     .withLoadTimeBase(2ms);
                  }
              };
            
          auto results = StressRig::with<Setup>()
                                   .perform<bench::ParameterRange> (33,128);
          
          auto [socket,gradient,v1,v2,corr,maxDelta,stdev] = bench::linearRegression (results.param, results.time);
          double avgConc = Setup::avgConcurrency (results);
/*
          cout << "───═══───═══───═══───═══───═══───═══───═══───═══───═══───═══───"<<endl;
          cout << Setup::renderGnuplot (results);
          cout << "───═══───═══───═══───═══───═══───═══───═══───═══───═══───═══───"<<endl;
          cout << _Fmt{"Model: %3.2f·p + %3.2f  corr=%4.2f Δmax=%4.2f σ=%4.2f ∅concurrency: %3.1f"}
                      % gradient % socket % corr % maxDelta % stdev % avgConc
               << endl;
*/          
          CHECK (corr > 0.80);                     // clearly a linear correlated behaviour
          CHECK (isLimited (0.4, gradient, 0.7));  // should be slightly above 0.5 (2ms and 4 threads => 0.5ms / Job)
          CHECK (isLimited (3,   socket,   9  ));  // we have a spin-up and a shut-down both ~ 2ms plus some further overhead
          
          CHECK (avgConc > 3);                     // should be able to utilise 4 workers (minus the spin-up/shut-down phase)
        }
      
      
      
      /** @test TODO build a load pattern to emolate a typical high work load
       * @todo WIP 4/24 🔁 define ⟶ implement
       */
      void
      investigateWorkProcessing()
        {
          ComputationalLoad cpuLoad;
          cpuLoad.timeBase = 200us;
          cpuLoad.calibrate();
//////////////////////////////////////////////////////////////////TODO for development only
          MARK_TEST_FUN
/*
                    TestChainLoad testLoad{200};
                    testLoad.configure_isolated_nodes()
                            .buildTopology()
//                            .printTopologyDOT()
                            .printTopologyStatistics();
          {
              
          TRANSIENTLY(work::Config::COMPUTATION_CAPACITY) = 4;
          BlockFlowAlloc bFlow;
          EngineObserver watch;
          Scheduler scheduler{bFlow, watch};
                    
          auto set1 = testLoad.setupSchedule(scheduler)
                         .withLevelDuration(200us)
                         .withJobDeadline(500ms)
                         .withUpfrontPlanning()
                         .withLoadTimeBase(2ms)
                         .withInstrumentation();
          double runTime = set1.launch_and_wait();
          auto stat = set1.getInvocationStatistic();
cout << "time="<<runTime/1000
     << " covered="<<stat.coveredTime / 1000
     << " avgconc="<<stat.avgConcurrency
     <<endl;          
          }
          
          TestChainLoad<8> testLoad{256};
          testLoad.seedingRule(testLoad.rule().probability(0.6).minVal(2))
                  .pruningRule(testLoad.rule().probability(0.44))
                  .setSeed(55)
                  .buildTopology()
//                .printTopologyDOT()
//                .printTopologyStatistics()
                  ;

//          auto stressFac = 1.0;
//          auto concurrency = 8;
//          
//          auto testSetup =
//            testLoad.setupSchedule(scheduler)
//                    .withLoadTimeBase(LOAD_BASE)
//                    .withJobDeadline(50ms)
//                    .withUpfrontPlanning()
//                    .withAdaptedSchedule (stressFac, concurrency);
//          double runTime = testSetup.launch_and_wait();
//          double expected = testSetup.getExpectedEndTime();
//          auto stat = testSetup.getInvocationStatistic();
//SHOW_EXPR(runTime)
//SHOW_EXPR(expected)
//SHOW_EXPR(refTime)
          return;
*/
      using StressRig = StressTestRig<8>;
          
            struct Setup : StressRig
              {
//                double UPPER_STRESS = 12;
                //
//                double FAIL_LIMIT   = 1.0; //0.7;
//                double TRIGGER_SDEV = 1.0; //0.25;
//                double TRIGGER_DELTA = 2.0; //0.5;
//                uint CONCURRENCY = 4;
                uint CONCURRENCY = 8;
//                bool SCHED_DEPENDS = true;
                bool showRuns = true;
                
                auto
                testLoad()
                  {
                    TestLoad testLoad{256};
                    testLoad.seedingRule(testLoad.rule().probability(0.6).minVal(2))
                            .pruningRule(testLoad.rule().probability(0.44))
                            .weightRule(testLoad.value(1))
                            .setSeed(55);
                    return testLoad;
                  }
                
                auto testSetup (TestLoad& testLoad)
                  {
                    return StressRig::testSetup(testLoad)
//                                     .withBaseExpense(200us)
                                     .withLoadTimeBase(5ms);
                  }
              };
          auto [stress,delta,time] = StressRig::with<Setup>()
                                               .perform<bench::BreakingPoint>();
SHOW_EXPR(stress)
SHOW_EXPR(delta)
SHOW_EXPR(time)
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SchedulerStress_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
