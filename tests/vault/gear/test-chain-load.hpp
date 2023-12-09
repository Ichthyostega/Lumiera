/*
  TEST-CHAIN-LOAD.hpp  -  produce a configurable synthetic computation load

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

*/

/** @file test-chain-load.hpp
 ** Generate synthetic computation load for Scheduler performance tests.
 ** The [Scheduler](\ref scheduler.hpp) is a service to invoke Render Job instances
 ** concurrently in accordance to a time plan. To investigate the runtime and performance
 ** characteristics of the implementation, a well-defined artificial computation load is
 ** necessary, comprised of the invocation of an extended number of Jobs, each configured
 ** to carry out a reproducible computation. Data dependencies between jobs can be established
 ** to verify handling of dependent jobs and job completion messages within the scheduler.
 ** 
 ** # Random computation structure
 ** A system of connected hash values is used as computation load, akin to a blockchain.
 ** Each processing step is embodied into a node, with a hash value computed by combining
 ** all predecessor nodes. Connectivity is represented as bidirectional pointer links, each
 ** nodes knows its predecessors and successors (if any), while the maximum _fan out_ or
 ** _fan in_ and the total number of nodes is limited statically. All nodes are placed
 ** into a single pre-allocated memory block and always processed in ascending dependency
 ** order. The result hash from complete processing can thus be computed by a single linear
 ** pass over all nodes; yet alternatively each node can be _scheduled_ as an individual
 ** computation job, which obviously requires that it's predecessor nodes have already
 ** been computed, otherwise the resulting hash will not match up with expectation.
 ** If significant per-node computation time is required, the hashing can be
 ** arbitrarily repeated at each node.
 ** 
 ** The topology of connectivity is generated randomly, yet completely deterministic through
 ** configurable _control functions_ driven by each node's (hash)value. This way, each node
 ** can optionally fork out to several successor nodes, but can also reduce and combine its
 ** predecessor nodes; additionally, new chains can be spawned (to simulate the effect of
 ** data loading Jobs without predecessor) and chains can be deliberately pruned, possibly
 ** splitting the computation into several disjoint sub-graphs. Anyway, the computation always
 ** begins with the _root node_, proceeds over the node links and finally connects any open
 ** chains of computation to the _top node,_ leaving no dead end. The probabilistic rules
 ** controlling the topology can be configured using the lib::RandomDraw component, allowing
 ** either just to set a fixed probability or to define elaborate dynamic configurations
 ** based on the graph height or node connectivity properties.
 ** 
 ** ## Usage
 ** A TestChainLoad instance is created with predetermined maximum fan factor and a fixed
 ** number of nodes, which are immediately allocated and initialised. Using _builder notation,_
 ** control functions can then be configured. The [topology generation](\ref TestChainLoad::buildTopology)
 ** then traverses the nodes, starting with the seed value from the root node, and establishes
 ** the complete node connectivity. After this priming, the expected result hash should be
 ** [retrieved](\ref TestChainLoad::getHash). The node structure can than be traversed or
 ** [scheduled as Render Jobs](\ref TestChainLoad::scheduleJobs).
 ** 
 ** ## Observation tools
 ** The generated topology can be visualised as a graph, using the Graphviz-DOT language.
 ** Nodes are rendered from bottom to top, organised into strata according to the time-level
 ** and showing predecessor -> successor connectivity. Seed nodes are distinguished by
 ** circular shape.
 ** 
 ** @see TestChainLoad_test
 ** @see SchedulerStress_test
 ** @see random-draw.hpp
 */


#ifndef VAULT_GEAR_TEST_TEST_CHAIN_LOAD_H
#define VAULT_GEAR_TEST_TEST_CHAIN_LOAD_H


#include "vault/common.hpp"
#include "lib/test/transiently.hpp"

//#include "lib/hash-value.h"
#include "vault/gear/job.h"
#include "vault/gear/scheduler.hpp"
#include "vault/gear/special-job-fun.hpp"
//#include "vault/gear/nop-job-functor.hpp"
#include "lib/uninitialised-storage.hpp"
#include "lib/time/timevalue.hpp"
//#include "lib/meta/variadic-helper.hpp"
//#include "lib/meta/function.hpp"
//#include "lib/wrapper.hpp"
#include "lib/time/quantiser.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/random-draw.hpp"
#include "lib/dot-gen.hpp"
#include "lib/util.hpp"

#include <boost/functional/hash.hpp>
#include <functional>
#include <utility>
#include <future>
#include <memory>
#include <string>
#include <vector>
#include <tuple>
#include <array>


namespace vault{
namespace gear {
namespace test {
  
  using std::string;
  using std::function;
  using lib::time::Time;
  using lib::time::TimeValue;
  using lib::time::FrameRate;
  using lib::time::Duration;
//  using lib::time::FSecs;
//  using lib::time::Offset;
//  using lib::meta::RebindVariadic;
  using util::_Fmt;
  using util::min;
  using util::max;
  using util::isnil;
  using util::limited;
  using util::unConst;
  using util::toString;
  using util::showHashLSB;
  using lib::meta::_FunRet;
  using lib::test::Transiently;

  using std::forward;
  using std::string;
  using std::swap;
  using std::move;
//  using boost::hash_combine;
  using std::chrono_literals::operator ""s;
  
  namespace err = lumiera::error;
  namespace dot = lib::dot_gen;
  
  namespace { // Default definitions for topology generation
    const size_t DEFAULT_FAN = 16;
    const size_t DEFAULT_SIZ = 256;
    
    const auto SAFETY_TIMEOUT = 5s;
    const auto STANDARD_DEADLINE = 10ms;
    const size_t DEFAULT_CHUNKSIZE = 64;
    const microseconds PLANNING_TIME_PER_NODE = 80us;
  }
  
