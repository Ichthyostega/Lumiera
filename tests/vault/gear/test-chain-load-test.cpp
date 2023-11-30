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
  
  namespace { // shorthands and parameters for test...
    
    /** shorthand for specific parameters employed by the following tests */
    using ChainLoad32 = TestChainLoad<32,16>;
    using Node = ChainLoad32::Node;
    auto isStartNode = [](Node& n){ return isStart(n); };
    auto isInnerNode = [](Node& n){ return isInner(n); };
    auto isExitNode =  [](Node& n){ return isExit(n); };
    
  }//(End)test definitions
  
  
  
  
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
          verify_Expansion();
          verify_Reduction();
          verify_SeedChains();
          verify_PruneChains();
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
          CHECK (n0.hash == 0x53F8F4753B85558A);
          
          Node n00;                                                         // another Node...
          n00.addPred(n2)                                                   // just adding the predecessors in reversed order
             .addPred(n1);
          
          CHECK (n00.hash == 0);
          n00.calculate();                                                  // ==> hash is different, since it depends on order
          CHECK (n00.hash == 0xECA6BE804934CAF2);
          CHECK (n0.hash  == 0x53F8F4753B85558A);

          CHECK (isSameObject (*n1.succ[0], n0));
          CHECK (isSameObject (*n1.succ[1], n00));
          CHECK (isSameObject (*n2.succ[0], n0));
          CHECK (isSameObject (*n2.succ[1], n00));
          CHECK (isSameObject (*n00.pred[0], n2));
          CHECK (isSameObject (*n00.pred[1], n1));
          CHECK (isSameObject (*n0.pred[0],  n1));
          CHECK (isSameObject (*n0.pred[1],  n2));
          
          CHECK (n00.hash == 0xECA6BE804934CAF2);
          n00.calculate();                                                  // calculation is NOT idempotent (inherently statefull)
          CHECK (n00.hash == 0xB682F06D29B165C0);
          
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
          auto graph = ChainLoad32{}
                          .buildToplolgy();
          
          CHECK (graph.topLevel() == 31);
          CHECK (graph.getSeed()  ==  0);
          CHECK (graph.getHash()  == 0x5CDF544B70E59866);
          
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
          CHECK (node->hash == 0x5CDF544B70E59866);
        }    //  hash of the graph is hash of last node
      
      
      
      /** @test demonstrate shaping of generated topology
       *      - the expansion rule injects forking nodes
       *      - after some expansion, width limitation is enforced
       *      - thus join nodes are introduced to keep all chains connected
       *      - by default, the hash controls shape, evolving identical in each branch
       *      - with additional shuffling, the decisions are more random
       *      - statistics can be computed to characterise the graph
       *      - the graph can be visualised as _Graphviz diagram_
       * @todo WIP 11/23 ✔ define ⟶ ✔ implement
       */
      void
      verify_Expansion()
        {
          ChainLoad32 graph;
          
          // moderate symmetrical expansion with 40% probability and maximal +2 links
          graph.expansionRule(graph.rule().probability(0.4).maxVal(2))
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0xAE332109116C5100);
          
          auto stat = graph.computeGraphStatistics();
          CHECK (stat.indicators[STAT_NODE].cnt == 32);                        // the 32 Nodes...
          CHECK (stat.levels                    == 11);                        // ... were organised into 11 levels
          CHECK (stat.indicators[STAT_FORK].cnt == 4);                         // we got 4 »Fork« events
          CHECK (stat.indicators[STAT_SEED].cnt == 1);                         // one start node
          CHECK (stat.indicators[STAT_EXIT].cnt == 1);                         // and one exit node at end
          CHECK (stat.indicators[STAT_NODE].pL == "2.9090909"_expect);         // ∅ 3 Nodes / level
          CHECK (stat.indicators[STAT_NODE].cL == "0.640625"_expect);          // with Node density concentrated towards end
          
          
          // with additional re-shuffling, probability acts independent in each branch
          // leading to more chances to draw a »fork«, leading to a faster expanding graph
          graph.expansionRule(graph.rule().probability(0.4).maxVal(2).shuffle(23))
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0xCBD0807DF6C84637);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                    == 8);                         // expands faster, with only 8 levels
          CHECK (stat.indicators[STAT_NODE].pL  == 4);                         // this time ∅ 4 Nodes / level
          CHECK (stat.indicators[STAT_FORK].cnt == 7);                         // 7 »Fork« events
          CHECK (stat.indicators[STAT_JOIN].cnt == 2);                         // but also 2 »Join« nodes...
          CHECK (stat.indicators[STAT_JOIN].cL  == "0.92857143"_expect);       // which are totally concentrated towards end
          CHECK (stat.indicators[STAT_EXIT].cnt == 1);                         // finally to connect to the single exit
          
          
          // if the generation is allowed to run for longer,
          // while more constrained in width...
          TestChainLoad<256,8> gra_2;
          gra_2.expansionRule(gra_2.rule().probability(0.4).maxVal(2).shuffle(23))
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (gra_2.getHash() == 0xE629826A1A8DEB38);
          
          stat = gra_2.computeGraphStatistics();
          CHECK (stat.levels                     == 37);                       // much more levels, as can be expected
          CHECK (stat.indicators[STAT_NODE].pL   == "6.9189189"_expect);       // ∅ 7 Nodes per level
          CHECK (stat.indicators[STAT_JOIN].pL   == "0.78378378"_expect);      // but also almost one join per level to deal with the limitation
          CHECK (stat.indicators[STAT_FORK].frac == "0.24609375"_expect);      // 25% forks (there is just not enough room for more forks)
          CHECK (stat.indicators[STAT_JOIN].frac == "0.11328125"_expect);      // and 11% joins
        }
      
      
      
      /** @test demonstrate impact of reduction on graph topology
       *      - after one fixed initial expansion, reduction causes
       *        all chains to be joined eventually
       *      - expansion and reduction can counterbalance each other,
       *        leading to localised »packages« of branchings and reductions
       * @todo WIP 11/23 ✔ define ⟶ ✔ implement
       */
      void
      verify_Reduction()
        {
          ChainLoad32 graph;
          
          // expand immediately at start and then gradually reduce / join chains
          graph.expansionRule(graph.rule_atStart(8))
               .reductionRule(graph.rule().probability(0.2).maxVal(3).shuffle(555))
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x8454196BFA40CFE1);
          
          auto stat = graph.computeGraphStatistics();
          CHECK (stat.levels                    == 9);                         // This connection pattern filled 9 levels
          CHECK (stat.indicators[STAT_JOIN].cnt == 4);                         // we got 4 »Join« events (reductions=
          CHECK (stat.indicators[STAT_FORK].cnt == 1);                         // and the single expansion/fork
          CHECK (stat.indicators[STAT_FORK].cL  == 0.0);                       // ...sitting right at the beginning
          CHECK (stat.indicators[STAT_NODE].cL  == "0.37890625"_expect);       // Nodes are concentrated towards the beginning
          
          
          
          // expansion and reduction can counterbalance each other
          graph.expansionRule(graph.rule().probability(0.2).maxVal(3).shuffle(555))
               .reductionRule(graph.rule().probability(0.2).maxVal(3).shuffle(555))
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x825696EA63E579A4);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                    == 12);                        // This example runs a bit longer
          CHECK (stat.indicators[STAT_NODE].pL  == "2.6666667"_expect);        // in the middle threading 3-5 Nodes per Level
          CHECK (stat.indicators[STAT_FORK].cnt == 5);                         // with 5 expansions
          CHECK (stat.indicators[STAT_JOIN].cnt == 3);                         // and 3 reductions
          CHECK (stat.indicators[STAT_FORK].cL  == "0.45454545"_expect);       // forks dominating earlier
          CHECK (stat.indicators[STAT_JOIN].cL  == "0.66666667"_expect);       // while joins need forks as prerequisite
          
          
          
          // expansion bursts can be balanced with a heightened reduction intensity
          graph.expansionRule(graph.rule().probability(0.3).maxVal(4).shuffle(555))
               .reductionRule(graph.rule().probability(0.9).maxVal(2).shuffle(555))
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0xA850E6A4921521AB);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                    == 12);                        // This graph has a similar outline
          CHECK (stat.indicators[STAT_NODE].pL  == "2.6666667"_expect);        // in the middle threading 3-5 Nodes per Level
          CHECK (stat.indicators[STAT_FORK].cnt == 7);                         // ...yet with quite different internal structure
          CHECK (stat.indicators[STAT_JOIN].cnt == 9);                         //
          CHECK (stat.indicators[STAT_FORK].cL  == "0.41558442"_expect);
          CHECK (stat.indicators[STAT_JOIN].cL  == "0.62626263"_expect);
          CHECK (stat.indicators[STAT_FORK].pLW == "0.19583333"_expect);       // while the densities of forks and joins almost match,
          CHECK (stat.indicators[STAT_JOIN].pLW == "0.26527778"_expect);       // a slightly higher reduction density leads to convergence eventually
        }
      
      
      
      /** @test demonstrate shaping of generated topology by seeding new chains
       *      - the seed rule allows to start new chains in the middle of the graph
       *      - combined with with reduction, the emerging structure resembles
       *        the processing pattern encountered with real media calculations
       * @todo WIP 11/23 ✔ define ⟶ ✔ implement
       */
      void
      verify_SeedChains()
        {
          ChainLoad32 graph;
          
          // randomly start new chains, to be carried-on linearly
          graph.seedingRule(graph.rule().probability(0.2).maxVal(3).shuffle())
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          
          CHECK (graph.getHash() == 0x12A49C0E413B573B);
          
          auto stat = graph.computeGraphStatistics();
          CHECK (stat.levels                    == 8);                         // 8 Levels...
          CHECK (stat.indicators[STAT_SEED].cnt == 11);                        // overall 11 »Seed« events generated several ongoing chains
          CHECK (stat.indicators[STAT_FORK].cnt == 0);                         // yet no branching/expanding
          CHECK (stat.indicators[STAT_LINK].cnt == 19);                        // thus more and more chains were just carried on
          CHECK (stat.indicators[STAT_LINK].pL == 2.375);                      // on average 2-3 per level are continuations
          CHECK (stat.indicators[STAT_NODE].pL == 4);                          // leading to ∅ 4 Nodes per level
          CHECK (stat.indicators[STAT_NODE].cL == "0.63392857"_expect);        // with nodes amassing towards the end
          CHECK (stat.indicators[STAT_LINK].cL == "0.63157895"_expect);        // because there are increasingly more links to carry-on
          CHECK (stat.indicators[STAT_JOIN].cL == "0.92857143"_expect);        // while joining only happens at the end when connecting to exit
          
          
          
          // combining random seed nodes with reduction leads to a processing pattern
          // with side-chaines successively joined into a single common result
          graph.seedingRule(graph.rule().probability(0.2).maxVal(3).shuffle())
               .reductionRule(graph.rule().probability(0.9).maxVal(2))
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x82E39529C470E20A);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.indicators[STAT_SEED].cnt == 11);                        // the same number of 11 »Seed« events
          CHECK (stat.indicators[STAT_JOIN].cnt == 6);                         // but now 6 joining nodes
          CHECK (stat.indicators[STAT_LINK].cnt == 15);                        // and less carry-on
          CHECK (stat.indicators[STAT_FORK].cnt == 0);                         // no branching
          CHECK (stat.indicators[STAT_NODE].pL  == 3.2);                       // leading a slightly leaner graph with ∅ 3.2 Nodes per level
          CHECK (stat.indicators[STAT_NODE].cL  == "0.5625"_expect);           // and also slightly more evenly spaced this time
          CHECK (stat.indicators[STAT_LINK].cL  == "0.55555556"_expect);       // links are also more encountered in the middle
          CHECK (stat.indicators[STAT_JOIN].cL  == "0.72222222"_expect);       // and also joins are happening underway
          CHECK (stat.levels                    == 10);                        // mostly because a leaner graph takes longer to use 32 Nodes
        }
      
      
      
      /** @test TODO demonstrate shaping of generated topology
       *      - TODO the prune rule terminates chains randomly
       *      - this can lead to fragmentation in several sub-graphs
       * @todo WIP 11/23 🔁 define ⟶ 🔁 implement
       */
      void
      verify_PruneChains()
        {
          
        }
