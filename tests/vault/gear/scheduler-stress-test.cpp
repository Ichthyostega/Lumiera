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
          
          auto node  = testLoad.allNodePtr().effuse();
          auto level = testLoad.allLevelWeights().effuse();
          CHECK (level.size() == 27);
          
          _Fmt  nodeFmt{"i=%-2d lev:%-2d w=%1d"};
          _Fmt levelFmt{"  Σ%-2d Σw:%2d"};
          auto  nodeStr = [&](uint i)
                            { 
                              size_t l = node[i]->level;
                              return string{nodeFmt % i % node[i]->level % node[i]->weight}
                                   + (i == level[l].endidx? string{levelFmt % level[l].nodes % level[l].weight}
                                                          : string{"  ·   ·    "});
                            };
                          //    |idx--level--wght|-levelSum-------  
          CHECK (nodeStr( 1) == "i=1  lev:1  w=0  Σ1  Σw: 0"_expect);
          CHECK (nodeStr( 2) == "i=2  lev:2  w=2  Σ1  Σw: 2"_expect);
          CHECK (nodeStr( 3) == "i=3  lev:3  w=0  Σ1  Σw: 0"_expect);
          CHECK (nodeStr( 4) == "i=4  lev:4  w=0  Σ1  Σw: 0"_expect);
          CHECK (nodeStr( 5) == "i=5  lev:5  w=0  Σ1  Σw: 0"_expect);
          CHECK (nodeStr( 6) == "i=6  lev:6  w=1  Σ1  Σw: 1"_expect);
          CHECK (nodeStr( 7) == "i=7  lev:7  w=2  Σ1  Σw: 2"_expect);
          CHECK (nodeStr( 8) == "i=8  lev:8  w=2  Σ1  Σw: 2"_expect);
          CHECK (nodeStr( 9) == "i=9  lev:9  w=1  ·   ·    "_expect);
          CHECK (nodeStr(10) == "i=10 lev:9  w=1  Σ2  Σw: 2"_expect);
          CHECK (nodeStr(11) == "i=11 lev:10 w=0  ·   ·    "_expect);
          CHECK (nodeStr(12) == "i=12 lev:10 w=0  Σ2  Σw: 0"_expect);
          CHECK (nodeStr(13) == "i=13 lev:11 w=0  ·   ·    "_expect);
          CHECK (nodeStr(14) == "i=14 lev:11 w=0  Σ2  Σw: 0"_expect);
          CHECK (nodeStr(15) == "i=15 lev:12 w=1  ·   ·    "_expect);
          CHECK (nodeStr(16) == "i=16 lev:12 w=1  Σ2  Σw: 2"_expect);
          CHECK (nodeStr(17) == "i=17 lev:13 w=1  ·   ·    "_expect);
          CHECK (nodeStr(18) == "i=18 lev:13 w=1  Σ2  Σw: 2"_expect);
          CHECK (nodeStr(19) == "i=19 lev:14 w=2  ·   ·    "_expect);
          CHECK (nodeStr(20) == "i=20 lev:14 w=2  Σ2  Σw: 4"_expect);
          CHECK (nodeStr(21) == "i=21 lev:15 w=0  Σ1  Σw: 0"_expect);
          CHECK (nodeStr(22) == "i=22 lev:16 w=1  Σ1  Σw: 1"_expect);
          CHECK (nodeStr(23) == "i=23 lev:17 w=3  Σ1  Σw: 3"_expect);
          CHECK (nodeStr(24) == "i=24 lev:18 w=0  ·   ·    "_expect);
          CHECK (nodeStr(25) == "i=25 lev:18 w=0  ·   ·    "_expect);
          CHECK (nodeStr(26) == "i=26 lev:18 w=0  ·   ·    "_expect);
          CHECK (nodeStr(27) == "i=27 lev:18 w=0  ·   ·    "_expect);
          CHECK (nodeStr(28) == "i=28 lev:18 w=0  Σ5  Σw: 0"_expect);
          CHECK (nodeStr(29) == "i=29 lev:19 w=2  ·   ·    "_expect);
          CHECK (nodeStr(30) == "i=30 lev:19 w=2  ·   ·    "_expect);
          CHECK (nodeStr(31) == "i=31 lev:19 w=2  ·   ·    "_expect);
          CHECK (nodeStr(32) == "i=32 lev:19 w=2  ·   ·    "_expect);
          CHECK (nodeStr(33) == "i=33 lev:19 w=2  Σ5  Σw:10"_expect);
          CHECK (nodeStr(34) == "i=34 lev:20 w=3  ·   ·    "_expect);
          CHECK (nodeStr(35) == "i=35 lev:20 w=2  Σ2  Σw: 5"_expect);
          CHECK (nodeStr(36) == "i=36 lev:21 w=1  ·   ·    "_expect);
          CHECK (nodeStr(37) == "i=37 lev:21 w=1  ·   ·    "_expect);
          CHECK (nodeStr(38) == "i=38 lev:21 w=3  Σ3  Σw: 5"_expect);
          CHECK (nodeStr(39) == "i=39 lev:22 w=3  ·   ·    "_expect);
          CHECK (nodeStr(40) == "i=40 lev:22 w=3  ·   ·    "_expect);
          CHECK (nodeStr(41) == "i=41 lev:22 w=0  ·   ·    "_expect);
          CHECK (nodeStr(42) == "i=42 lev:22 w=0  ·   ·    "_expect);
          CHECK (nodeStr(43) == "i=43 lev:22 w=0  ·   ·    "_expect);
          CHECK (nodeStr(44) == "i=44 lev:22 w=0  Σ6  Σw: 6"_expect);
          CHECK (nodeStr(45) == "i=45 lev:23 w=0  ·   ·    "_expect);
          
          
          CHECK (level[19].nodes = 5);
          CHECK (level[19].weight = 10);
          CHECK (computeWeightFactor(level[19], 1) == 10.0);
          CHECK (computeWeightFactor(level[19], 2) == 10.0 / (5.0/3));
          CHECK (computeWeightFactor(level[19], 3) == 10.0 / (5.0/2));
          CHECK (computeWeightFactor(level[19], 4) == 10.0 / (5.0/2));
          CHECK (computeWeightFactor(level[19], 5) == 10.0 / (5.0/1));
          