  struct Statistic;
  
  
  
  
  /***********************************************************************//**
   * A Generator for synthetic Render Jobs for Scheduler load testing.
   * Allocates a fixed set of #numNodes and generates connecting topology.
   * @tparam maxFan maximal fan-in/out from a node, also limits maximal parallel strands.
   * @see TestChainLoad_test
   */
  template<size_t maxFan =DEFAULT_FAN>
  class TestChainLoad
    : util::MoveOnly
    {
          
    public:
      /** Graph Data structure */
      struct Node
        : util::MoveOnly
        {
          using _Arr = std::array<Node*, maxFan>;
          using Iter = typename _Arr::iterator;
          using CIter = typename _Arr::const_iterator;
          
          /** Table with connections to other Node records */
          struct Tab : _Arr
            {
              Iter after = _Arr::begin();
              
              Iter  end()      { return after; }
              CIter end() const{ return after; }
              friend Iter  end (Tab      & tab){ return tab.end(); }
              friend CIter end (Tab const& tab){ return tab.end(); }
              
              Node* front() { return empty()? nullptr : _Arr::front(); }
              Node* back()  { return empty()? nullptr : *(after-1);    }
              
              void clear() { after = _Arr::begin(); }      ///< @warning pointer data in array not cleared
              
              size_t size() const { return unConst(this)->end()-_Arr::begin(); }
              bool  empty() const { return 0 == size();     }
              
              Iter
              add(Node* n)
                {
                  if (after != _Arr::end())
                    {
                      *after = n;
                      return after++;
                    }
                  NOTREACHED ("excess node linkage");
                }
              
            };
          
          size_t hash;
          size_t level{0}, repeat{0};
          Tab pred{0}, succ{0};
          
          Node(size_t seed =0)
            : hash{seed}
            { }
          
          void
          clear()
            {
              hash = 0;
              level = repeat = 0;
              pred.clear();
              succ.clear();
            }
          
          Node&
          addPred (Node* other)
            {
              REQUIRE (other);
              pred.add (other);
              other->succ.add (this);
              return *this;
            }
          
          Node&
          addSucc (Node* other)
            {
              REQUIRE (other);
              succ.add (other);
              other->pred.add (this);
              return *this;
            }
          Node& addPred(Node& other) { return addPred(&other); }
          Node& addSucc(Node& other) { return addSucc(&other); }
          
          size_t
          calculate()
            {
              for (Node* entry: pred)
                boost::hash_combine (hash, entry->hash);
              return hash;
            }
          
          friend bool isStart (Node const& n) { return isnil (n.pred); };
          friend bool isExit  (Node const& n) { return isnil (n.succ); };
          friend bool isInner (Node const& n) { return not (isStart(n) or isExit(n)); }
          friend bool isFork  (Node const& n) { return 1 < n.succ.size(); }
          friend bool isJoin  (Node const& n) { return 1 < n.pred.size(); }
          friend bool isLink  (Node const& n) { return 1 == n.pred.size() and 1 == n.succ.size(); }
          friend bool isKnot  (Node const& n) { return isFork(n) and isJoin(n); }

          
          friend bool isStart (Node const* n) { return n and isStart(*n); };
          friend bool isExit  (Node const* n) { return n and isExit (*n); };
          friend bool isInner (Node const* n) { return n and isInner(*n); };
          friend bool isFork  (Node const* n) { return n and isFork (*n); };
          friend bool isJoin  (Node const* n) { return n and isJoin (*n); };
          friend bool isLink  (Node const* n) { return n and isLink (*n); };
          friend bool isKnot  (Node const* n) { return n and isKnot (*n); };
        };
        
        
        /** link Node.hash to random parameter generation */
        class NodeControlBinding;
        
        /** Parameter values limited [0 .. maxFan] */
        using Param = lib::Limited<size_t, maxFan>;
        
        /** Topology is governed by rules for random params */
        using Rule = lib::RandomDraw<NodeControlBinding>;
      
    private:
      using NodeTab = typename Node::Tab;
      using NodeIT = lib::RangeIter<Node*>;
      
      std::unique_ptr<Node[]> nodes_;
      size_t numNodes_;
      
      Rule seedingRule_  {};
      Rule expansionRule_{};
      Rule reductionRule_{};
      Rule pruningRule_  {};
      
      Node* frontNode() { return &nodes_[0];          }
      Node* afterNode() { return &nodes_[numNodes_];  }
      Node* backNode()  { return &nodes_[numNodes_-1];}
      
    public:
      explicit
      TestChainLoad(size_t nodeCnt =DEFAULT_SIZ)
        : nodes_{new Node[nodeCnt]}
        , numNodes_{nodeCnt}
        {
          REQUIRE (1 < nodeCnt);
        }
      
      
      size_t size()     const { return numNodes_; }
      size_t topLevel() const { return unConst(this)->backNode()->level; }
      size_t getSeed()  const { return unConst(this)->frontNode()->hash; }
      
      
      auto
      allNodes()
        {
          return lib::explore (NodeIT{frontNode(),afterNode()});
        }
      auto
      allNodePtr()
        {
          return allNodes().asPtr();
        }
      
      auto
      allExitNodes()
        {
          return allNodes().filter([](Node& n){ return isExit(n); });
        }
      auto
      allExitHashes()  const
        {
          return unConst(this)->allExitNodes().transform([](Node& n){ return n.hash; });
        }
      
      /** global hash is the combination of all exit node hashes != 0 */
      size_t
      getHash()  const
        {
          auto combineBoostHashes = [](size_t h, size_t hx){ boost::hash_combine(h,hx); return h;};
          return allExitHashes()
                    .filter([](size_t h){ return h != 0; })
                    .reduce(lib::iter_explorer::IDENTITY
                           ,combineBoostHashes
                           );
        }
      
      
      /** @return the node's index number, based on its storage location */
      size_t nodeID(Node const* n){ return n - frontNode(); };
      size_t nodeID(Node const& n){ return nodeID (&n); };
      
      
      
      /* ===== topology control ===== */
      
      TestChainLoad&&
      seedingRule (Rule r)
        {
          seedingRule_ = move(r);
          return move(*this);
        }
      
      TestChainLoad&&
      expansionRule (Rule r)
        {
          expansionRule_ = move(r);
          return move(*this);
        }
      
      TestChainLoad&&
      reductionRule (Rule r)
        {
          reductionRule_ = move(r);
          return move(*this);
        }
      
      TestChainLoad&&
      pruningRule (Rule r)
        {
          pruningRule_ = move(r);
          return move(*this);
        }
      
      
      /** Abbreviation for starting rules */
      static Rule rule() { return Rule(); }
      
      static Rule
      rule_atStart (uint v)
        {
          return Rule().mapping([v](Node* n)
                                  {
                                    return isStart(n)? Rule().fixedVal(v)
                                                     : Rule();
                                  });
        }
      
      static Rule
      rule_atJoin (uint v)
        {
          return Rule().mapping([v](Node* n)
                                  {
                                    return isJoin(n) ? Rule().fixedVal(v)
                                                     : Rule();
                                  });
        }
      
      static Rule
      rule_atLink (uint v)
        {
          return Rule().mapping([v](Node* n)
                                  { // NOTE: when applying these rules,
                                    //       successors are not yet wired...
                                    return not (isJoin(n) or isStart(n))
                                                     ? Rule().fixedVal(v)
                                                     : Rule();
                                  });
        }
      
      static Rule
      rule_atJoin_else (double p1, double p2, uint v=1)
        {
          return Rule().mapping([p1,p2,v](Node* n)
                                  {
                                    return isJoin(n) ? Rule().probability(p1).maxVal(v)
                                                     : Rule().probability(p2).maxVal(v);
                                  });
        }
      
      
      /** preconfigured topology: simple interwoven 3-step graph segments */
      TestChainLoad&&
      configureShape_simple_short_segments()
        {
          seedingRule(rule().probability(0.8).maxVal(1));
          reductionRule(rule().probability(0.75).maxVal(3));
          pruningRule(rule_atJoin(1));
          return move(*this);
        }

      
      
      /**
       * Use current configuration and seed to (re)build Node connectivity.
       * While working in-place, the wiring and thus the resulting hash values
       * are completely rewritten, progressing from start and controlled by
       * evaluating the _drawing rules_ on the current node, computing its hash.
       */
      TestChainLoad&&
      buildToplolgy()
        {
          NodeTab a,b,          // working data for generation
                 *curr{&a},     // the current set of nodes to carry on
                 *next{&b};     // the next set of nodes connected to current
          Node* node = frontNode();
          size_t level{0};
          
          // transient snapshot of rules (non-copyable, once engaged)
          Transiently originalExpansionRule{expansionRule_};
          Transiently originalReductionRule{reductionRule_};
          Transiently originalseedingRule  {seedingRule_};
          Transiently originalPruningRule  {pruningRule_};
          
          // prepare building blocks for the topology generation...
          auto moreNext  = [&]{ return next->size() < maxFan;      };
          auto moreNodes = [&]{ return node < backNode();          };
          auto spaceLeft = [&]{ return moreNext() and moreNodes(); };
          auto addNode   = [&](size_t seed =0)
                              {
                                Node* n = *next->add (node++);
                                n->clear();
                                n->level = level;
                                n->hash = seed;
                                return n;
                              };
          auto apply  = [&](Rule& rule, Node* n)
                              {
                                return rule(n);
                              };
          
          // visit all further nodes and establish links
          while (moreNodes())
            {
              curr->clear();
              swap (next, curr);
              size_t toReduce{0};
              Node* r = nullptr;
              REQUIRE (spaceLeft());
              for (Node* o : *curr)
                { // follow-up on all Nodes in current level...
                  o->calculate();
                  if (apply (pruningRule_,o))
                    continue; // discontinue
                  size_t toSeed   = apply (seedingRule_, o);
                  size_t toExpand = apply (expansionRule_,o);
                  while (0 < toSeed and spaceLeft())
                    { // start a new chain from seed
                      addNode(this->getSeed());
                      --toSeed;
                    }
                  while (0 < toExpand and spaceLeft())
                    { // fork out secondary chain from o
                      Node* n = addNode();
                      o->addSucc(n);
                      --toExpand;
                    }
                  if (not toReduce)
                    {          // carry-on chain from o
                      r = spaceLeft()? addNode():nullptr;
                      toReduce = apply (reductionRule_, o);
                    }
                  else
                    --toReduce;
                  if (r) // connect chain from o...
                    r->addPred(o);
                  else // space for successors is already exhausted
                    { //  can not carry-on, but must ensure no chain is broken
                      ENSURE (not next->empty());
                      if (o->succ.empty())
                        o->addSucc (next->back());
                    }
                }
              ENSURE (not isnil(next) or spaceLeft());
              if (isnil(next)) // ensure graph continues
                addNode(this->getSeed());
              ENSURE (not next->empty());
              ++level;
            }
          ENSURE (node == backNode());
          // connect ends of all remaining chains to top-Node
          node->clear();
          node->level = level;
          for (Node* o : *next)
            {
              o->calculate();
              node->addPred(o);
            }
          node->calculate();
          //
          return move(*this);
        }
      
      
      /**
       * Set the overall seed value.
       * @note does not propagate seed to consecutive start nodes
       */
      TestChainLoad&&
      setSeed (size_t seed = rand())
        {
          frontNode()->hash = seed;
          return move(*this);
        }
      
      
      /**
       * Recalculate all node hashes and propagate seed value.
       */
      TestChainLoad&&
      recalculate()
        {
          size_t seed = this->getSeed();
          for (Node& n : allNodes())
            {
              n.hash = isStart(n)? seed : 0;
              n.calculate();
            }
          return move(*this);
        }
      
      
      /**
       * Clear node hashes and propagate seed value.
       */
      TestChainLoad&&
      clearNodeHashes()
        {
          size_t seed = this->getSeed();
          for (Node& n : allNodes())
            n.hash = isStart(n)? seed : 0;
          return move(*this);
        }
      
      
      
      /* ===== Operators ===== */
      
      std::string
      generateTopologyDOT()
        {
          using namespace dot;
          
          Section nodes("Nodes");
          Section layers("Layers");
          Section topology("Topology");
          
          // Styles to distinguish the computation nodes
          Code BOTTOM{"shape=doublecircle"};
          Code SEED  {"shape=circle"};
          Code TOP   {"shape=box, style=rounded"};
          Code DEFAULT{};
          
          // prepare time-level zero
          size_t level(0);
          auto timeLevel = scope(level).rank("min ");
          
          for (Node& n : allNodes())
            {
              size_t i = nodeID(n);
              nodes += node(i).label(toString(i)+": "+showHashLSB(n.hash))
                              .style(i==0         ? BOTTOM
                                    :isnil(n.pred)? SEED
                                    :isnil(n.succ)? TOP
                                    :               DEFAULT);
              for (Node* suc : n.succ)
                topology += connect (i, nodeID(*suc));
              
              if (level != n.level)
                {// switch to next time-level
                  layers += timeLevel;
                  ++level;
                  ENSURE (level == n.level);
                  timeLevel = scope(level).rank("same");
                }
              timeLevel.add (node(i));
            }
          layers += timeLevel; // close last layer
          
          // combine and render collected definitions as DOT-code
          return digraph (nodes, layers, topology);
        }
      
      TestChainLoad&&
      printTopologyDOT()
        {
          cout << "───═══───═══───═══───═══───═══───═══───═══───═══───═══───═══───\n"
               << generateTopologyDOT()
               << "───═══───═══───═══───═══───═══───═══───═══───═══───═══───═══───"
               << endl;
          return move(*this);
        }
      
      
      Statistic computeGraphStatistics();
      TestChainLoad&& printTopologyStatistics();
      
      class ScheduleCtx;
      friend class ScheduleCtx; // accesses raw storage array
      
      ScheduleCtx setupSchedule (Scheduler& scheduler);
    };
  
  
  
