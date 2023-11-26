/*
  TestChainLoad(Test)  -  verify diagnostic setup to watch scheduler activities

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

/** @file test-chain-load-test.cpp
 ** unit test \ref TestChainLoad_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "test-chain-load.hpp"
//#include "vault/real-clock.hpp"
//#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp" ////////////////////////////////////TODO Moo-oh
#include "lib/test/diagnostic-output.hpp"//////////////////////////TODO TOD-oh
#include "lib/util.hpp"


//using lib::time::Time;
//using lib::time::FSecs;

using util::isnil;
using util::isSameObject;
//using lib::test::randStr;
//using lib::test::randTime;


namespace vault{
namespace gear {
namespace test {
  
  
  /*****************************************************************//**
   * @test verify a tool to generate synthetic load for Scheduler tests.
   * @see SchedulerService_test
   * @see SchedulerStress_test
   */
  class TestChainLoad_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          verify_Node();
          verify_Topology();
          control_Topology();
          reseed_recalculate();

          witch_gate();
        }
      
      
      /** @test TODO demonstrate simple usage of the test-load
       * @todo WIP 11/23 🔁 define ⟶ 🔁 implement
       */
      void
      simpleUsage()
        {
          TestChainLoad testLoad;
        }
      
      
      
      /** @test data structure to represent a computation Node
       * @todo WIP 11/23 ✔ define ⟶ ✔ implement
       */
      void
      verify_Node()
        {
          using Node = TestChainLoad<>::Node;
          
          Node n0;                                                          // Default-created empty Node
          CHECK (n0.hash == 0);
          CHECK (n0.level == 0);
          CHECK (n0.repeat == 0);
          CHECK (n0.pred.size() == 0 );
          CHECK (n0.succ.size() == 0 );
          CHECK (n0.pred == Node::Tab{0});
          CHECK (n0.succ == Node::Tab{0});
          
          Node n1{23}, n2{55};                                              // further Nodes with initial seed hash
          CHECK (n1.hash == 23);
          CHECK (n2.hash == 55);
          
          CHECK (0 == n0.calculate());                                      // hash calculation is NOP on unconnected Nodes
          CHECK (0 == n0.hash);
          CHECK (23 == n1.calculate());
          CHECK (23 == n1.hash);
          CHECK (55 == n2.calculate());
          CHECK (55 == n2.hash);
          
          n0.addPred(n1);                                                   // establish bidirectional link between Nodes
          CHECK (isSameObject (*n0.pred[0], n1));
          CHECK (isSameObject (*n1.succ[0], n0));
          CHECK (not n0.pred[1]);
          CHECK (not n1.succ[1]);
          CHECK (n2.pred == Node::Tab{0});
          CHECK (n2.succ == Node::Tab{0});
          
          n2.addSucc(n0);                                                   // works likewise in the other direction
          CHECK (isSameObject (*n0.pred[0], n1));
          CHECK (isSameObject (*n0.pred[1], n2));                           // next link added into next free slot
          CHECK (isSameObject (*n2.succ[0], n0));
          CHECK (not n0.pred[2]);
          CHECK (not n2.succ[1]);
          
          CHECK (n0.hash == 0);
          n0.calculate();                                                   // but now hash calculation combines predecessors
          CHECK (n0.hash == 6050854883719206282u);
          
          Node n00;                                                         // another Node...
          n00.addPred(n2)                                                   // just adding the predecessors in reversed order
             .addPred(n1);
          
          CHECK (n00.hash == 0);
          n00.calculate();                                                  // ==> hash is different, since it depends on order
          CHECK (n00.hash == 17052526497278249714u);
          CHECK (n0.hash  == 6050854883719206282u);

          CHECK (isSameObject (*n1.succ[0], n0));
          CHECK (isSameObject (*n1.succ[1], n00));
          CHECK (isSameObject (*n2.succ[0], n0));
          CHECK (isSameObject (*n2.succ[1], n00));
          CHECK (isSameObject (*n00.pred[0], n2));
          CHECK (isSameObject (*n00.pred[1], n1));
          CHECK (isSameObject (*n0.pred[0],  n1));
          CHECK (isSameObject (*n0.pred[1],  n2));
          
          CHECK (n00.hash == 17052526497278249714u);
          n00.calculate();                                                  // calculation is NOT idempotent (inherently statefull)
          CHECK (n00.hash == 13151338213516862912u);
          
          CHECK (isnil (n0.succ));                                          // number of predecessors or successors properly accounted for
          CHECK (isnil (n00.succ));
          CHECK (n00.succ.empty());
          CHECK (0 == n00.succ.size());
          CHECK (2 == n00.pred.size());
          CHECK (2 == n0.pred.size());
          CHECK (2 == n1.succ.size());
          CHECK (2 == n2.succ.size());
          CHECK (isnil (n1.pred));
          CHECK (isnil (n2.pred));
        }
      
      
      
      /** @test build topology by connecting the nodes
       *        - pre-allocate a block with 32 nodes and then
       *          build a topology to connect these, using default rules
       *        - in the default case, nodes are linearly chained
       *        - hash is also computed by chaining with predecessor hash
       *        - hash computations can be reproduced
       * @todo WIP 11/23 ✔ define ⟶ ✔ implement
       */
      void
      verify_Topology()
        {
          auto graph = TestChainLoad<32>{}
                          .buildToplolgy();
          
          CHECK (graph.topLevel() == 31);
          CHECK (graph.getSeed()  ==  0);
          CHECK (graph.getHash()  == 6692160254289221734u);
          
          auto* node = & *graph.allNodes();
          CHECK (node->hash == graph.getSeed());
          CHECK (node->succ.size() == 1);
          CHECK (isSameObject(*node, *node->succ[0]->pred[0]));
          
          size_t steps{0};
          while (not isnil(node->succ))
            {// verify node connectivity
              ++steps;
              node = node->succ[0];
              CHECK (steps == node->level);
              CHECK (1 == node->pred.size());
              size_t exHash = node->hash;
              
              // recompute the hash -> reproducible
              node->hash = 0;
              node->calculate();
              CHECK (exHash == node->hash);
              
              // explicitly compute the hash using boost::hash
              node->hash = 0;
              boost::hash_combine (node->hash, node->pred[0]->hash);
              CHECK (exHash == node->hash);
            }
           // got a complete chain using all allocated nodes
          CHECK (steps == 31);
          CHECK (steps == graph.topLevel());
          CHECK (node->hash == graph.getHash());
          CHECK (node->hash == 6692160254289221734u);
        }    //  hash of the graph is hash of last node
      
      
      
      /** @test flexible control of generated topology
       * @todo WIP 11/23 🔁 define ⟶ 🔁 implement
       */
      void
      control_Topology()
        {
          auto graph = TestChainLoad<32>{};
          
          graph.expansionRule(graph.rule().probability(0.8).maxVal(1))
               .pruningRule(graph.rule().probability(0.6))
               .buildToplolgy()
               .printTopologyDOT();
        }
      
      
      
      /** @test set and propagate seed values and recalculate all node hashes
       * @todo WIP 11/23 🔁 define ⟶ implement
       */
      void
      reseed_recalculate()
        {
          auto graph = TestChainLoad<32>{};
          graph.expansionRule(graph.rule().probability(0.8).maxVal(1))
               .pruningRule(graph.rule().probability(0.6))
               .buildToplolgy();
          
          using Node = TestChainLoad<32>::Node;
          auto isStartNode = [](Node& n){ return isStart(n); };
          auto isExitNode = [](Node& n){ return isExit(n); };
          
          CHECK (8 == graph.allNodes().filter(isStartNode).count());
          CHECK (15 == graph.allNodes().filter(isExitNode).count());
          
          CHECK (graph.getHash() == 14172386810742845390u);
          
          graph.setSeed(55).clearNodeHashes();
          CHECK (graph.getSeed() == 55);
          CHECK (graph.getHash() == 0);
          
          graph.recalculate();
          CHECK (graph.getHash() == 6093128458724583708u);
        }
      
      
      
      /** @test TODO diagnostic blah
       * @todo WIP 11/23 🔁 define ⟶ implement
       */
      void
      witch_gate()
        {
          UNIMPLEMENTED ("witch gate");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TestChainLoad_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
