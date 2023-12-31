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
          
          std::vector<LevelWeight> levelWeights = testLoad.allLevelWeights().effuse();
          std::vector<double>      scheduleSeq  = testLoad.levelScheduleSequence(4).effuse();
SHOW_EXPR(levelWeights.size())
          for (uint i=0; i<levelWeights.size(); ++i)
            {
              cout<<"level:"<<i<<" nodes:"<<levelWeights[i].nodes<<" ∑="<<levelWeights[i].weight<<" ..step="<<scheduleSeq[i]<<endl;
            }
          
          auto node = testLoad.allNodePtr().effuse();
          for (auto& n : node)
            cout<<"level:"<<n->level<<" w="<<n->weight;
          auto getWeight = [&](uint level) { return levelWeights[level].weight; };
          CHECK (node[22]->level == 16);
          CHECK (node[23]->level == 17);  CHECK (node[23]->weight == 3);  CHECK (getWeight(17) == 3);
          
          CHECK (node[24]->level == 18);  CHECK (node[24]->weight == 0);
          CHECK (node[25]->level == 18);  CHECK (node[25]->weight == 0);
          CHECK (node[26]->level == 18);  CHECK (node[26]->weight == 0);
          CHECK (node[27]->level == 18);  CHECK (node[27]->weight == 0);
          CHECK (node[28]->level == 18);  CHECK (node[28]->weight == 0);  CHECK (getWeight(18) == 0);
          
          CHECK (node[29]->level == 19);  CHECK (node[29]->weight == 2);
          CHECK (node[30]->level == 19);  CHECK (node[30]->weight == 2);
          CHECK (node[31]->level == 19);  CHECK (node[31]->weight == 2);
          CHECK (node[32]->level == 19);  CHECK (node[32]->weight == 2);
          CHECK (node[33]->level == 19);  CHECK (node[33]->weight == 2);  CHECK (getWeight(19) == 10);
          
          CHECK (node[34]->level == 20);  CHECK (node[34]->weight == 3);
          CHECK (node[35]->level == 20);  CHECK (node[35]->weight == 2);  CHECK (getWeight(20) == 5);
          
          CHECK (node[36]->level == 21);  CHECK (node[36]->weight == 1);
          CHECK (node[37]->level == 21);  CHECK (node[37]->weight == 1);
          CHECK (node[38]->level == 21);  CHECK (node[38]->weight == 3);  CHECK (getWeight(21) == 5);
          
          CHECK (levelWeights[19].nodes = 5);
          CHECK (levelWeights[19].weight = 10);
          CHECK (computeWeightFactor(levelWeights[19], 1) == 10.0);
          CHECK (computeWeightFactor(levelWeights[19], 2) == 10.0 / (5.0/3));
          CHECK (computeWeightFactor(levelWeights[19], 3) == 10.0 / (5.0/2));
          CHECK (computeWeightFactor(levelWeights[19], 4) == 10.0 / (5.0/2));
          CHECK (computeWeightFactor(levelWeights[19], 5) == 10.0 / (5.0/1));
          
          CHECK (levelWeights[16].weight == 1);  CHECK (levelWeights[16].nodes == 1);  CHECK(scheduleSeq[16] == 13.0);
          CHECK (levelWeights[17].weight == 3);  CHECK (levelWeights[17].nodes == 1);  CHECK(scheduleSeq[17] == 16.0);
          CHECK (levelWeights[18].weight == 0);  CHECK (levelWeights[18].nodes == 5);  CHECK(scheduleSeq[18] == 16.0);
          CHECK (levelWeights[19].weight ==10);  CHECK (levelWeights[19].nodes == 5);  CHECK(scheduleSeq[19] == 20.0);
          CHECK (levelWeights[20].weight == 5);  CHECK (levelWeights[20].nodes == 2);  CHECK(scheduleSeq[20] == 22.5);
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
