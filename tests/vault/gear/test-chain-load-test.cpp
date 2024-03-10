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
#include "lib/format-string.hpp"
#include "test-chain-load.hpp"
#include "vault/gear/job.h"
#include "lib/util.hpp"

#include <array>


using util::_Fmt;
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
          verify_adjusted_schedule();
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
               .buildTopology();
          
          // while building the graph, node hashes are computed
          CHECK (testLoad.getHash() == 0x554F5086DE5B0861);
          
          
          BlockFlowAlloc bFlow;
          EngineObserver watch;
          Scheduler scheduler{bFlow, watch};
          
          testLoad.setupSchedule(scheduler)
                  .launch_and_wait();
          
          // invocation through Scheduler has reproduced all node hashes
          CHECK (testLoad.getHash() == 0x554F5086DE5B0861);
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
                          .buildTopology();
          
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
               .buildTopology()
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
               .buildTopology()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x710D010554FEA614);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                    == 7);                         // expands faster, with only 7 levels
          CHECK (stat.indicators[STAT_NODE].pL  == "4.5714286"_expect);        // this time ∅ 4.6 Nodes / level
          CHECK (stat.indicators[STAT_FORK].cnt == 7);                         // 7 »Fork« events
          CHECK (stat.indicators[STAT_EXIT].cnt == 10);                        // but 10 »Exit« nodes....
          CHECK (stat.indicators[STAT_JOIN].cnt == 1);                         // and even one »Join« node....
          CHECK (stat.indicators[STAT_EXIT].cL  == 1);                         // which are totally concentrated towards end
          CHECK (stat.indicators[STAT_JOIN].cL  == 1);                         //  when nodes are exhausted
          
          
          // if the generation is allowed to run for longer,
          // while more constrained in width...
          TestChainLoad<8> gra_2{256};
          gra_2.expansionRule(gra_2.rule().probability(0.4).maxVal(2).shuffle(23))
               .buildTopology()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (gra_2.getHash() == 0x619491B22C3F8A6F);
          
          stat = gra_2.computeGraphStatistics();
          CHECK (stat.levels                     == 36);                       // much more levels, as can be expected
          CHECK (stat.indicators[STAT_NODE].pL   == "7.1111111"_expect);       // ∅ 7 Nodes per level
          CHECK (stat.indicators[STAT_JOIN].pL   == "0.77777778"_expect);      // but also almost one join per level to deal with the limitation
          CHECK (stat.indicators[STAT_FORK].frac == "0.24609375"_expect);      // 25% forks (there is just not enough room for more forks)
          CHECK (stat.indicators[STAT_JOIN].frac == "0.109375"_expect);        // and 10% joins
          CHECK (stat.indicators[STAT_EXIT].cnt  == 3);                        // ...leading to 3 »Exit« nodes
          CHECK (stat.indicators[STAT_EXIT].cL   == 1);                        // ....located at the very end
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
               .buildTopology()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x3E9BFAE5E686BEB4);
          
          auto stat = graph.computeGraphStatistics();
          CHECK (stat.levels                    == 8);                         // This connection pattern filled 8 levels
          CHECK (stat.indicators[STAT_JOIN].cnt == 4);                         // we got 4 »Join« events (reductions=
          CHECK (stat.indicators[STAT_FORK].cnt == 1);                         // and the single expansion/fork
          CHECK (stat.indicators[STAT_FORK].cL  == 0.0);                       // ...sitting right at the beginning
          CHECK (stat.indicators[STAT_NODE].cL  == "0.42857143"_expect);       // Nodes are concentrated towards the beginning
          
          
          // expansion and reduction can counterbalance each other
          graph.expansionRule(graph.rule().probability(0.2).maxVal(3).shuffle(555))
               .reductionRule(graph.rule().probability(0.2).maxVal(3).shuffle(555))
               .buildTopology()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0xB0335595D34F1D8D);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                    == 11);                        // This example runs a bit longer
          CHECK (stat.indicators[STAT_NODE].pL  == "2.9090909"_expect);        // in the middle threading 3-5 Nodes per Level
          CHECK (stat.indicators[STAT_FORK].cnt == 5);                         // with 5 expansions
          CHECK (stat.indicators[STAT_JOIN].cnt == 3);                         // and 3 reductions
          CHECK (stat.indicators[STAT_FORK].cL  == 0.5);                       // forks dominating earlier
          CHECK (stat.indicators[STAT_JOIN].cL  == "0.73333333"_expect);       // while joins need forks as prerequisite
          
          
          // expansion bursts can be balanced with a heightened reduction intensity
          graph.expansionRule(graph.rule().probability(0.3).maxVal(4).shuffle(555))
               .reductionRule(graph.rule().probability(0.9).maxVal(2).shuffle(555))
               .buildTopology()
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
               .buildTopology()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0xBC35A96B3CE1F39F);
          
          auto stat = graph.computeGraphStatistics();
          CHECK (stat.levels                    == 7);                         // 7 Levels...
          CHECK (stat.indicators[STAT_SEED].cnt == 12);                        // overall 12 »Seed« events generated several ongoing chains
          CHECK (stat.indicators[STAT_FORK].cnt == 0);                         // yet no branching/expanding
          CHECK (stat.indicators[STAT_LINK].cnt == 14);                        // thus more and more chains were just carried on
          CHECK (stat.indicators[STAT_LINK].pL == 2);                          // on average 2-3 per level are continuations
          CHECK (stat.indicators[STAT_NODE].pL == "4.5714286"_expect);         // leading to ∅ 4.5 Nodes per level
          CHECK (stat.indicators[STAT_NODE].cL == "0.734375"_expect);          // with nodes amassing towards the end
          CHECK (stat.indicators[STAT_LINK].cL == "0.64285714"_expect);        // because there are increasingly more links to carry-on
          CHECK (stat.indicators[STAT_JOIN].cL == 1);                          // while joining only happens at the very end
          
          
          // combining random seed nodes with reduction leads to a processing pattern
          // with side-chaines successively joined into a single common result
          graph.seedingRule(graph.rule().probability(0.2).maxVal(3).shuffle())
               .reductionRule(graph.rule().probability(0.9).maxVal(2))
               .buildTopology()
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
               .buildTopology()
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
               .buildTopology()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x1D0A7C39647340AA);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                    == 14);                        //
          CHECK (stat.segments                  ==  5);                        // this time the graph is segregated into 5 parts
          CHECK (stat.indicators[STAT_NODE].pS  == "6.4"_expect);              // with 4 Nodes per segment
          CHECK (stat.indicators[STAT_FORK].sL  == "0"_expect);                // where »Fork« is always placed at the beginning of each segment
          CHECK (stat.indicators[STAT_EXIT].sL  == "1"_expect);                // and several »Exit« at the end
          CHECK (stat.indicators[STAT_EXIT].pS  == "3"_expect);                // with always 3 exits per segment
          CHECK (stat.indicators[STAT_SEED].cnt ==  5);                        // so overall we get 5 »Seed« nodes
          CHECK (stat.indicators[STAT_FORK].cnt ==  5);                        //                   5 »Fork« nodes
          CHECK (stat.indicators[STAT_EXIT].cnt == 15);                        //                  15 »Exit« nodes
          CHECK (stat.indicators[STAT_LINK].cnt == 12);                        //              and 12 interconnecting links
          CHECK (stat.indicators[STAT_NODE].pL  == "2.2857143"_expect);        // leading to ∅ ~2 Nodes per level
          
          
          // however, by chance, with more randomised pruning points...
          graph.pruningRule(graph.rule().probability(0.2).shuffle(5))
               .expansionRule(graph.rule().probability(0.6))
               .setSeed(10101)
               .buildTopology()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x12BB22F76ECC5C1B);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.segments                  ==  1);                        // ...the graph can evade severing altogether
          CHECK (stat.indicators[STAT_FORK].cnt ==  3);                        // with overall 3 »Fork«
          CHECK (stat.indicators[STAT_EXIT].cnt == 10);                        //         and 10 »Exit« nodes
          CHECK (stat.indicators[STAT_EXIT].pL == "1.6666667"_expect);         // ∅ 1.6 exits per level
          CHECK (stat.indicators[STAT_NODE].pL == "5.3333333"_expect);         // ∅ 5.3 nodes per level
          

          graph.expansionRule(graph.rule()); // reset
          
          
          // combined with a special seeding rule,
          // which injects /another seed/ in the next level after each seed,
          // an equilibrium of chain seeding and termination can be achieved...
          graph.seedingRule(graph.rule_atStart(1))
               .pruningRule(graph.rule().probability(0.2))
               .setSeed(10101)
               .buildTopology()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0xBFFA04FE8202C708);
          
          // NOTE: this example produced 11 disjoint graph parts,
          //       which however start and end interleaved
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                    == 12);                        // Generation carries on for 12 levels
          CHECK (stat.segments                  ==  1);                        // NOTE: the detection of segments FAILS here (due to interleaved starts)
          CHECK (stat.indicators[STAT_SEED].cnt == 12);                        // 12 »Seed« nodes
          CHECK (stat.indicators[STAT_EXIT].cnt == 11);                        // 11 »Exit« nodes (including the isolated, last one)
          CHECK (stat.indicators[STAT_LINK].cnt == 10);                        // 10 interconnecting links
          CHECK (stat.indicators[STAT_JOIN].cnt ==  1);                        // and one additional »Join«
          CHECK (stat.indicators[STAT_JOIN].cL  == "1"_expect);                // ....appended at graph completion
          CHECK (stat.indicators[STAT_NODE].pL  == "2.6666667"_expect);        // overall ∅ 2⅔ nodes per level (converging ⟶ 3)
          CHECK (stat.indicators[STAT_NODE].cL  == "0.52840909"_expect);       // with generally levelled distribution
          CHECK (stat.indicators[STAT_SEED].cL  == "0.5"_expect);              // also for the seeds
          CHECK (stat.indicators[STAT_EXIT].cL  == "0.62809917"_expect);       // and the exits
          
          
          // The next example is »interesting« insofar it shows self-similarity
          // The generation is entirely repetitive and locally predictable,
          // producing an ongoing sequence of small graph segments,
          // partially overlapping with interwoven starts.
          graph.seedingRule(graph.rule().fixedVal(1))
               .pruningRule(graph.rule().probability(0.5))
               .reductionRule(graph.rule().probability(0.8).maxVal(4))
               .setSeed(10101)
               .buildTopology()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0xFB0A0EA9B7072507);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                    == 8);                         // Generation carries on for 13 levels
          CHECK (stat.indicators[STAT_JOIN].pL  == 1);                         // with one »Join« event per level on average
          CHECK (stat.indicators[STAT_SEED].cnt == 22);                        // seeds are injected with /fixed rate/, meaning that
          CHECK (stat.indicators[STAT_SEED].pL  == 2.75);                      // there is one additional seed for every node in previous level
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
               .buildTopology()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x6B5D7BD3130044E2);
          
          auto stat = graph.computeGraphStatistics();
          CHECK (stat.indicators[STAT_NODE].cL   == "0.50509511"_expect);      // The resulting distribution of nodes is stable and balanced
          CHECK (stat.levels                     == 93);                       // ...arranging the 256 nodes into 93 levels
          CHECK (stat.indicators[STAT_NODE].pL   == "2.7526882"_expect);       // ...with ∅ 2.7 nodes per level
          CHECK (stat.indicators[STAT_SEED].pL   == "1.0537634"_expect);       // comprised of ∅ 1 seed per level
          CHECK (stat.indicators[STAT_JOIN].pL   == "0.48387097"_expect);      //            ~ ∅ ½ join per level
          CHECK (stat.indicators[STAT_EXIT].pL   == "0.34408602"_expect);      //            ~ ∅ ⅓ exit per level
          CHECK (stat.indicators[STAT_SEED].frac == "0.3828125"_expect);       // overall, 38% nodes are seeds
          CHECK (stat.indicators[STAT_EXIT].frac == "0.125"_expect);           //      and ⅛ are exit nodes
          CHECK (stat.indicators[STAT_SEED].cLW  == "0.49273514"_expect);      // the density centre of all node kinds
          CHECK (stat.indicators[STAT_LINK].cLW  == "0.49588657"_expect);      //  ...is close to the middle
          CHECK (stat.indicators[STAT_JOIN].cLW  == "0.52481335"_expect);
          CHECK (stat.indicators[STAT_EXIT].cLW  == "0.55716297"_expect);
          
          
          
          // with only a slight increase in pruning probability
          // the graph goes into a stable repetition loop rather,
          // repeating a single shape with 3 seeds, 3 links and one 3-fold join as exit
          graph.seedingRule(graph.rule_atLink(1))
               .pruningRule(graph.rule().probability(0.5))
               .reductionRule(graph.rule().probability(0.6).maxVal(5).minVal(2))
               .setSeed(23)
               .buildTopology()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x20122CF2A1F301D1);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                     == 77);                       //
          CHECK (stat.indicators[STAT_NODE].pL   == "3.3246753"_expect);       // ∅ 3.3 nodes per level
          CHECK (stat.indicators[STAT_SEED].frac == "0.421875"_expect);        // 42% seed
          CHECK (stat.indicators[STAT_EXIT].frac == "0.14453125"_expect);      // 14% exit
          
          
          
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
               .buildTopology()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0xB58904674ED84031);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                     == 104);                       //
          CHECK (stat.indicators[STAT_NODE].pL   == "2.4615385"_expect);        // ∅ 2.5 nodes per level
          CHECK (stat.indicators[STAT_SEED].frac == "0.40234375"_expect);       // 40% seed
          CHECK (stat.indicators[STAT_EXIT].frac == "0.19921875"_expect);       // 20% exit
          CHECK (stat.indicators[STAT_SEED].pL   == "0.99038462"_expect);       // resulting in 1 seed per level
          CHECK (stat.indicators[STAT_EXIT].pL   == "0.49038462"_expect);       //              ½ exit per level
          
          
          // »short_segments_interleaved«
          // Increased seed probability combined with overall seed value 0  ◁──── (crucial, other seeds produce larger graphs)
          // produces what seems to be the best stable repetition loop:
          // same shape as in preceding, yet interwoven by 2 steps
          graph.seedingRule(graph.rule().probability(0.8).maxVal(1))
               .reductionRule(graph.rule().probability(0.75).maxVal(3))
               .pruningRule(graph.rule_atJoin(1))
               .setSeed(0)
               .buildTopology()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x11B57D9E98FDF6DF);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                     == 55);                       // much denser arrangement due to stronger interleaving
          CHECK (stat.indicators[STAT_NODE].pL   == "4.6545455"_expect);       // ∅ 4.7 nodes per level — almost twice as much
          CHECK (stat.indicators[STAT_SEED].frac == "0.3984375"_expect);       // 40% seed
          CHECK (stat.indicators[STAT_EXIT].frac == "0.1953125"_expect);       // 20% exit              — same fractions
          CHECK (stat.indicators[STAT_SEED].pL   == "1.8545455"_expect);       // 1.85 seed per level   — higher density
          CHECK (stat.indicators[STAT_EXIT].pL   == "0.90909091"_expect);      // 0.9 exit per level
          
          
          // With just the addition of irregularity through shuffling on the reduction,
          // a stable and tightly interwoven pattern of medium sized graphs is generated
          graph.seedingRule(graph.rule().probability(0.8).maxVal(1))
               .reductionRule(graph.rule().probability(0.75).maxVal(3).shuffle())
               .pruningRule(graph.rule_atJoin(1))
               .setSeed(0)
               .buildTopology()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x7C0453E7A4F6418D);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                   == 44);                         //
          CHECK (stat.indicators[STAT_NODE].pL == "5.8181818"_expect);         // ∅ 5.7 nodes per level
          CHECK (stat.indicators[STAT_SEED].pL == "2.4318182"_expect);         // ∅ 2.4 seeds
          CHECK (stat.indicators[STAT_LINK].pL == "2.4772727"_expect);         // ∅ 2.5 link nodes
          CHECK (stat.indicators[STAT_EXIT].pL == "1"_expect);                 // ∅ 1   join/exit nodes — indicating stronger spread/reduction
          
          
          
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
               .buildTopology()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x904A906B7859301A);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                     == 21);                       // ▶ resulting graph is very dense, hitting the parallelisation limit
          CHECK (stat.indicators[STAT_NODE].pL   == "12.190476"_expect);       // ∅ more than 12 nodes per level !
          CHECK (stat.indicators[STAT_SEED].pL   == "6.8571429"_expect);       // comprised of ∅ 6.9 seeds
          CHECK (stat.indicators[STAT_LINK].pL   == "2.3809524"_expect);       //              ∅ 2.4 links
          CHECK (stat.indicators[STAT_JOIN].pL   == "2.8095238"_expect);       //              ∅ 2.8 joins
          CHECK (stat.indicators[STAT_EXIT].pL   == "2.5714286"_expect);       //              ∅ 2.6 exits
          CHECK (stat.indicators[STAT_SEED].frac == "0.5625"_expect    );      // 56% seed
          CHECK (stat.indicators[STAT_EXIT].frac == "0.2109375"_expect);       // 21% exit
          
          
          
          // A slight parameters variation generates medium sized graphs, which are deep interwoven;
          // the generation is slowly expanding, but becomes stable under width constriction
          graph.seedingRule(graph.rule().probability(0.8).maxVal(1))
               .reductionRule(graph.rule().probability(0.6).maxVal(5).minVal(2))
               .pruningRule(graph.rule().probability(0.4))
               .setSeed(42)
               .buildTopology()
