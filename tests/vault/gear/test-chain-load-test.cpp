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
#include "lib/test/diagnostic-output.hpp"
#include "test-chain-load.hpp"
#include "vault/gear/job.h"
#include "lib/util.hpp"

#include <array>


using util::isnil;
using util::isSameObject;
using std::array;


namespace vault{
namespace gear {
namespace test {
  
  namespace { // shorthands and parameters for test...
    
    /** shorthand for specific parameters employed by the following tests */
    using ChainLoad16 = TestChainLoad<16>;
    using Node = ChainLoad16::Node;
    auto isStartNode = [](Node& n){ return isStart(n); };
    auto isInnerNode = [](Node& n){ return isInner(n); };
    auto isExitNode =  [](Node& n){ return isExit(n); };
    
  }//(End)test definitions
  
  
  
  
  /*****************************************************************//**
   * @test verify a tool to generate synthetic load for Scheduler tests.
   * @remark statistics output and the generation of Graphviz-DOT diagrams
   *       is commented out; these diagnostics are crucial to understand
   *       the generated load pattern or to develop new graph shapes.
   *       Visualise graph with `dot -Tpng example.dot | display`
   * @see SchedulerService_test
   * @see SchedulerStress_test
   */
  class TestChainLoad_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          usageExample();
          verify_Node();
          verify_Topology();
          showcase_Expansion();
          showcase_Reduction();
          showcase_SeedChains();
          showcase_PruneChains();
          showcase_StablePattern();
          verify_computation_load();
          verify_reseed_recalculate();
          verify_runtime_reference();
          verify_scheduling_setup();
        }
      
      
      /** @test demonstrate simple usage of the test-load
       *      - build a graph with 64 nodes, grouped into small segments
       *      - use a scheduler instance to »perform« this graph
       */
      void
      usageExample()
        {
          auto testLoad =
            TestChainLoad{64}
               .configureShape_short_segments3_interleaved()
               .buildToplolgy();
          
          // while building the graph, node hashes are computed
          CHECK (testLoad.getHash() == 0xD2F292D864CF8086);
          
          
          BlockFlowAlloc bFlow;
          EngineObserver watch;
          Scheduler scheduler{bFlow, watch};
          
          testLoad.setupSchedule(scheduler)
                  .launch_and_wait();
          
          // invocation through Scheduler has reproduced all node hashes
          CHECK (testLoad.getHash() == 0xD2F292D864CF8086);
        }
      
      
      
      /** @test data structure to represent a computation Node
       */
      void
      verify_Node()
        {
          Node n0;                                                          // Default-created empty Node
          CHECK (n0.hash == 0);
          CHECK (n0.level == 0);
          CHECK (n0.weight == 0);
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
       */
      void
      verify_Topology()
        {
          auto graph = ChainLoad16{32}
                          .buildToplolgy();
          
          CHECK (graph.topLevel() == 31);
          CHECK (graph.getSeed()  ==  0);
          CHECK (graph.getHash()  == 0xB3445F1240A1B05F);
          
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
          CHECK (node->hash == 0x5CDF544B70E59866);
          
          // Since this graph has only a single exit-node,
          // the global hash of the graph is derived from this hash
          size_t globalHash{0};
          boost::hash_combine (globalHash, node->hash);
          CHECK (globalHash == graph.getHash());
          CHECK (globalHash == 0xB3445F1240A1B05F);
        }
      
      
      
      
      
      /** @test demonstrate shaping of generated topology
       *      - the expansion rule injects forking nodes
       *      - after some expansion, width limitation is enforced
       *      - thus join nodes are introduced to keep all chains connected
       *      - by default, the hash controls shape, evolving identical in each branch
       *      - with additional shuffling, the decisions are more random
       *      - statistics can be computed to characterise the graph
       *      - the graph can be visualised as _Graphviz diagram_
       */
      void
      showcase_Expansion()
        {
          ChainLoad16 graph{32};
          
          // moderate symmetrical expansion with 40% probability and maximal +2 links
          graph.expansionRule(graph.rule().probability(0.4).maxVal(2))
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x6EDD7B92F12E9A37);
          
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
          CHECK (graph.getHash() == 0x9E0C7D98B61E1789);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                    == 8);                         // expands faster, with only 8 levels
          CHECK (stat.indicators[STAT_NODE].pL  == 4);                         // this time ∅ 4 Nodes / level
          CHECK (stat.indicators[STAT_FORK].cnt == 7);                         // 7 »Fork« events
          CHECK (stat.indicators[STAT_JOIN].cnt == 2);                         // but also 2 »Join« nodes...
          CHECK (stat.indicators[STAT_JOIN].cL  == "0.92857143"_expect);       // which are totally concentrated towards end
          CHECK (stat.indicators[STAT_EXIT].cnt == 1);                         // finally to connect to the single exit
          
          
          // if the generation is allowed to run for longer,
          // while more constrained in width...
          TestChainLoad<8> gra_2{256};
          gra_2.expansionRule(gra_2.rule().probability(0.4).maxVal(2).shuffle(23))
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (gra_2.getHash() == 0x28B121BE7F1F7362);
          
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
       */
      void
      showcase_Reduction()
        {
          ChainLoad16 graph{32};
          
          // expand immediately at start and then gradually reduce / join chains
          graph.expansionRule(graph.rule_atStart(8))
               .reductionRule(graph.rule().probability(0.2).maxVal(3).shuffle(555))
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x1D201B70F18E995A);
          
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
          CHECK (graph.getHash() == 0x8AF4BDAE5AA6880C);
          
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
          CHECK (graph.getHash() == 0x220A2E81F65146FC);
          
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
       */
      void
      showcase_SeedChains()
        {
          ChainLoad16 graph{32};
          
          // randomly start new chains, to be carried-on linearly
          graph.seedingRule(graph.rule().probability(0.2).maxVal(3).shuffle())
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x11BB1409A61A9B78);
          
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
          CHECK (graph.getHash() == 0x3DFA720156540247);
          
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
      
      
      
      
      