//SHOW_EXPR(graph.getHash())
//SHOW_EXPR(stat.indicators[STAT_NODE].pL)
//SHOW_EXPR(stat.indicators[STAT_FORK].cL)
//SHOW_EXPR(stat.indicators[STAT_JOIN].cL)
      
      
      
      /** @test set and propagate seed values and recalculate all node hashes.
       * @remark This test uses parameter rules with some expansion and a
       *         pruning rule with 60% probability. This setup is known to
       *         create a sequence of tiny isolated trees with 4 nodes each;
       *         there are 8 such groups, each with a fork and two exit nodes;
       *         the last group is wired differently however, because there the
       *         limiting-mechanism of the topology generation activates to ensure
       *         that the last node is an exit node. The following code traverses
       *         all nodes grouped into 4-node clusters to verify this regular
       *         pattern and the calculated hashes.
       * @todo WIP 11/23 ✔ define ⟶ ✔ implement
       */
      void
      reseed_recalculate()
        {
          ChainLoad32 graph;
          graph.expansionRule(graph.rule().probability(0.8).maxVal(1))
               .pruningRule(graph.rule().probability(0.6))
               .buildToplolgy();
          
          CHECK (8 == graph.allNodes().filter(isStartNode).count());
          CHECK (15 == graph.allNodes().filter(isExitNode).count());
          
          CHECK (graph.getHash() == 0xC4AE6EB741C22FCE);
          graph.allNodePtr().grouped<4>()
               .foreach([&](auto group)
                 {  // verify wiring pattern
                   //  and the resulting exit hashes
                   auto& [a,b,c,d] = *group;
                   CHECK (isStart(a));
                   CHECK (isInner(b));
                   if (b->succ.size() == 2)
                     {
                       CHECK (isExit(c));
                       CHECK (isExit(d));
                       CHECK (c->hash == 0xAEDC04CFA2E5B999);
                       CHECK (d->hash == 0xAEDC04CFA2E5B999);
                     }
                   else
                     { // the last chunk is wired differently
                       CHECK (b->succ.size() == 1);
                       CHECK (b->succ[0] == c);
                       CHECK (isInner(c));
                       CHECK (isExit(d));
                       CHECK (graph.nodeID(d) == 31);
                       CHECK (d->hash == graph.getHash());
                     }      //   this is the global exit node
                 });
          
          
          graph.setSeed(55).clearNodeHashes();
          CHECK (graph.getSeed() == 55);
          CHECK (graph.getHash() == 0);
          graph.allNodePtr().grouped<4>()
               .foreach([&](auto group)
                 {  // verify hashes have been reset
                   auto& [a,b,c,d] = *group;
                   CHECK (a->hash == 55);
                   CHECK (b->hash ==  0);
                   CHECK (b->hash ==  0);
                   CHECK (b->hash ==  0);
                 });
          
          graph.recalculate();
          CHECK (graph.getHash() == 0x548F240CE91A291C);
          graph.allNodePtr().grouped<4>()
               .foreach([&](auto group)
                 {  // verify hashes were recalculated
                   //  based on the new seed
                   auto& [a,b,c,d] = *group;
                   CHECK (a->hash == 55);
                   if (b->succ.size() == 2)
                     {
                       CHECK (c->hash == 0x7887993B0ED41395);
                       CHECK (d->hash == 0x7887993B0ED41395);
                     }
                   else
                     {
                       CHECK (graph.nodeID(d) == 31);
                       CHECK (d->hash == graph.getHash());
                     }
                 });
          
          // seeding and recalculation are reproducible
          graph.setSeed(0).recalculate();
          CHECK (graph.getHash() == 0xC4AE6EB741C22FCE);
          graph.setSeed(55).recalculate();
          CHECK (graph.getHash() == 0x548F240CE91A291C);
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
