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
          
          std::vector<TestChainLoad<>::Agg> levelWeights = testLoad.allNodes().processingLayer<TestChainLoad<>::WeightAggregator>().effuse();
SHOW_EXPR(levelWeights.size())
          for (auto& w : levelWeights)
            {
              auto& [level,nodes,weight] = w;
              cout<<"level:"<<level<<" nodes:"<<nodes<<" ∑="<<weight<<endl;
            }
          
          using Node = TestChainLoad<>::Node;
          auto nodeData = testLoad.allNodes()
                                  .transform([](Node& n){ return make_pair(n.level,n.weight); })
                                  .effuse();
          for (auto& n : nodeData)
            cout<<"level:"<<n.first<<" w="<<n.second<<endl;
          auto getWeight = [&](uint level) { return std::get<2> (levelWeights[level]); };
          CHECK (nodeData[22].first == 16);
          CHECK (nodeData[23].first == 17);  CHECK (nodeData[23].second == 3);  CHECK (getWeight(17) == 3);
          
          CHECK (nodeData[24].first == 18);  CHECK (nodeData[24].second == 0);
          CHECK (nodeData[25].first == 18);  CHECK (nodeData[25].second == 0);
          CHECK (nodeData[26].first == 18);  CHECK (nodeData[26].second == 0);
          CHECK (nodeData[27].first == 18);  CHECK (nodeData[27].second == 0);
          CHECK (nodeData[28].first == 18);  CHECK (nodeData[28].second == 0);  CHECK (getWeight(18) == 0);
          
          CHECK (nodeData[29].first == 19);  CHECK (nodeData[29].second == 2);
          CHECK (nodeData[30].first == 19);  CHECK (nodeData[30].second == 2);
          CHECK (nodeData[31].first == 19);  CHECK (nodeData[31].second == 2);
          CHECK (nodeData[32].first == 19);  CHECK (nodeData[32].second == 2);
          CHECK (nodeData[33].first == 19);  CHECK (nodeData[33].second == 2);  CHECK (getWeight(19) == 10);
          
          CHECK (nodeData[34].first == 20);  CHECK (nodeData[34].second == 3);
          CHECK (nodeData[35].first == 20);  CHECK (nodeData[35].second == 2);  CHECK (getWeight(20) == 5);
          
          CHECK (nodeData[36].first == 21);  CHECK (nodeData[36].second == 1);
          CHECK (nodeData[37].first == 21);  CHECK (nodeData[37].second == 1);
          CHECK (nodeData[38].first == 21);  CHECK (nodeData[38].second == 3);  CHECK (getWeight(21) == 5);
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
