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
 ** - jaleck
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
//#include "lib/format-util.hpp"
//#include "lib/util.hpp"

#include <boost/functional/hash.hpp>
#include <functional>
#include <utility>
//#include <string>
//#include <deque>
//#include <vector>
#include <sstream>
#include <string>
#include <memory>
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
//  using util::isnil;
  using util::max;
  using util::unConst;
//  using std::forward;
  using std::string;
  using std::swap;
  using std::move;
  using boost::hash_combine;
  
  
  namespace {// Diagnostic markers
//    const string MARK_INC{"IncSeq"};
//    const string MARK_SEQ{"Seq"};
    const size_t DEFAULT_FAN = 16;
    const size_t DEFAULT_SIZ = 256;
    
//    using SIG_JobDiagnostic = void(Time, int32_t);
  }
  
  
  namespace dot {
    
    /**
     * Helper to generate DOT-Graphviz rendering of topology
     */
    class DotOut
      {
        std::ostringstream buff_;
        
      public:
        
        operator string()  const { return buff_.str(); }
      };
    
    struct Code
      {
        Code (string code ="")
          {
            
          }
        
        Code&&
        operator+= (Code&& c)
          {
            
          }
        
        Code&&
        operator+ (Code&& c)
          {
            
          }
      };
    
    struct Section : Code
      {
        Section(string name)
          {
            
          }
      };
    
    struct Node : Code
      {
        Node (size_t id)
          {
            
          }
        Node&&
        label(size_t i)
          {
            return move(*this);
          }
        Node&&
        style(Code code)
          {
            return move(*this);
          }
      };
    
    inline Code
    scope (size_t id)
    {
      
    }
    inline Code
    rankMIN()
    {
      
    }
    
    inline Code
    connect (size_t src, size_t dest)
    {
      
    }
    
    template<class...COD>
    inline DotOut
    digraph (COD ...parts)
    {
      
    }
    
    
////////////////////////////////////////////    
////////////////////////////////////////////    
  }
  
  
  
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
          
          /** Table with connections to other Node records */
          struct Tab : _Arr
            {
              Iter after = _Arr::begin();
              Iter end() { return after; }
              friend Iter end(Tab& tab) { return tab.end(); }
              
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
      using CtrlRule = std::function<size_t(size_t, double)>;
      
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
      
      
      /* ===== topology control ===== */
      
      /**
       * Use current configuration and seed to (re)build Node connectivity.
       */
      TestChainLoad
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
                                return rule (n->hash, height(level));
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
      
    private:
    };
  
  
  
}}} // namespace vault::gear::test
#endif /*VAULT_GEAR_TEST_TEST_CHAIN_LOAD_H*/