  /**
   * Policy/Binding for generation of [random parameters](\ref TestChainLoad::Param)
   * by [»drawing«](\ref random-draw.hpp) based on the [node-hash](\ref TestChainLoad::Node).
   * Notably this policy template maps the ways to spell out [»Ctrl rules«](\ref TestChainLoad::Rule)
   * to configure the probability profile of the topology parameters _seeding, expansion, reduction
   * and pruning._ The RandomDraw component used to implement those rules provides a builder-DSL
   * and accepts λ-bindings in various forms to influence mapping of Node hash into result parameters.
   */
  template<size_t maxFan>
  class TestChainLoad<maxFan>::NodeControlBinding
    : public std::function<Param(Node*)>
    {
    protected:
      /** by default use Node-hash directly as source of randomness */
      static size_t
      defaultSrc (Node* node)
        {
          return node? node->hash:0;
        }
      
      static size_t
      level (Node* node)
        {
          return node? node->level:0;
        }
      
      static double
      guessHeight (size_t level)
        {     // heuristic guess for a »fully stable state«
          double expectedHeight = 2*maxFan;
          return level / expectedHeight;
        }
      
      
      
      /** Adaptor to handle further mapping functions */
      template<class SIG>
      struct Adaptor
        {
          static_assert (not sizeof(SIG), "Unable to adapt given functor.");
        };
      
      /** allow simple rules directly manipulating the hash value */
      template<typename RES>
      struct Adaptor<RES(size_t)>
        {
          template<typename FUN>
          static auto
          build (FUN&& fun)
            {
              return [functor=std::forward<FUN>(fun)]
                     (Node* node) -> _FunRet<FUN>
                        {
                          return functor (defaultSrc (node));
                        };
            }
        };
      
      /** allow rules additionally involving the height of the graph,
       *  which also represents time. 1.0 refers to _stable state generation,_
       *  guessed as height Level ≡ 2·maxFan . */
      template<typename RES>
      struct Adaptor<RES(size_t,double)>
        {
          
          template<typename FUN>
          static auto
          build (FUN&& fun)
            {
              return [functor=std::forward<FUN>(fun)]
                     (Node* node) -> _FunRet<FUN>
                        {
                          return functor (defaultSrc (node)
                                         ,guessHeight(level(node)));
                        };
            }
        };
      
      /** rules may also build solely on the (guessed) height. */
      template<typename RES>
      struct Adaptor<RES(double)>
        {
          
          template<typename FUN>
          static auto
          build (FUN&& fun)
            {
              return [functor=std::forward<FUN>(fun)]
                     (Node* node) -> _FunRet<FUN>
                        {
                          return functor (guessHeight(level(node)));
                        };
            }
        };
    };
  
  
  
  
  /* ========= Graph Statistics Evaluation ========= */
  