//             .printTopologyDOT()
//             .printTopologyStatistics()
             ;
          CHECK (graph.getHash() == 0x9453C56534FF9CD6);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                     == 26);                       //
          CHECK (stat.indicators[STAT_NODE].pL   == "9.8461538"_expect);       // ∅ 9.8 nodes per level — ⅓ less dense
          CHECK (stat.indicators[STAT_SEED].frac == "0.40234375"_expect);      // 40% seed
          CHECK (stat.indicators[STAT_LINK].frac == "0.453125"_expect);        // 45% link
          CHECK (stat.indicators[STAT_JOIN].frac == "0.109375"_expect );       // 11% joins
          CHECK (stat.indicators[STAT_EXIT].frac == "0.08984375"_expect);      //  8% exits  — hinting at very strong reduction
          
          
          // The same setup with different seeing produces a
          // stable repetitive change of linear chain and small tree with 2 joins
          graph.seedingRule(graph.rule().probability(0.8).maxVal(2))
               .reductionRule(graph.rule().probability(0.6).maxVal(5).minVal(2))
               .pruningRule(graph.rule().probability(0.42))
               .setSeed(23)
               .buildTopology()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0xA57727C2ED277C87);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                     == 129);                      //
          CHECK (stat.indicators[STAT_NODE].pL   == "1.9844961"_expect);       // ∅ ~2 nodes per level — much lesser density
          CHECK (stat.indicators[STAT_SEED].frac == "0.3359375"_expect);       // 33% seed
          CHECK (stat.indicators[STAT_LINK].frac == "0.4140625"_expect);       // 42% link
          CHECK (stat.indicators[STAT_JOIN].frac == "0.1640625"_expect);       // 16% join
          CHECK (stat.indicators[STAT_EXIT].frac == "0.171875"_expect);        // 17% exit  — only a 2:1 reduction on average
          
          
          // With added shuffling in the seed rule, and under width constriction,
          // an irregular sequence of small to large and strongly interwoven graphs emerges.
          graph.seedingRule(graph.rule().probability(0.8).maxVal(2).shuffle())
               .reductionRule(graph.rule().probability(0.6).maxVal(5).minVal(2))
               .pruningRule(graph.rule().probability(0.42))
               .setSeed(23)
               .buildTopology()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x4D0575F8BD269FC3);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                   == 20);                         // rather dense
          CHECK (stat.indicators[STAT_NODE].pL == "12.8"_expect);              // ∅ 12.8 nodes per level
          CHECK (stat.indicators[STAT_SEED].pL == "7.65"_expect);              // ∅  7.7 seeds
          CHECK (stat.indicators[STAT_LINK].pL == "3.15"_expect);              // ∅  3   links
          CHECK (stat.indicators[STAT_JOIN].pL == "1.9"_expect);               // ∅  1.9 joins
          CHECK (stat.indicators[STAT_EXIT].pL == "0.95"_expect);              // ∅ ~1   exit per level
          
          
          
          // »chain_loadBursts«
          // The final example attempts to balance expansion and reduction forces.
          // Since reduction needs expanded nodes to work on, expansion always gets
          // a head start and we need to tune reduction to slightly higher strength
          // to ensure the graph width does not explode. The result is one single
          // graph with increasingly complex connections, which can expand into
          // width limitation at places, but also collapse to a single thread.
          // The seed controls how fast the onset of the pattern happens.
          // low values -> long single-chain prelude
          // seed ≔ 55 -> prelude with 2 chains, then join, then onset at level 17
          // high values -> massive onset quickly going into saturation
          graph.expansionRule(graph.rule().probability(0.27).maxVal(4))
               .reductionRule(graph.rule().probability(0.44).maxVal(6).minVal(2))
               .seedingRule(graph.rule())
               .pruningRule(graph.rule())
               .setSeed(62)
               .buildTopology()