      /** @test demonstrate topology with pruning and multiple segments
       *      - the prune rule terminates chains randomly
       *      - this can lead to fragmentation into several sub-graphs
       *      - these can be completely segregated, or appear interwoven
       *      - equilibrium of seeding and pruning can be established
       */
      void
      showcase_PruneChains()
        {
          ChainLoad16 graph{32};
          
          // terminate chains randomly
          graph.pruningRule(graph.rule().probability(0.2))
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x660BD1CD261A990);
          
          auto stat = graph.computeGraphStatistics();
          CHECK (stat.levels                    == 32);                        // only a single line of connections...
          CHECK (stat.segments                  ==  8);                        // albeit severed into 8 segments
          CHECK (stat.indicators[STAT_NODE].pS  ==  4);                        // with always 4 Nodes per segment
          CHECK (stat.indicators[STAT_NODE].pL  ==  1);                        // and only ever a single node per level
          CHECK (stat.indicators[STAT_SEED].cnt ==  8);                        // consequently we get 8 »Seed« nodes
          CHECK (stat.indicators[STAT_EXIT].cnt ==  8);                        //                     8 »Exit« nodes
          CHECK (stat.indicators[STAT_LINK].cnt == 16);                        //                and 16 interconnecting links
          
          
          // combined with expansion, several tree-shaped segments emerge
          graph.pruningRule(graph.rule().probability(0.2))
               .expansionRule(graph.rule().probability(0.6))
               .setSeed(10101)
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0xAF4204DD69BB467C);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                    == 15);                        //
          CHECK (stat.segments                  ==  5);                        // this time the graph is segregated into 5 parts
          CHECK (stat.indicators[STAT_NODE].pS  == 6.4);                       // with 4 Nodes per segment
          CHECK (stat.indicators[STAT_FORK].sL  == 0.0);                       // where »Fork« is always placed at the beginning of each segment
          CHECK (stat.indicators[STAT_LINK].sL  == 0.5);                       // carry-on »Link« nodes in the very middle of the segment
          CHECK (stat.indicators[STAT_EXIT].sL  == 1.0);                       // and several »Exit« at the end
          CHECK (stat.indicators[STAT_EXIT].pS  == 2.6);                       // averaging 2.6 exits per segment (4·3 + 1)/5
          CHECK (stat.indicators[STAT_SEED].cnt ==  5);                        // so overall we get 8 »Seed« nodes
          CHECK (stat.indicators[STAT_FORK].cnt ==  5);                        //                   5 »Fork« nodes
          CHECK (stat.indicators[STAT_EXIT].cnt == 13);                        //                  13 »Exit« nodes
          CHECK (stat.indicators[STAT_LINK].cnt == 14);                        //              and 14 interconnecting links
          CHECK (stat.indicators[STAT_NODE].pL  == "2.1333333"_expect);        // leading to ∅ ~2 Nodes per level
          
          
          // however, by chance, with more randomised pruning points...
          graph.pruningRule(graph.rule().probability(0.2).shuffle(5))
               .expansionRule(graph.rule().probability(0.6))
               .setSeed(10101)
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0xF14A09EEFFEC7B18);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.segments                  ==  1);                        // ...the graph can evade severing altogether
          CHECK (stat.indicators[STAT_FORK].cnt ==  2);                        // with overall 2 »Fork«
          CHECK (stat.indicators[STAT_EXIT].cnt ==  9);                        //          and 9 »Exit« nodes
          CHECK (stat.indicators[STAT_EXIT].pL == "1.2857143"_expect);         // ∅ 1.3 exits per level
          CHECK (stat.indicators[STAT_NODE].pL == "4.5714286"_expect);         // ∅ 4.6 nodes per level
          

          graph.expansionRule(graph.rule()); // reset
          
          
          // combined with a special seeding rule,
          // which injects /another seed/ in the next level after each seed,
          // an equilibrium of chain seeding and termination can be achieved...
          graph.seedingRule(graph.rule_atStart(1))
               .pruningRule(graph.rule().probability(0.2))
               .setSeed(10101)
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x3965FAC3B9A2A545);
          
          // NOTE: this example produced 10 disjoint graph parts,
          //       which however start and end interleaved
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                    == 13);                        // Generation carries on for 13 levels
          CHECK (stat.segments                  ==  1);                        // NOTE: the detection of segments FAILS here (due to interleaved starts)
          CHECK (stat.indicators[STAT_SEED].cnt == 11);                        // 11 »Seed« nodes
          CHECK (stat.indicators[STAT_EXIT].cnt == 10);                        // 10 »Exit« nodes
          CHECK (stat.indicators[STAT_LINK].cnt == 10);                        // 10 interconnecting links
          CHECK (stat.indicators[STAT_JOIN].cnt ==  1);                        // and one additional »Join«
          CHECK (stat.indicators[STAT_JOIN].cL  == "0.91666667"_expect);       // ....appended at graph completion
          CHECK (stat.indicators[STAT_NODE].pL  == "2.4615385"_expect);        // overall ∅ 2½ nodes per level
          CHECK (stat.indicators[STAT_NODE].cL  == "0.48697917"_expect);       // with generally levelled distribution
          CHECK (stat.indicators[STAT_SEED].cL  == "0.41666667"_expect);       // also for the seeds
          CHECK (stat.indicators[STAT_EXIT].cL  == "0.55"_expect);             // and the exits
          
          
          // The next example is »interesting« insofar it shows self-similarity
          // The generation is entirely repetitive and locally predictable,
          // producing an ongoing sequence of small graph segments,
          // partially overlapping with interwoven starts.
          graph.seedingRule(graph.rule().fixedVal(1))
               .pruningRule(graph.rule().probability(0.5))
               .reductionRule(graph.rule().probability(0.8).maxVal(4))
               .setSeed(10101)
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x20546083916F7521);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                    == 9);                         // Generation carries on for 13 levels
          CHECK (stat.indicators[STAT_JOIN].pL  == 1);                         // with one »Join« event per level on average
          CHECK (stat.indicators[STAT_SEED].cnt == 21);                        // seeds are injected with /fixed rate/, meaning that
          CHECK (stat.indicators[STAT_SEED].pL  == "2.3333333"_expect);        // there is one additional seed for every node in previous level
        }
      
      
      
      
      
      /** @test examples of realistic stable processing patterns
       *      - some cases achieve a real equilibrium
       *      - other examples' structure is slowly expanding
       *        and become stable under constriction of width
       *      - some examples go into a stable repetitive loop
       *      - injecting additional randomness generates a
       *        chaotic yet stationary flow of similar patterns
       * @note these examples use a larger pre-allocation of nodes
       *       to demonstrate the stable state; because, towards end,
       *       a tear-down into one single exit node will be enforced.
       * @remark creating any usable example is a matter of experimentation;
       *       the usual starting point is to balance expanding and contracting
       *       forces; yet generation can either run-away or suffocate, and
       *       so the task is to find a combination of seed values and slight
       *       parameter variations leading into repeated re-establishment
       *       of some node constellation. When this is achieved, additional
       *       shuffling can be introduced to uncover further potential.
       */
      void
      showcase_StablePattern()
        {
          ChainLoad16 graph{256};
          
          // This example creates a repetitive, non-expanding stable pattern
          // comprised of four small graph segments, generated interleaved
          // Explanation: rule_atLink() triggers when the preceding node is a »Link«
          graph.seedingRule(graph.rule_atLink(1))
               .pruningRule(graph.rule().probability(0.4))
               .reductionRule(graph.rule().probability(0.6).maxVal(5).minVal(2))
               .setSeed(23)
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0xCAFA895DF9BDFB70);
          
          auto stat = graph.computeGraphStatistics();
          CHECK (stat.indicators[STAT_NODE].cL   == "0.49970598"_expect);      // The resulting distribution of nodes is stable and even
          CHECK (stat.levels                     == 94);                       // ...arranging the 256 nodes into 94 levels
          CHECK (stat.indicators[STAT_NODE].pL   == "2.7234043"_expect);       // ...with ∅ 2.7 nodes per level
          CHECK (stat.indicators[STAT_SEED].pL   == "1.0319149"_expect);       // comprised of ∅ 1 seed per level
          CHECK (stat.indicators[STAT_JOIN].pL   == "0.4787234"_expect);       //            ~ ∅ ½ join per level
          CHECK (stat.indicators[STAT_EXIT].pL   == "0.32978723"_expect);      //            ~ ∅ ⅓ exit per level
          CHECK (stat.indicators[STAT_SEED].frac == "0.37890625"_expect);      // overall, 38% nodes are seeds
          CHECK (stat.indicators[STAT_EXIT].frac == "0.12109375"_expect);      //      and 12% are exit nodes
          CHECK (stat.indicators[STAT_SEED].cLW  == "0.47963675"_expect);      // the density centre of all node kinds
          CHECK (stat.indicators[STAT_LINK].cLW  == "0.49055446"_expect);      //  ...is close to the middle
          CHECK (stat.indicators[STAT_JOIN].cLW  == "0.53299599"_expect);
          CHECK (stat.indicators[STAT_EXIT].cLW  == "0.55210026"_expect);
          
          
          
          // with only a slight increase in pruning probability
          // the graph goes into a stable repetition loop rather,
          // repeating a single shape with 3 seeds, 3 links and one 3-fold join as exit
          graph.seedingRule(graph.rule_atLink(1))
               .pruningRule(graph.rule().probability(0.5))
               .reductionRule(graph.rule().probability(0.6).maxVal(5).minVal(2))
               .setSeed(23)
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x38788543EA81C664);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                     == 78);                       //
          CHECK (stat.indicators[STAT_NODE].pL   == "3.2820513"_expect);       // ∅ 3.3 nodes per level
          CHECK (stat.indicators[STAT_SEED].frac == "0.41796875"_expect);      // 42% seed
          CHECK (stat.indicators[STAT_EXIT].frac == "0.140625"_expect);        // 14% exit
          
          
          
          // The next example uses a different generation approach:
          // Here, seeding happens randomly, while every join immediately
          // forces a prune, so all joins become exit nodes.
          // With a reduction probability slightly over seed, yet limited reduction strength
          // the generation goes into a stable repetition loop, yet with rather small graphs,
          // comprised each of two seeds, two links and a single 2-fold join at exit,
          // with exit and the two seeds of the following graph happening simultaneously.
          graph.seedingRule(graph.rule().probability(0.6).maxVal(1))
               .reductionRule(graph.rule().probability(0.75).maxVal(3))
               .pruningRule(graph.rule_atJoin(1))
               .setSeed(47)
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0xBD309241A343FD43);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                     == 104);                       //
          CHECK (stat.indicators[STAT_NODE].pL   == "2.4615385"_expect);        // ∅ 2.5 nodes per level
          CHECK (stat.indicators[STAT_SEED].frac == "0.40234375"_expect);       // 40% seed
          CHECK (stat.indicators[STAT_EXIT].frac == "0.1953125"_expect);        // 20% exit
          CHECK (stat.indicators[STAT_SEED].pL   == "0.99038462"_expect);       // resulting in 1 seed per level
          CHECK (stat.indicators[STAT_EXIT].pL   == "0.48076923"_expect);       //              ½ exit per level
          
          
          // »short_segments_interleaved«
          // Increased seed probability combined with overall seed value 0  ◁──── (crucial, other seeds produce larger graphs)
          // produces what seems to be the best stable repetition loop:
          // same shape as in preceding, yet interwoven by 2 steps
          graph.seedingRule(graph.rule().probability(0.8).maxVal(1))
               .reductionRule(graph.rule().probability(0.75).maxVal(3))
               .pruningRule(graph.rule_atJoin(1))
               .setSeed(0)
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x44860825C80753C4);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                     == 55);                       // much denser arrangement due to stronger interleaving
          CHECK (stat.indicators[STAT_NODE].pL   == "4.6545455"_expect);       // ∅ 4.7 nodes per level — almost twice as much
          CHECK (stat.indicators[STAT_SEED].frac == "0.3984375"_expect);       // 40% seed
          CHECK (stat.indicators[STAT_EXIT].frac == "0.19140625"_expect);      // 20% exit              — same fractions
          CHECK (stat.indicators[STAT_SEED].pL   == "1.8545455"_expect);       // 1.85 seed per level   — higher density
          CHECK (stat.indicators[STAT_EXIT].pL   == "0.89090909"_expect);      // 0.9 exit per level
          
          
          // With just the addition of irregularity through shuffling on the reduction,
          // a stable and tightly interwoven pattern of medium sized graphs is generated
          graph.seedingRule(graph.rule().probability(0.8).maxVal(1))
               .reductionRule(graph.rule().probability(0.75).maxVal(3).shuffle())
               .pruningRule(graph.rule_atJoin(1))
               .setSeed(0)
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x202F5F3801CB3A0A);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                   == 45);                         //
          CHECK (stat.indicators[STAT_NODE].pL == "5.6888889"_expect);         // ∅ 5.7 nodes per level
          CHECK (stat.indicators[STAT_SEED].pL == "2.3555556"_expect);         // ∅ 2.4 seeds
          CHECK (stat.indicators[STAT_LINK].pL == "2.4888889"_expect);         // ∅ 2.5 link nodes
          CHECK (stat.indicators[STAT_EXIT].pL == "0.82222222"_expect);        // ∅ 0.8 join/exit nodes — indicating stronger spread/reduction
          
          
          
          // This example uses another setup, without special rules;
          // rather, seed, reduction and pruning are tuned to balance each other.
          // The result is a regular interwoven pattern of very small graphs,
          // slowly expanding yet stable under constriction of width.
          // Predominant is a shape with two seeds on two levels, a single link and a 2-fold join;
          // caused by width constriction, this becomes complemented by larger compounds at intervals.
          graph.seedingRule(graph.rule().probability(0.8).maxVal(1))
               .reductionRule(graph.rule().probability(0.75).maxVal(3))
               .pruningRule(graph.rule().probability(0.55))
               .setSeed(55)   // ◁───────────────────────────────────────────── use 31 for width limited to 8 nodes
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x9E124CA52972CC66);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                     == 22);                       // ▶ resulting graph is very dense, hitting the parallelisation limit
          CHECK (stat.indicators[STAT_NODE].pL   == "11.636364"_expect);       // ∅ almost 12 nodes per level !
          CHECK (stat.indicators[STAT_SEED].pL   == "6.5454545"_expect);       // comprised of ∅ 6.5 seeds
          CHECK (stat.indicators[STAT_LINK].pL   == "2.2727273"_expect);       //              ∅ 2.3 links
          CHECK (stat.indicators[STAT_JOIN].pL   == "2.7272727"_expect);       //              ∅ 2.7 joins
          CHECK (stat.indicators[STAT_EXIT].pL   == "2.3636364"_expect);       //              ∅ 2.4 exits
          CHECK (stat.indicators[STAT_SEED].frac == "0.5625"_expect);          // 56% seed
          CHECK (stat.indicators[STAT_EXIT].frac == "0.203125"_expect);        // 20% exit
          
          
          
          // A slight parameters variation generates medium sized graphs, which are deep interwoven;
          // the generation is slowly expanding, but becomes stable under width constriction
          graph.seedingRule(graph.rule().probability(0.8).maxVal(1))
               .reductionRule(graph.rule().probability(0.6).maxVal(5).minVal(2))
               .pruningRule(graph.rule().probability(0.4))
               .setSeed(42)
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
             ;
          CHECK (graph.getHash() == 0xE0FF7A332B7335E5);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                     == 27);                       //
          CHECK (stat.indicators[STAT_NODE].pL   == "9.4814815"_expect);       // ∅ 9.5 nodes per level — ⅓ less dense
          CHECK (stat.indicators[STAT_SEED].frac == "0.3984375"_expect);       // 40% seed
          CHECK (stat.indicators[STAT_LINK].frac == "0.45703125"_expect);      // 45% link
          CHECK (stat.indicators[STAT_JOIN].frac == "0.11328125"_expect);      // 11% joins
          CHECK (stat.indicators[STAT_EXIT].frac == "0.08203125"_expect);      //  8% exits  — hinting at very strong reduction
          
          
          
          // The same setup with different seeing produces a
          // stable repetitive change of linear chain and small tree with 2 joins
          graph.seedingRule(graph.rule().probability(0.8).maxVal(2))
               .reductionRule(graph.rule().probability(0.6).maxVal(5).minVal(2))
               .pruningRule(graph.rule().probability(0.42))
               .setSeed(23)
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0xCD8B689C1514D40E);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                     == 130);                      //
          CHECK (stat.indicators[STAT_NODE].pL   == "1.9692308"_expect);       // ∅ ~2 nodes per level — much lesser density
          CHECK (stat.indicators[STAT_SEED].frac == "0.33203125"_expect);      // 33% seed
          CHECK (stat.indicators[STAT_LINK].frac == "0.41796875"_expect);      // 42% link
          CHECK (stat.indicators[STAT_JOIN].frac == "0.1640625"_expect);       // 16% join
          CHECK (stat.indicators[STAT_EXIT].frac == "0.16796875"_expect);      // 16% exit  — only a 2:1 reduction on average
          
          
          // With added shuffling in the seed rule, and under width constriction,
          // an irregular sequence of small to large and strongly interwoven graphs emerges.
          graph.seedingRule(graph.rule().probability(0.8).maxVal(2).shuffle())
               .reductionRule(graph.rule().probability(0.6).maxVal(5).minVal(2))
               .pruningRule(graph.rule().probability(0.42))
               .setSeed(23)
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x7DA33206D0773991);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                   == 21);                         // rather dense
          CHECK (stat.indicators[STAT_NODE].pL == "12.190476"_expect);         // ∅ 12.2 nodes per level
          CHECK (stat.indicators[STAT_SEED].pL == "7.2380952"_expect);         // ∅  7.2 seeds
          CHECK (stat.indicators[STAT_LINK].pL == "3.047619"_expect);          // ∅  3   links
          CHECK (stat.indicators[STAT_JOIN].pL == "1.8571429"_expect);         // ∅  1.9 joins
          CHECK (stat.indicators[STAT_EXIT].pL == "0.66666667"_expect);        // ∅  0.6 exits
          
          
          
          // The final example attempts to balance expansion and reduction forces.
          // Since reduction needs expanded nodes to work on, expansion always gets
          // a head start and we need to tune reduction to slightly higher strength
          // to ensure the graph width does not explode. The result is one single
          // graph with increasingly complex connections, which can expand into
          // width limitation at places, but also collapse to a single thread
          graph.expansionRule(graph.rule().probability(0.27).maxVal(4))
               .reductionRule(graph.rule().probability(0.44).maxVal(6).minVal(2))
               .seedingRule(graph.rule())
               .pruningRule(graph.rule())
               .setSeed(62)
               .buildToplolgy()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x259C7CA1B86E6C61);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                     == 31);                       // rather high concurrency
          CHECK (stat.indicators[STAT_SEED].cnt  ==  1);                       // a single seed
          CHECK (stat.indicators[STAT_EXIT].cnt  ==  1);                       // ...and exit
          CHECK (stat.indicators[STAT_NODE].pL   == "8.2580645"_expect);       // ∅ 8.25 nodes per level
          CHECK (stat.indicators[STAT_FORK].frac == "0.16015625"_expect);      // 16% forks
          CHECK (stat.indicators[STAT_LINK].frac == "0.76953125"_expect);      // 77% links
          CHECK (stat.indicators[STAT_JOIN].frac == "0.10546875"_expect);      // 10% joins
          CHECK (stat.indicators[STAT_KNOT].frac == "0.0390625"_expect);       //  3% »Knot« nodes which both join and fork
          CHECK (stat.indicators[STAT_FORK].cLW  == "0.41855453"_expect);      // density centre of forks lies earlier
          CHECK (stat.indicators[STAT_JOIN].cLW  == "0.70806275"_expect);      // while density centre of joins heavily leans towards end
        }
      
      
      
      
      
      
      
      
      /** @test verify calibration of a configurable computational load.
       */
      void
      verify_computation_load()
        {
          ComputationalLoad cpuLoad;
          CHECK (cpuLoad.timeBase == 100us);
          
          double micros = cpuLoad.invoke();
          CHECK (micros < 2000);
          CHECK (micros > 2);
          
          cpuLoad.calibrate();
          
          micros = cpuLoad.invoke();
          CHECK (micros < 133);
          CHECK (micros > 80);
          
          micros = cpuLoad.benchmark();
          CHECK (micros < 110);
          CHECK (micros > 90);
          
          cpuLoad.useAllocation = true;
          micros = cpuLoad.invoke();
          CHECK (micros < 133);
          CHECK (micros > 80);
          
          micros = cpuLoad.benchmark();
          CHECK (micros < 110);
          CHECK (micros > 90);
          
          cpuLoad.timeBase = 1ms;
          cpuLoad.sizeBase *= 100;
          cpuLoad.calibrate();
          
          cpuLoad.useAllocation = false;
          micros = cpuLoad.invoke();
          CHECK (micros > 900);
          micros = cpuLoad.invoke(5);
          CHECK (micros > 4600);
          micros = cpuLoad.invoke(10);
          CHECK (micros > 9500);
          micros = cpuLoad.invoke(100);
          CHECK (micros > 95000);
          
          cpuLoad.useAllocation = true;
          micros = cpuLoad.invoke();
          CHECK (micros > 900);
          micros = cpuLoad.invoke(5);
          CHECK (micros > 4600);
          micros = cpuLoad.invoke(10);
          CHECK (micros > 9500);
          micros = cpuLoad.invoke(100);
          CHECK (micros > 95000);
        }
      
      
      
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
       */
      void
      verify_reseed_recalculate()
        {
          ChainLoad16 graph{32};
          graph.expansionRule(graph.rule().probability(0.8).maxVal(1))
               .pruningRule(graph.rule().probability(0.6))
               .weightRule((graph.rule().probability(0.5)))
               .buildToplolgy();
          
          CHECK (8 == graph.allNodes().filter(isStartNode).count());
          CHECK (15 == graph.allNodes().filter(isExitNode).count());
          
          
          // verify computation of the globally combined exit hash
          auto exitHashes = graph.allNodes()
                                 .filter(isExitNode)
                                 .transform([](Node& n){ return n.hash; })
                                 .effuse();
          CHECK (15 == exitHashes.size());
          
          size_t combinedHash{0};
          for (uint i=0; i <15; ++i)
            boost::hash_combine (combinedHash, exitHashes[i]);
          
          CHECK (graph.getHash() == combinedHash);
          CHECK (graph.getHash() == 0x59AC21CFAE268613);
          
          
          // verify connectivity and local exit hashes
          graph.allNodePtr().grouped<4>()
               .foreach([&](auto group)
                 {  // verify wiring pattern
                   //  and the resulting exit hashes
                   auto& [a,b,c,d] = *group;
                   CHECK (isStart(a));
                   CHECK (isInner(b));
                   CHECK (not a->weight);
                   CHECK (not b->weight);
                   if (b->succ.size() == 2)
                     {
                       CHECK (isExit(c));
                       CHECK (isExit(d));
                       CHECK (c->hash == 0xAEDC04CFA2E5B999);
                       CHECK (d->hash == 0xAEDC04CFA2E5B999);
                       CHECK (c->weight == 4);
                       CHECK (d->weight == 4);
                     }
                   else
                     { // the last chunk is wired differently
                       CHECK (b->succ.size() == 1);
                       CHECK (b->succ[0] == c);
                       CHECK (isInner(c));
                       CHECK (isExit(d));
                       CHECK (graph.nodeID(d) == 31);
                       CHECK (d->hash == 0xC4AE6EB741C22FCE);
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
          CHECK (graph.getHash() == 0xA76EA46C6C004CA2);
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
                       CHECK (d->hash == 0x548F240CE91A291C);
                     }
                 });
          
          // seeding and recalculation are reproducible
          graph.setSeed(0).recalculate();
          CHECK (graph.getHash() == 0x59AC21CFAE268613);
          graph.setSeed(55).recalculate();
          CHECK (graph.getHash() == 0xA76EA46C6C004CA2);
        }
      
      
      
      /** @test compute synchronous execution time for reference
       */
      void
      verify_runtime_reference()
        {
          double t1 =
            TestChainLoad{64}
               .configureShape_short_segments3_interleaved()
               .buildToplolgy()
               .calcRuntimeReference();
          
          double t2 =
            TestChainLoad{64}
               .configureShape_short_segments3_interleaved()
               .buildToplolgy()
               .calcRuntimeReference(1ms);
          
          double t3 =
            TestChainLoad{256}
               .configureShape_short_segments3_interleaved()
               .buildToplolgy()
               .calcRuntimeReference();
          
          auto isWithin10Percent = [](double t, double r)
                                    {
                                      auto delta = abs (1.0 - t/r);
                                      return delta < 0.1;
                                    };
          
          // the test-graph has 64 Nodes,
          // each using the default load of 100µs
          CHECK (isWithin10Percent(t1, 6400));   // thus overall we should be close to 6.4ms
          CHECK (isWithin10Percent(t2, 10*t1));  // and the 10-fold load should yield 10-times
          CHECK (isWithin10Percent(t3,  4*t1));  // using 4 times as much nodes (64->256)
          
          // the time measurement uses a performance
          // which clears, re-seeds and calculates the complete graph
          auto graph =
            TestChainLoad{64}
               .configureShape_short_segments3_interleaved()
               .buildToplolgy();
          
          CHECK (graph.getHash() == 0xD2F292D864CF8086);
          
          graph.clearNodeHashes();
          CHECK (graph.getHash() == 0);
          
          // this is used by the timing benchmark
          graph.performGraphSynchronously();
          CHECK (graph.getHash() == 0xD2F292D864CF8086);
          
          graph.clearNodeHashes();
          CHECK (graph.getHash() == 0);
          
          graph.calcRuntimeReference();
          CHECK (graph.getHash() == 0xD2F292D864CF8086);
        }
      
      
      
      
      /** @test setup for running a chain-load as scheduled task
       *      - running an isolated Node recalculation
       *      - dispatch of this recalculation packaged as render job
       *      - verify the planning job, which processes nodes in batches;
       *        for the test, the callback-λ will not invoke the Scheduler,
       *        but rather use the instructions to create clone nodes;
       *        if all nodes are processed and all dependency connections
       *        properly reported through the callback-λ, then calculating
       *        this clone network should reproduce the original hash.
       */
      void
      verify_scheduling_setup()
        {
          array<Node,4> nodes;
          auto& [s,p1,p2,e] = nodes;
          s.addSucc(p1)
           .addSucc(p2);
          e.addPred(p1)
           .addPred(p2);
          s.level = 0;
          p1.level = p2.level = 1;
          e.level = 2;
          CHECK (e.hash == 0);
          for (Node& n : nodes)
            n.calculate();
          CHECK (e.hash == 0x6A5924BA3389D7C);
          
          
          // now do the same invoked as »render job«
          for (Node& n : nodes)
            n.hash = 0;
          s.level  = 0;
          p1.level = 1;
          p2.level = 1;
          e.level  = 2;
          
          RandomChainCalcFunctor<16> chainJob{nodes[0]};
          Job job0{chainJob
                  ,chainJob.encodeNodeID(0)
                  ,chainJob.encodeLevel(0)};
          Job job1{chainJob
                  ,chainJob.encodeNodeID(1)
                  ,chainJob.encodeLevel(1)};
          Job job2{chainJob
                  ,chainJob.encodeNodeID(2)
                  ,chainJob.encodeLevel(1)};
          Job job3{chainJob
                  ,chainJob.encodeNodeID(3)
                  ,chainJob.encodeLevel(2)};
          
          CHECK (e.hash == 0);
          job0.triggerJob();
          //   ◁───────────────────────────────────────────── Note: fail to invoke some predecessor....
          job2.triggerJob();
          job3.triggerJob();
          CHECK (e.hash != 0x6A5924BA3389D7C);
          
          e.hash = 0;
          job1.triggerJob(); // recalculate missing part of the graph...
          job3.triggerJob();
          CHECK (e.hash == 0x6A5924BA3389D7C);
          
          job3.triggerJob(); // Hash calculations are *not* idempotent
          CHECK (e.hash != 0x6A5924BA3389D7C);
          
          
          // use the »planing job« to organise the calculations:
          // Let the callbacks create a clone — which at the end should generate the same hash
          array<Node,4> clone;
          size_t lastTouched(-1);
          size_t lastNode (-1);
          size_t lastLevel(-1);
          bool shallContinue{false};
          auto getNodeIdx  = [&](Node* n) { return n - &nodes[0]; };
          
          // callback-λ rigged for test....
          // Instead of invoking the Scheduler, here we replicate the node structure
          auto disposeStep = [&](size_t idx, size_t level)
                                {
                                  Node& n = clone[idx];
                                  n.clear();
                                  n.level = level;
                                  lastTouched = idx;
                                };
          auto setDependency = [&](Node* pred, Node* succ)
                                {
                                  size_t predIdx = getNodeIdx(pred);
                                  size_t succIdx = getNodeIdx(succ);
                                  // replicate this relation into the clone array
                                  clone[predIdx].addSucc(clone[succIdx]);
                                };
          auto continuation = [&](size_t nodeDone, size_t levelDone, bool work_left)
                                {
                                  lastNode =nodeDone;
                                  lastLevel = levelDone;
                                  shallContinue = work_left;
                                };
          // build a JobFunctor for the planning step(s)
          RandomChainPlanFunctor<16> planJob{nodes.front(), nodes.size()
                                            ,disposeStep
                                            ,setDependency
                                            ,continuation};
          Job jobP1{planJob
                   ,planJob.encodeNodeID(1)
                   ,Time::ANYTIME};
          Job jobP2{planJob
                   ,planJob.encodeNodeID(5)
                   ,Time::ANYTIME};
          
          jobP1.triggerJob();
          CHECK (lastLevel = 1);
          CHECK (lastTouched = 2);
          CHECK (lastTouched == lastNode);
          Node* lastN = &clone[lastTouched];
          CHECK (lastN->level == lastLevel);
          CHECK (    isnil (lastN->succ));
          CHECK (not isnil (lastN->pred));
          CHECK (shallContinue);
          
          jobP2.triggerJob();
          CHECK (lastLevel = 3);
          CHECK (lastTouched = 3);
          CHECK (lastTouched == lastNode);
          lastN = &clone[lastTouched];
          CHECK (lastN->level == 2);
          CHECK (lastN->level < lastLevel);
          CHECK (    isnil (lastN->succ));
          CHECK (not isnil (lastN->pred));
          CHECK (not shallContinue);
          
          // all clone nodes should be wired properly now
          CHECK (lastN->hash == 0);
          for (Node& n : clone)
            n.calculate();
          CHECK (lastN->hash == 0x6A5924BA3389D7C);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TestChainLoad_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test