  struct StatKey
    : std::pair<size_t,string>
    {
      using std::pair<size_t,string>::pair;
      operator size_t const&() const { return this->first; }
      operator string const&() const { return this->second;}
    };
  const StatKey STAT_NODE{0,"node"};    ///< all nodes
  const StatKey STAT_SEED{1,"seed"};    ///< seed node
  const StatKey STAT_EXIT{2,"exit"};    ///< exit node
  const StatKey STAT_INNR{3,"innr"};    ///< inner node
  const StatKey STAT_FORK{4,"fork"};    ///< forking node
  const StatKey STAT_JOIN{5,"join"};    ///< joining node
  const StatKey STAT_LINK{6,"link"};    ///< 1:1 linking node
  const StatKey STAT_KNOT{7,"knot"};    ///< knot (joins and forks)
  
  const std::array KEYS = {STAT_NODE,STAT_SEED,STAT_EXIT,STAT_INNR,STAT_FORK,STAT_JOIN,STAT_LINK,STAT_KNOT};
  const uint CAT = KEYS.size();
  const uint IDX_SEED = 1; // index of STAT_SEED
  
  namespace {
    template<class NOD>
    inline auto
    prepareEvaluations()
    {
      return std::array<std::function<uint(NOD&)>, CAT>
        { [](NOD&  ){ return 1;         }
        , [](NOD& n){ return isStart(n);}
        , [](NOD& n){ return isExit(n); }
        , [](NOD& n){ return isInner(n);}
        , [](NOD& n){ return isFork(n); }
        , [](NOD& n){ return isJoin(n); }
        , [](NOD& n){ return isLink(n); }
        , [](NOD& n){ return isKnot(n); }
        };
    }
  }
  
