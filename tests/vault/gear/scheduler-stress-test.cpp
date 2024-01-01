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
#include "vault/gear/scheduler.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/test/diagnostic-output.hpp"//////////////////////////TODO work in distress
//#include "lib/format-string.hpp"
//#include "lib/test/transiently.hpp"
//#include "lib/test/microbenchmark.hpp"
//#include "lib/util.hpp"

//#include <utility>

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
          MARK_TEST_FUN
          TestChainLoad testLoad{64};
          testLoad.configureShape_chain_loadBursts()
                  .buildToplolgy()
                  .printTopologyDOT()
                  .printTopologyStatistics()
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
          auto stepsFactors = testLoad.levelScheduleSequence(concurrency).effuse();
          CHECK (stepsFactors.size() == 1+testLoad.topLevel());
          CHECK (stepsFactors.size() == 27);
          
          
          // Build-Performance-test-setup--------
          BlockFlowAlloc bFlow;
          EngineObserver watch;
          Scheduler scheduler{bFlow, watch};
          
          auto testSetup =
            testLoad.setupSchedule(scheduler)
                    .withLoadTimeBase(LOAD_BASE)
                    .withJobDeadline(20ms)
                    .withUpfrontPlanning();
          
          auto schedule = testSetup.getScheduleSeq().effuse();
          CHECK (schedule.size() == testLoad.topLevel() + 2);
          CHECK (schedule[ 0] == _uTicks(0ms));
          CHECK (schedule[ 1] == _uTicks(1ms));
          CHECK (schedule[26] == _uTicks(26ms));
          CHECK (schedule[27] == _uTicks(27ms));
          
          micros = testSetup.launch_and_wait();
SHOW_EXPR(micros);
        }
      
      
      
      /** @test TODO
       * @todo WIP 12/23 🔁 define ⟶ implement
       */
      void
      generalFuckup()
        {
          UNIMPLEMENTED("tbd");
        }
      
      
      
      /** @test TODO
       * @todo WIP 12/23 🔁 define ⟶ implement
       */
      void
      walkingDeadline()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SchedulerStress_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