//             .printTopologyDOT()
//             .printTopologyStatistics()
               ;
          CHECK (graph.getHash() == 0x25114F8770B1B78E);
          
          stat = graph.computeGraphStatistics();
          CHECK (stat.levels                     == 30);                       // rather high concurrency
          CHECK (stat.indicators[STAT_SEED].cnt  ==  1);                       // a single seed
          CHECK (stat.indicators[STAT_EXIT].cnt  ==  4);                       // ...and 4 exit when running out of node space
          CHECK (stat.indicators[STAT_NODE].pL   == "8.5333333"_expect);       // ∅ 8.25 nodes per level
          CHECK (stat.indicators[STAT_FORK].frac == "0.16015625"_expect);      // 16% forks
          CHECK (stat.indicators[STAT_LINK].frac == "0.76171875"_expect);      // 77% links
          CHECK (stat.indicators[STAT_JOIN].frac == "0.1015625"_expect);       // 10% joins
          CHECK (stat.indicators[STAT_KNOT].frac == "0.0390625"_expect);       //  3% »Knot« nodes which both join and fork
          CHECK (stat.indicators[STAT_FORK].cLW  == "0.43298744"_expect);      // density centre of forks lies earlier
          CHECK (stat.indicators[STAT_JOIN].cLW  == "0.64466378"_expect);      // while density centre of joins leans rather towards end
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
       *         there are 8 such groups, each with a fork and two exit nodes.
       *         The following code traverses all nodes grouped into 4-node
       *         clusters to verify the regular pattern and calculated hashes.
       */
      void
      verify_reseed_recalculate()
        {
          ChainLoad16 graph{32};
          graph.expansionRule(graph.rule().probability(0.8).maxVal(1))
               .pruningRule(graph.rule().probability(0.6))
               .weightRule((graph.rule().probability(0.5)))
               .buildTopology();
          
          CHECK (8 == graph.allNodes().filter(isStartNode).count());
          CHECK (16 == graph.allNodes().filter(isExitNode).count());
          
          
          // verify computation of the globally combined exit hash
          auto exitHashes = graph.allNodes()
                                 .filter(isExitNode)
                                 .transform([](Node& n){ return n.hash; })
                                 .effuse();
          CHECK (16 == exitHashes.size());
          
          size_t combinedHash{0};
          for (uint i=0; i <16; ++i)
            boost::hash_combine (combinedHash, exitHashes[i]);
          
          CHECK (graph.getHash() == combinedHash);
          CHECK (graph.getHash() == 0x33B00C450215EB00);
          
          
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
                   CHECK (isExit(c));
                   CHECK (isExit(d));
                   CHECK (c->hash == 0xAEDC04CFA2E5B999);
                   CHECK (d->hash == 0xAEDC04CFA2E5B999);
                   CHECK (c->weight == 4);
                   CHECK (d->weight == 4);
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
          CHECK (graph.getHash() == 0x17427F67DBC8BCC0);
          graph.allNodePtr().grouped<4>()
               .foreach([&](auto group)
                 {  // verify hashes were recalculated
                   //  based on the new seed
                   auto& [a,b,c,d] = *group;
                   CHECK (a->hash == 55);
                   CHECK (c->hash == 0x7887993B0ED41395);
                   CHECK (d->hash == 0x7887993B0ED41395);
                 });
          
          // seeding and recalculation are reproducible
          graph.setSeed(0).recalculate();
          CHECK (graph.getHash() == 0x33B00C450215EB00);
          graph.setSeed(55).recalculate();
          CHECK (graph.getHash() == 0x17427F67DBC8BCC0);
        }
      
      
      
      /** @test compute synchronous execution time for reference
       */
      void
      verify_runtime_reference()
        {
          double t1 =
            TestChainLoad{64}
               .configureShape_short_segments3_interleaved()
               .buildTopology()
               .calcRuntimeReference();
          
          double t2 =
            TestChainLoad{64}
               .configureShape_short_segments3_interleaved()
               .buildTopology()
               .calcRuntimeReference(1ms);
          
          double t3 =
            TestChainLoad{256}
               .configureShape_short_segments3_interleaved()
               .buildTopology()
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
               .buildTopology();
          
          CHECK (graph.getHash() == 0x554F5086DE5B0861);
          
          graph.clearNodeHashes();
          CHECK (graph.getHash() == 0);
          
          // this is used by the timing benchmark
          graph.performGraphSynchronously();
          CHECK (graph.getHash() == 0x554F5086DE5B0861);
          
          graph.clearNodeHashes();
          CHECK (graph.getHash() == 0);
          
          graph.calcRuntimeReference();
          CHECK (graph.getHash() == 0x554F5086DE5B0861);
        }
      
      
      
      /** @test verify use of computation weights and topology to establish
       *        a predicted load pattern, which can be used to construct a
       *        schedule adapted to the expected load.
       * @remark use `printTopologyDOT()` and then `dot -Tpng xx.dot|display`
       *        to understand the numbers in context of the topology
       */
      void
      verify_adjusted_schedule()
        {
          TestChainLoad testLoad{64};
          testLoad.configureShape_chain_loadBursts()
                  .buildTopology()
//                .printTopologyDOT()
                  ;
          
          // compute aggregated level data....
          auto level = testLoad.allLevelWeights().effuse();
          CHECK (level.size() == 26);
          
          // visualise and verify this data......
          auto node  = testLoad.allNodePtr().effuse();
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
          
          // compute a weight factor for each level,
          // using the number of nodes, the weight sum and concurrency
          CHECK (level[19].nodes = 5);       // ╭────────────────────────╢ concurrency
          CHECK (level[19].weight = 10);    //  ▽                ╭───────╢ boost by concurrency
          CHECK (computeWeightFactor(level[19], 1) == 10.0);//   ▽
          CHECK (computeWeightFactor(level[19], 2) == 10.0 / (5.0/3));
          CHECK (computeWeightFactor(level[19], 3) == 10.0 / (5.0/2));
          CHECK (computeWeightFactor(level[19], 4) == 10.0 / (5.0/2));
          CHECK (computeWeightFactor(level[19], 5) == 10.0 / (5.0/1));
          
          // build a schedule sequence based on
          // summing up weight factors, with example concurrency ≔ 4
          uint concurrency = 4;
          auto steps = testLoad.levelScheduleSequence(concurrency).effuse();
          CHECK (steps.size() == 26);
          
          // for documentation/verification: show also the boost factor and the resulting weight factor
          auto boost = [&](uint i){ return level[i].nodes / std::ceil (double(level[i].nodes)/concurrency); };
          auto wfact = [&](uint i){ return computeWeightFactor(level[i], concurrency);                      };
          
          _Fmt stepFmt{"lev:%-2d  nodes:%-2d Σw:%2d %4.1f Δ%5.3f ▿▿ %6.3f"};
          auto stepStr = [&](uint i){ return string{stepFmt % i % level[i].nodes % level[i].weight % boost(i) % wfact(i) % steps[i]}; };

                 //                                    boost wfactor    steps
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
          CHECK (stepStr(25) == "lev:25  nodes:3  Σw: 4  3.0 Δ1.333 ▿▿ 32.200"_expect);
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
          auto continuation = [&](size_t, size_t nodeDone, size_t levelDone, bool work_left)
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