  using VecU      = std::vector<uint>;
  using LevelSums = std::array<uint, CAT>;
  
  /**
   * Distribution indicators for one kind of evaluation.
   * Evaluations over the kind of node are collected per (time)level.
   * This data is then counted, averaged and weighted.
   */
  struct Indicator
    {
      VecU data{};
      uint cnt{0};                   ///< global sum over all levels
      double frac{0};                ///< fraction of all nodes
      double pS {0};                 ///< average per segment
      double pL {0};                 ///< average per level
      double pLW{0};                 ///< average per level and level-width
      double cL {0};                 ///< weight centre level for this indicator
      double cLW{0};                 ///< weight centre level width-reduced
      double sL {0};                 ///< weight centre on subgraph
      double sLW{0};                 ///< weight centre on subgraph width-reduced
      
      void
      addPoint (uint levelID, uint sublevelID, uint width, uint items)
        {
          REQUIRE (levelID == data.size()); // ID is zero based
          REQUIRE (width > 0);
          data.push_back (items);
          cnt += items;
          pS  += items;
          pL  += items;
          pLW += items / double(width);
          cL  += levelID * items;
          cLW += levelID * items/double(width);
          sL  += sublevelID * items;
          sLW += sublevelID * items/double(width);
        }
      
      void
      closeAverages (uint nodes, uint levels, uint segments, double avgheight)
        {
          REQUIRE (levels == data.size());
          REQUIRE (levels > 0);
          frac = cnt / double(nodes);
          cL   = pL?   cL/pL  :0;    // weighted averages: normalise to weight sum
          cLW  = pLW? cLW/pLW :0;
          sL   = pL?   sL/pL  :0;
          sLW  = pLW? sLW/pLW :0;
          pS  /= segments;           // simple averages : normalise to number of levels
          pL  /= levels;             // simple averages : normalise to number of levels
          pLW /= levels;
          cL  /= levels-1;           // weight centres : as fraction of maximum level-ID
          cLW /= levels-1;
          ASSERT (avgheight >= 1.0);
          if (avgheight > 1.0)
            {                        // likewise for weight centres relative to subgraph
              sL  /= avgheight-1;    // height is 1-based, while the contribution was 0-based
              sLW /= avgheight-1;
            }
          else
              sL = sLW = 0.5;
        }
    };
  
  /**
   * Statistic data calculated for a given chain-load topology
   */
  struct Statistic
    {
      uint nodes{0};
      uint levels{0};
      uint segments{1};
      uint maxheight{0};
      double avgheight{0};
      VecU width{};
      VecU sublevel{};
      
      std::array<Indicator, CAT> indicators;
      
      explicit
      Statistic (uint lvls)
        : nodes{0}
        , levels{0}
        {
          reserve (lvls);
        }
      
      void
      addPoint (uint levelWidth, uint sublevelID, LevelSums& particulars)
        {
          levels += 1;
          nodes += levelWidth;
          width.push_back (levelWidth);
          sublevel.push_back (sublevelID);
          ASSERT (levels == width.size());
          ASSERT (0 < levels);
          ASSERT (0 < levelWidth);
          for (uint i=0; i< CAT; ++i)
            indicators[i].addPoint (levels-1, sublevelID, levelWidth, particulars[i]);
        }
      
      void
      closeAverages (uint segs, uint maxSublevelID)
        {
          segments = segs;
          maxheight = maxSublevelID + 1;
          avgheight = levels / double(segments);
          for (uint i=0; i< CAT; ++i)
            indicators[i].closeAverages (nodes,levels,segments,avgheight);
        }
      
    private:
      void
      reserve (uint lvls)
        {
          width.reserve (lvls);
          sublevel.reserve(lvls);
          for (uint i=0; i< CAT; ++i)
            {
              indicators[i] = Indicator{};
              indicators[i].data.reserve(lvls);
            }
        }
    };
  
  
  
  /**
   * Operator on TestChainLoad to evaluate current graph connectivity.
   * In a pass over the internal storage, all nodes are classified
   * and accounted into a set of categories, thereby evaluating
   * - the overall number of nodes and levels generated
   * - the number of nodes in each level (termed _level width_)
   * - the fraction of overall nodes falling into each category
   * - the average number of category members over the levels
   * - the density of members, normalised over level width
   * - the weight centre of this category members
   * - the weight centre of according to density
   */
  template<size_t maxFan>
  inline Statistic
  TestChainLoad<maxFan>::computeGraphStatistics()
    {
      auto totalLevels = uint(topLevel());
      auto classify = prepareEvaluations<Node>();
      Statistic stat(totalLevels);
      LevelSums particulars{0};
      size_t level{0},
          sublevel{0},
       maxsublevel{0};
      size_t segs{0};
      uint width{0};
      auto detectSubgraphs = [&]{ // to be called when a level is complete
                                  if (width==1 and particulars[IDX_SEED]==1)
                                    { // previous level actually started new subgraph
                                      sublevel = 0;
                                      ++segs;
                                    }
                                  else
                                    maxsublevel = max (sublevel,maxsublevel);
                                };
      
      for (Node& node : allNodes())
        {
          if (level != node.level)
            {// Level completed...
              detectSubgraphs();
              // record statistics for previous level
              stat.addPoint (width, sublevel, particulars);
              //   switch to next time-level
              ++level;
              ++sublevel;
              ENSURE (level == node.level);
              particulars = LevelSums{0};
              width = 0;
            }
           // classify and account..
          ++width;
          for (uint i=0; i<CAT; ++i)
            particulars[i] += classify[i](node);
        }
      ENSURE (level = topLevel());
      detectSubgraphs();
      stat.addPoint (width, sublevel, particulars);
      stat.closeAverages (segs, maxsublevel);
      return stat;
    }
  
  
  