SHOW_EXPR(level.size())

          uint concurrency = 4;
          auto steps = testLoad.levelScheduleSequence(concurrency).effuse();
          CHECK (steps.size() == 27);
          
          auto boost = [&](uint i){ return level[i].nodes / std::ceil (double(level[i].nodes)/concurrency); };
          auto wfact = [&](uint i){ return computeWeightFactor(level[i], concurrency);                      }; 
          
          _Fmt stepFmt{"lev:%-2d  nodes:%-2d Σw:%2d %4.1f Δ%5.3f ▿▿ %6.3f"};
          auto stepStr = [&](uint i){ return string{stepFmt % i % level[i].nodes % level[i].weight % boost(i) % wfact(i) % steps[i]}; };
          
          CHECK (stepStr( 0) == "lev:0   nodes:1  Σw: 0  1.0 Δ0.000 ▿▿  0.000"_expect);
          CHECK (stepStr( 1) == "lev:1   nodes:1  Σw: 0  1.0 Δ0.000 ▿▿  0.000"_expect);
          CHECK (stepStr( 2) == "lev:2   nodes:1  Σw: 2  1.0 Δ2.000 ▿▿  2.000"_expect);
          CHECK (stepStr( 3) == "lev:3   nodes:1  Σw: 0  1.0 Δ0.000 ▿▿  2.000"_expect);
          CHECK (stepStr( 4) == "lev:4   nodes:1  Σw: 0  1.0 Δ0.000 ▿▿  2.000"_expect);
          CHECK (stepStr( 5) == "lev:5   nodes:1  Σw: 0  1.0 Δ0.000 ▿▿  2.000"_expect);
          CHECK (stepStr( 6) == "lev:6   nodes:1  Σw: 1  1.0 Δ1.000 ▿▿  3.000"_expect);
          CHECK (stepStr( 7) == "lev:7   nodes:1  Σw: 2  1.0 Δ2.000 ▿▿  5.000"_expect);
          CHECK (stepStr( 8) == "lev:8   nodes:1  Σw: 2  1.0 Δ2.000 ▿▿  7.000"_expect);
          CHECK (stepStr( 9) == "lev:9   nodes:2  Σw: 2  2.0 Δ1.000 ▿▿  8.000"_expect);
          CHECK (stepStr(10) == "lev:10  nodes:2  Σw: 0  2.0 Δ0.000 ▿▿  8.000"_expect);
          CHECK (stepStr(11) == "lev:11  nodes:2  Σw: 0  2.0 Δ0.000 ▿▿  8.000"_expect);
          CHECK (stepStr(12) == "lev:12  nodes:2  Σw: 2  2.0 Δ1.000 ▿▿  9.000"_expect);
          CHECK (stepStr(13) == "lev:13  nodes:2  Σw: 2  2.0 Δ1.000 ▿▿ 10.000"_expect);
          CHECK (stepStr(14) == "lev:14  nodes:2  Σw: 4  2.0 Δ2.000 ▿▿ 12.000"_expect);
          CHECK (stepStr(15) == "lev:15  nodes:1  Σw: 0  1.0 Δ0.000 ▿▿ 12.000"_expect);
          CHECK (stepStr(16) == "lev:16  nodes:1  Σw: 1  1.0 Δ1.000 ▿▿ 13.000"_expect);
          CHECK (stepStr(17) == "lev:17  nodes:1  Σw: 3  1.0 Δ3.000 ▿▿ 16.000"_expect);
          CHECK (stepStr(18) == "lev:18  nodes:5  Σw: 0  2.5 Δ0.000 ▿▿ 16.000"_expect);
          CHECK (stepStr(19) == "lev:19  nodes:5  Σw:10  2.5 Δ4.000 ▿▿ 20.000"_expect);
          CHECK (stepStr(20) == "lev:20  nodes:2  Σw: 5  2.0 Δ2.500 ▿▿ 22.500"_expect);
          CHECK (stepStr(21) == "lev:21  nodes:3  Σw: 5  3.0 Δ1.667 ▿▿ 24.167"_expect);
          CHECK (stepStr(22) == "lev:22  nodes:6  Σw: 6  3.0 Δ2.000 ▿▿ 26.167"_expect);
          CHECK (stepStr(23) == "lev:23  nodes:6  Σw: 6  3.0 Δ2.000 ▿▿ 28.167"_expect);
          CHECK (stepStr(24) == "lev:24  nodes:10 Σw: 9  3.3 Δ2.700 ▿▿ 30.867"_expect);
          CHECK (stepStr(25) == "lev:25  nodes:2  Σw: 2  2.0 Δ1.000 ▿▿ 31.867"_expect);
          CHECK (stepStr(26) == "lev:26  nodes:1  Σw: 1  1.0 Δ1.000 ▿▿ 32.867"_expect);
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
