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
 ** all chains of computation, leaving no dead end.
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
//  using std::forward;
//  using std::string;
  using std::swap;
  using std::move;
  using boost::hash_combine;
  
  namespace dot = lib::dot_gen;
  
  namespace { // Default definitions for topology generation
    const size_t DEFAULT_FAN = 16;
    const size_t DEFAULT_SIZ = 256;
    
    const double CAP_EPSILON = 0.001;    ///< tiny bias to absorb rounding problems
  }
  
  
  /**
   * Helper to cap and map to a value range.
   */
  struct Cap
    {
      double lower{0};
      double value{0};
      double upper{1};
      
      Cap(int    i) : value(i){ }
      Cap(size_t s) : value(s){ }
      Cap(double d) : value{d}{ }
      
      size_t
      mapped (size_t scale)
        {
          if (value==lower)
            return 0;
          value -= lower;
          value /= upper-lower;
          value *= scale;
          value += CAP_EPSILON;
          value = limited (size_t(0), value, scale);
          return size_t(value);
        }
    };
  
  
  /**
   * A Generator for synthetic Render Jobs for Scheduler load testing.
   * @tparam maxFan maximal fan-in/out from a node, also limits maximal parallel strands.
   * @see TestChainLoad_test
   */
  template<size_t numNodes =DEFAULT_SIZ, size_t maxFan =DEFAULT_FAN>
  class TestChainLoad
    : util::MoveOnly
    {
          
    public:
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
      
    private:
      using NodeTab = typename Node::Tab;
      using NodeStorage = std::array<Node, numNodes>;
      using CtrlRule = std::function<Cap(size_t, double)>;
      
      std::unique_ptr<NodeStorage> nodes_;
      
      CtrlRule seedingRule_  {[](size_t, double){ return 0; }};
      CtrlRule expansionRule_{[](size_t, double){ return 0; }};
      CtrlRule reductionRule_{[](size_t, double){ return 0; }};
      
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
      
      TestChainLoad&&
      seedingRule (CtrlRule r)
        {
          seedingRule_ = r;
          return move(*this);
        }
      
      TestChainLoad&&
      expansionRule (CtrlRule r)
        {
          expansionRule_ = r;
          return move(*this);
        }
      
      TestChainLoad&&
      reductionRule (CtrlRule r)
        {
          reductionRule_ = r;
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
          size_t expectedLevel = max (1u, numNodes/maxFan);
          
          // prepare building blocks for the topology generation...
          auto height = [&](double level)
                              {
                                return level/expectedLevel;
                              };
          auto spaceLeft = [&]{ return next->size() < maxFan; };
          auto addNode   = [&]{
                                Node* n = *next->add (node++);
                                n->clear();
                                n->level = level;
                                return n;
                              };
          auto apply  = [&](CtrlRule& rule, Node* n)
                              {
                                Cap param = rule (n->hash, height(level));
                                return param.mapped (maxFan);
                              };
          
          addNode(); // prime next with root node
          // visit all further nodes and establish links
          while (node < &nodes_->back())
            {
              ++level;
              curr->clear();
              swap (next, curr);
              size_t toReduce{0};
              Node* r;
              REQUIRE (spaceLeft());
              for (Node* o : *curr)
                { // follow-up on all Nodes in current level...
                  o->calculate();
                  size_t toSeed   = apply (seedingRule_, o);
                  size_t toExpand = apply (expansionRule_,o);
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
                  if (not toReduce and spaceLeft())
                    { // carry-on the chain from o
                      r = addNode();
                      toReduce = apply (reductionRule_, o);
                    }
                  else
                    --toReduce;
                  ENSURE (r);
                  r->addPred(o);
                }
            }
          ENSURE (node == &nodes_->back());
          // connect ends of all remaining chains to top-Node
          node->clear();
          node->level = ++level;
          for (Node* o : *next)
            node->addPred(o);
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
  
  
  
}}} // namespace vault::gear::test
#endif /*VAULT_GEAR_TEST_TEST_CHAIN_LOAD_H*/