  /**
   * Print a tabular summary of graph characteristics
   * @remark explanation of indicators
   *  - »node« : accounting for all nodes
   *  - »seed« : seed nodes start a new subgraph or side chain
   *  - »exit« : exit nodes produce output and have no successor
   *  - »innr« : inner nodes have both predecessors and successors
   *  - »fork« : a node linked to more than one successor
   *  - »join« : a node consuming data from more than one predecessor
   *  - »link« : a node in a linear processing chain; one input, one output
   *  - »LEVL« : the overall number of distinct _time levels_ in the graph
   *  - »SEGS« : the number of completely disjoint partial subgraphs
   *  - »knot« : a node which both joins data and forks out to multiple successors
   *  - `frac` : the percentage of overall nodes falling into this category
   *  - `∅pS`  : averaged per Segment (warning: see below)
   *  - `∅pL`  : averaged per Level
   *  - `∅pLW` : count normalised to the width at that level and then averaged per Level
   *  - `γL◆`  : weight centre of this kind of node, relative to the overall graph
   *  - `γLW◆` : the same, but using the level-width-normalised value
   *  - `γL⬙`  : weight centre, but relative to the current subgraph or segment
   *  - `γLW⬙` : same but using level-width-normalised value
   *  Together, these values indicates how the simulated processing load
   *  is structured over time, assuming that the _»Levels« are processed consecutively_
   *  in temporal order. The graph can unfold or contract over time, and thus nodes can
   *  be clustered irregularly, which can be seen from the weight centres; for that
   *  reason, the width-normalised variants of the indicators are also accounted for,
   *  since a wider graph also implies that there are more nodes of each kind per level,
   *  even while the actual density of this kind did not increase.
   *  @warning no comprehensive connectivity analysis is performed, and thus there is
   *           *no reliable indication of subgraphs*. The `SEGS` statistics may be misleading,
   *           since these count only completely severed and restarted graphs.
   */
  template<size_t maxFan>
  inline TestChainLoad<maxFan>&&
  TestChainLoad<maxFan>::printTopologyStatistics()
    {
      cout << "INDI: cnt frac   ∅pS  ∅pL  ∅pLW  γL◆ γLW◆  γL⬙ γLW⬙\n";
      _Fmt line{"%4s: %3d %3.0f%% %5.1f %5.2f %4.2f %4.2f %4.2f %4.2f %4.2f\n"};
      Statistic stat = computeGraphStatistics();
      for (uint i=0; i< CAT; ++i)
        {
          Indicator& indi = stat.indicators[i];
          cout << line % KEYS[i]
                       % indi.cnt
                       % (indi.frac*100)
                       % indi.pS
                       % indi.pL
                       % indi.pLW
                       % indi.cL
                       % indi.cLW
                       % indi.sL
                       % indi.sLW
                       ;
        }
      cout << _Fmt{"LEVL: %3d\n"} % stat.levels;
      cout << _Fmt{"SEGS: %3d   h = ∅%3.1f / max.%2d\n"}
                       % stat.segments
                       % stat.avgheight
                       % stat.maxheight;
      cout << "───═══───═══───═══───═══───═══───═══───═══───═══───═══───═══───"
           << endl;
      return move(*this);
    }
  
  
  
  
  
  /* ========= Render Job generation and Scheduling ========= */
  
  /**
   * Baseclass: JobFunctor to invoke TestChainLoad
   */
  class ChainFunctor
    : public JobClosure
    {
      
      static lib::time::Grid&
      testGrid()        ///< Meyer's Singleton : a fixed 1-f/s quantiser
        {
          static lib::time::FixedFrameQuantiser gridOne{FrameRate::STEP};
          return gridOne;
        }
      
      /* === JobFunctor Interface === */
      
      string diagnostic()  const             =0;
      void invokeJobOperation (JobParameter) =0;
      
      JobKind
      getJobKind()  const
        {
          return TEST_JOB;
        }
      
      InvocationInstanceID
      buildInstanceID (HashVal)  const override
        {
          return InvocationInstanceID();
        }
      
      size_t
      hashOfInstance (InvocationInstanceID invoKey)  const override
        {
          std::hash<size_t> hashr;
          HashVal res = hashr (invoKey.frameNumber);
          return res;
        }
      
    public:
      
      /** package the node-index to invoke.
       * @note per convention for this test, this info will be
       *  packaged into the lower word of the InvocationInstanceID
       */
      static InvocationInstanceID
      encodeNodeID (size_t idx)
        {
          InvocationInstanceID invoKey;
          invoKey.code.w1 = idx;
          return invoKey;
        };
      
      static size_t
      decodeNodeID (InvocationInstanceID invoKey)
        {
          return size_t(invoKey.code.w1);
        };
      
      static Time
      encodeLevel (size_t level)
        {
          return Time{testGrid().timeOf (FrameCnt(level))};
        }
      
      static size_t
      decodeLevel (TimeValue nominalTime)
        {
          return testGrid().gridPoint (nominalTime);
        }
    };
  
  
  
