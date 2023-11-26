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
 ** data loading Jobs without predecessor). The computation always  begins with the _root
 ** node_, proceeds over the node links and finally leads to the _top node,_ which connects
 ** all chains of computation, leaving no dead end. The probabilistic rules controlling the
 ** topology can be configured using the lib::RandomDraw component, allowing either just
 ** to set a fixed probability or to define elaborate dynamic configurations based on the
 ** graph height or node connectivity properties.
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
#include "lib/test/test-helper.hpp"

//#include "vault/gear/job.h"
//#include "vault/gear/activity.hpp"
//#include "vault/gear/nop-job-functor.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/meta/variadic-helper.hpp"
//#include "lib/meta/function.hpp"
//#include "lib/wrapper.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/format-cout.hpp"
#include "lib/random-draw.hpp"
#include "lib/dot-gen.hpp"
#include "lib/util.hpp"

#include <boost/functional/hash.hpp>
#include <functional>
#include <utility>
//#include <string>
//#include <deque>
#include <memory>
#include <string>
#include <array>


namespace vault{
namespace gear {
namespace test {
  
//  using std::string;
//  using std::function;
//  using lib::time::TimeValue;
//  using lib::time::Time;
//  using lib::time::FSecs;
//  using lib::time::Offset;
//  using lib::meta::RebindVariadic;
  using util::min;
  using util::max;
  using util::limited;
  using util::unConst;
  using util::toString;
  using util::showHashLSB;
  using lib::meta::_FunRet;

//  using std::forward;
//  using std::string;
  using std::swap;
  using std::move;
  using boost::hash_combine;
  
  namespace dot = lib::dot_gen;
  
  namespace { // Default definitions for topology generation
    const size_t DEFAULT_FAN = 16;
    const size_t DEFAULT_SIZ = 256;
  }
  
  
  
  
  /***********************************************************************//**
   * A Generator for synthetic Render Jobs for Scheduler load testing.
   * Allocates a fixed set of #numNodes and generates connecting topology.
   * @tparam maxFan maximal fan-in/out from a node, also limits maximal parallel strands.
   * @see TestChainLoad_test
   */
  template<size_t numNodes =DEFAULT_SIZ, size_t maxFan =DEFAULT_FAN>
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
              for (Node*& entry: pred)
                if (entry)
                  hash_combine (hash, entry->hash);
              return hash;
            }
        };
        
        
        /** link Node.hash to random parameter generation */
        class NodeControlBinding;
        
        /** Parameter values limited [0 .. maxFan] */
        using Param = lib::Limited<size_t, maxFan>;
        
        /** Topology is governed by rules for random params */
        using Rule = lib::RandomDraw<NodeControlBinding>;
      
    private:
      using NodeTab = typename Node::Tab;
      using NodeStorage = std::array<Node, numNodes>;
      
      std::unique_ptr<NodeStorage> nodes_;
      
      Rule seedingRule_  {};
      Rule expansionRule_{};
      Rule reductionRule_{};
      
    public:
      TestChainLoad()
        : nodes_{new NodeStorage}
        { }
      
      
      size_t size()     const { return nodes_->size(); }
      size_t topLevel() const { return nodes_->back().level; }
      size_t getSeed()  const { return nodes_->front().hash; }
      size_t getHash()  const { return nodes_->back().hash;  }
      
      
      using NodeIter = decltype(lib::explore (std::declval<NodeStorage & >()));
      
      NodeIter
      allNodes()
        {
          return lib::explore (*nodes_);
        }
      
      
      /* ===== topology control ===== */
      
      static Rule rule() { return Rule(); }
      
      TestChainLoad&&
      seedingRule (Rule&& r)
        {
          seedingRule_ = move(r);
          return move(*this);
        }
      
      TestChainLoad&&
      expansionRule (Rule&& r)
        {
          expansionRule_ = move(r);
          return move(*this);
        }
      
      TestChainLoad&&
      reductionRule (Rule&& r)
        {
          reductionRule_ = move(r);
          return move(*this);
        }
      
      
      /**
       * Use current configuration and seed to (re)build Node connectivity.
       */
      TestChainLoad&&
      buildToplolgy()
        {
          NodeTab a,b,          // working data for generation
                 *curr{&a},     // the current set of nodes to carry on
                 *next{&b};     // the next set of nodes connected to current
          Node* node = &nodes_->front();
          size_t level{0};
          
          // local copy of all rules (they are non-copyable, once engaged)
          Rule expansionRule = expansionRule_;
          Rule reductionRule = reductionRule_;
          Rule seedingRule   = seedingRule_;
          
          // prepare building blocks for the topology generation...
          auto moreNext  = [&]{ return next->size() < maxFan;      };
          auto moreNodes = [&]{ return node < &nodes_->back();     };
          auto spaceLeft = [&]{ return moreNext() and moreNodes(); };
          auto addNode   = [&]{
                                Node* n = *next->add (node++);
                                n->clear();
                                n->level = level;
                                return n;
                              };
          auto apply  = [&](Rule& rule, Node* n)
                              {
                                return rule(n);
                              };
          
          addNode(); // prime next with root node
          // visit all further nodes and establish links
          while (moreNodes())
            {
              ++level;
              curr->clear();
              swap (next, curr);
              size_t toReduce{0};
              Node* r = nullptr;
              REQUIRE (spaceLeft());
              for (Node* o : *curr)
                { // follow-up on all Nodes in current level...
                  o->calculate();
                  size_t toSeed   = apply (seedingRule, o);
                  size_t toExpand = apply (expansionRule,o);
                  while (0 < toSeed and spaceLeft())
                    { // start a new chain from seed
                      Node* n = addNode();
                      n->hash = this->getSeed();
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
                      toReduce = apply (reductionRule, o);
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
              ENSURE (not next->empty());
            }
          ENSURE (node == &nodes_->back());
          // connect ends of all remaining chains to top-Node
          node->clear();
          node->level = ++level;
          for (Node* o : *next)
            {
              o->calculate();
              node->addPred(o);
            }
          node->calculate();
          //
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
          
          auto nodeID = [&](Node& n){ return size_t(&n - &nodes_->front()); };
          
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
      
    private:
    };
  
  
  
  /**
   * Policy/Binding for generation of [random parameters](\ref TestChainLoad::Param)
   * by [»drawing«](\ref random-draw.hpp) based on the [node-hash](\ref TestChainLoad::Node).
   * Notably this policy template maps the ways to spell out [»Ctrl rules«](\ref TestChainLoad::Rule)
   * to configure the probability profile of the topology parameters _seeding, expansion, reduction
   * and pruning._ The RandomDraw component used to implement those rules provides a builder-DSL
   * and accepts λ-bindings in various forms to influence mapping of Node hash into result parameters.
   */
  template<size_t numNodes, size_t maxFan>
  class TestChainLoad<numNodes,maxFan>::NodeControlBinding
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
        {     // heuristic guess, typically too low
          double expectedHeight = max (1u, numNodes/maxFan);
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
       *  which also represents time. 1.0 refers to (guessed) _full height. */
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
  
  
  
}}} // namespace vault::gear::test
#endif /*VAULT_GEAR_TEST_TEST_CHAIN_LOAD_H*/