  /**
   * Render JobFunctor to invoke the _calculation_ of a single Node.
   * The existing Node connectivity is used to retrieve the hash values
   * from predecessors — so these are expected to be calculated beforehand.
   * For setup, the start of the ChainLoad's Node array is required.
   * @tparam maxFan controls expected Node memory layout
   */
  template<size_t maxFan>
  class RandomChainCalcFunctor
    : public ChainFunctor
    {
      using Node = typename TestChainLoad<maxFan>::Node;
      
      Node* startNode_;
      
    public:
      RandomChainCalcFunctor(Node& startNode)
        : startNode_{&startNode}
        { }
      
      
      /** render job invocation to trigger one Node recalculation */
      void
      invokeJobOperation (JobParameter param)  override
        {
          size_t nodeIdx = decodeNodeID (param.invoKey);
          size_t level = decodeLevel (TimeValue{param.nominalTime});
          Node& target = startNode_[nodeIdx];
          ASSERT (target.level == level);
          // invoke the »media calculation«
cout<<_Fmt{"\n!◆! %s: calc(i=%d, lev:%d)"} % markThread() % nodeIdx % level <<endl;          
          target.calculate();
        }
      
      string diagnostic()  const override
        {
          return _Fmt{"ChainCalc(w:%d)◀%s"}
                     % maxFan
                     % util::showAddr(startNode_);
        }
    };
  
  
  /**
   * Render JobFunctor to perform chunk wise planning of Node jobs
   * to calculate a complete Chain-Load graph step by step.
   */
  template<size_t maxFan>
  class RandomChainPlanFunctor
    : public ChainFunctor
    {
      using Node = typename TestChainLoad<maxFan>::Node;
      
      function<void(size_t,size_t)>   scheduleCalcJob_;
      function<void(Node*,Node*)>      markDependency_;
      function<void(size_t,size_t,bool)> continuation_;
      
      size_t maxCnt_;
      Node* nodes_;
      
      size_t currIdx_{0}; // Note: this test-JobFunctor is statefull
      
    public:
      template<class CAL, class DEP, class CON>
      RandomChainPlanFunctor(Node& nodeArray, size_t nodeCnt,
                             CAL&& schedule, DEP&& markDepend,
                             CON&& continuation)
        : scheduleCalcJob_{forward<CAL> (schedule)}
        , markDependency_{forward<DEP> (markDepend)}
        , continuation_{forward<CON> (continuation)}
        , maxCnt_{nodeCnt}
        , nodes_{&nodeArray}
        { }
      
      
      /** render job invocation to trigger one batch of scheduling;
       *  the installed callback-λ should actually place a job with
       *  RandomChainCalcFunctor for each node, and also inform the
       *  Scheduler about dependency relations between jobs. */
      void
      invokeJobOperation (JobParameter param)  override
        {
          size_t reachedLevel{0};
          size_t targetNodeIDX = decodeNodeID (param.invoKey);
cout<<_Fmt{"\n!◆!plan...to:%d%19t|curr=%d (max:%d)"} % targetNodeIDX % currIdx_ % maxCnt_<<endl;          
          for ( ; currIdx_<maxCnt_; ++currIdx_)
            {
              Node* n = &nodes_[currIdx_];
cout<<_Fmt{"%16t|n.(%d,lev:%d)"} % currIdx_ % n->level <<endl;
              if (currIdx_ <= targetNodeIDX)
                reachedLevel = n->level;
              else // continue until end of current level
                if (n->level > reachedLevel)
                  break;
              scheduleCalcJob_(currIdx_, n->level);
              for (Node* pred: n->pred)
                markDependency_(pred,n);
            }
          ENSURE (currIdx_ > 0);
          continuation_(currIdx_-1, reachedLevel, currIdx_ < maxCnt_);
        }
      
      
      string diagnostic()  const override
        {
          return "ChainPlan";
        }
    };
  
  
  
  
  /**
   * Setup and wiring for a test run to schedule a computation structure
   * as defined by this TestChainLoad instance. This context is linked to
   * a concrete TestChainLoad and Scheduler instance and holds a memory block
   * with actual schedules, which are dispatched in batches into the Scheduler.
   * It is *crucial* to keep this object *alive during the complete test run*,
   * which is achieved by a blocking wait on the callback triggered after
   * dispatching the last batch of calculation jobs. This process itself
   * is meant for test usage and not thread-safe (while obviously the
   * actual scheduling and processing happens in the worker threads).
   * Yet the instance can be re-used to dispatch further test runs.
   */
  template<size_t maxFan>
  class TestChainLoad<maxFan>::ScheduleCtx
    : util::MoveOnly
    {
      TestChainLoad& chainLoad_;
      Scheduler&     scheduler_;
      
      lib::UninitialisedDynBlock<ScheduleSpec> schedule_;

      FrameRate  levelSpeed_{1, Duration{_uTicks(1ms)}};
      uint       loadFactor_{2};
      size_t      chunkSize_{DEFAULT_CHUNKSIZE};
      TimeVar     startTime_{Time::ANYTIME};
      microseconds  preRoll_{guessPlanningPreroll (chunkSize_)};
      microseconds deadline_{STANDARD_DEADLINE};
      ManifestationID manID_{};
      
      std::promise<void> signalDone_{};
      
      std::unique_ptr<RandomChainCalcFunctor<maxFan>> calcFunctor_;
      std::unique_ptr<RandomChainPlanFunctor<maxFan>> planFunctor_;
      
      
      /* ==== Callbacks from job planning ==== */
      
      /** Callback: place a single job into the scheduler */
      void
      disposeStep (size_t idx, size_t level)
        {
cout <<_Fmt{"... dispose(i=%d,lev:%d) -> @%s"} % idx % level % relT(calcStartTime(level))<<endl;
          schedule_[idx] = scheduler_.defineSchedule(calcJob (idx,level))
                                     .manifestation(manID_)
                                     .startTime (calcStartTime(level))
                                     .lifeWindow (deadline_)
                                     .post();
        }
      
      /** Callback: define a dependency between scheduled jobs */
      void
      setDependency (Node* pred, Node* succ)
        {
          size_t predIdx = chainLoad_.nodeID (pred);
          size_t succIdx = chainLoad_.nodeID (succ);
          schedule_[predIdx].linkToSuccessor (schedule_[succIdx]);
        }
      
      /** continue planning: schedule follow-up planning job */
      void
      continuation (size_t lastNodeIDX, size_t levelDone, bool work_left)
        {
cout <<_Fmt{"+++ %s: Continuation(lastNode=%d, levelDone=%d, work_left:%s)"} % markThread() % lastNodeIDX % levelDone % work_left <<endl;
          if (work_left)
            {
              size_t nextChunkEndNode = calcNextChunkEnd (lastNodeIDX);
cout <<"--> reschedule to ..."<<nextChunkEndNode<<endl;
              scheduler_.continueMetaJob (calcPlanScheduleTime (nextChunkEndNode)
                                         ,planningJob (nextChunkEndNode)
                                         ,manID_);
            }
          else
            scheduler_.continueMetaJob (calcStartTime (levelDone+1)
                                       ,wakeUpJob()
                                       ,manID_);
        }
      
      std::future<void>
      performRun()
        {
          auto finished = attachNewCompletionSignal();
          size_t numNodes = chainLoad_.size();
          size_t firstChunkEndNode = calcNextChunkEnd(0)-1;
cout <<"+++ "<<markThread()<<": seed(num:"<<numNodes<<")"<<endl;
          schedule_.allocate (numNodes);
          startTime_ = anchorStartTime();
          scheduler_.seedCalcStream (planningJob(firstChunkEndNode)
                                    ,manID_
                                    ,calcLoadHint());
          return finished;
        }
      
    public:
      ScheduleCtx (TestChainLoad& mother, Scheduler& scheduler)
        : chainLoad_{mother}
        , scheduler_{scheduler}
        , calcFunctor_{new RandomChainCalcFunctor<maxFan>{chainLoad_.nodes_[0]}}
        , planFunctor_{new RandomChainPlanFunctor<maxFan>{chainLoad_.nodes_[0], chainLoad_.numNodes_
                                                         ,[this](size_t i, size_t l){ disposeStep(i,l);  }
                                                         ,[this](auto* p, auto* s)  { setDependency(p,s);}
                                                         ,[this](size_t n,size_t l, bool w){ continuation(n,l,w); }
                                                         }}
        { }
      
      ScheduleCtx&&
      launch_and_wait()
        {
          awaitBlocking(
            performRun());
          return move(*this);
        }
      
      
      /* ===== Setter / builders for custom configuration ===== */
      
      ScheduleCtx&&
      withLevelDuration (microseconds plannedTime_per_level)
        {
          levelSpeed_ = FrameRate{1, Duration{_uTicks(plannedTime_per_level)}};
        }
      
      ScheduleCtx&&
      withLoadFactor (uint factor_on_levelSpeed)
        {
          loadFactor_ = factor_on_levelSpeed;
        }
      
      ScheduleCtx&&
      withChunkSize (size_t nodes_per_chunk)
        {
          chunkSize_ = nodes_per_chunk;
          preRoll_ = guessPlanningPreroll (chunkSize_);
        }
      
      ScheduleCtx&&
      withPreRoll (microseconds planning_headstart)
        {
          preRoll_ = planning_headstart;
        }
      
      ScheduleCtx&&
      withJobDeadline (microseconds deadline_after_start)
        {
          deadline_ = deadline_after_start;
        }
      
      ScheduleCtx&&
      withManifestation (ManifestationID manID)
        {
          manID_ = manID;
        }
      
    private:
      /** push away any existing wait state and attach new clean state */
      std::future<void>
      attachNewCompletionSignal()
        {
          std::promise<void> notYetTriggered;
          signalDone_.swap (notYetTriggered);
          return signalDone_.get_future();
        }
      
      void
      awaitBlocking(std::future<void> signal)
        {
          if (std::future_status::timeout == signal.wait_for (SAFETY_TIMEOUT))
            throw err::Fatal("Timeout on Scheduler test exceeded.");
        }
      
      Job
      calcJob (size_t idx, size_t level)
        {
          return Job{*calcFunctor_
                    , calcFunctor_->encodeNodeID(idx)
                    , calcFunctor_->encodeLevel(level)
                    };
        }
      
      Job
      planningJob (size_t endNodeIDX)
        {
          return Job{*planFunctor_
                    , planFunctor_->encodeNodeID(endNodeIDX)
                    , Time::ANYTIME
                    };
        }
      
      Job
      wakeUpJob ()
        {
          SpecialJobFun wakeUpFun{[this](JobParameter)
                                    {
                                      signalDone_.set_value();
                                    }};
          return Job{ wakeUpFun
                    , InvocationInstanceID()
                    , Time::ANYTIME
                    };
        }
      
      Time
      anchorStartTime()
        {
Time ank = RealClock::now() + _uTicks(preRoll_);
cout<<"ANCHOR="+relT(ank)+" preRoll="+util::toString(_raw(_uTicks(preRoll_)))<<endl;
//          return RealClock::now() + _uTicks(preRoll_);
          return ank;
        }
      
      static microseconds
      guessPlanningPreroll(size_t chunkSize)
        {
          return chunkSize * PLANNING_TIME_PER_NODE;
        }
      
      FrameRate
      calcLoadHint()
        {
          return FrameRate{levelSpeed_ * loadFactor_};
        }
      
      size_t
      calcNextChunkEnd (size_t lastNodeIDX)
        {
          return lastNodeIDX + chunkSize_;
        }
      
      Time
      calcStartTime(size_t level)
        {
          return startTime_ + Time{level / levelSpeed_};
        }
      
      Time
      calcPlanScheduleTime (size_t lastNodeIDX)
        {/* must be at least 1 level ahead,
            because dependencies are defined backwards;
            the chain-load graph only defines dependencies over one level
            thus the first level in the next chunk must still be able to attach
            dependencies to the last row of the preceding chunk, implying that
            those still need to be ahead of schedule.
          */
          size_t nextChunkLevel = chainLoad_.nodes_[lastNodeIDX].level;
          nextChunkLevel = nextChunkLevel>2? nextChunkLevel-2 : 0;
          return calcStartTime(nextChunkLevel) - _uTicks(preRoll_);
        }
    };
  
  
  /**
   * establish and configure the context used for scheduling computations.
   * @note clears hashes and re-propagates seed in the node graph beforehand.
   */
  template<size_t maxFan>
  typename TestChainLoad<maxFan>::ScheduleCtx
  TestChainLoad<maxFan>::setupSchedule(Scheduler& scheduler)
  {
    clearNodeHashes();
    return ScheduleCtx{*this, scheduler};
  }
  
  
  
}}} // namespace vault::gear::test
#endif /*VAULT_GEAR_TEST_TEST_CHAIN_LOAD_H*/
