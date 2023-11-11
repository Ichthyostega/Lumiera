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
 ** # Usage
 ** 
 ** Duh oh
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
//#include <functional>
//#include <utility>
//#include <string>
//#include <deque>
#include <vector>
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
//  using std::forward;
//  using std::move;
  using boost::hash_combine;
  
  
  namespace {// Diagnostic markers
//    const string MARK_INC{"IncSeq"};
//    const string MARK_SEQ{"Seq"};
    const size_t DEFAULT_FAN = 16;
    const size_t DEFAULT_SIZ = 256;
    
//    using SIG_JobDiagnostic = void(Time, int32_t);
  }
  
  
  
  /**
   * A Generator for synthetic Render Jobs for Scheduler load testing.
   * @tparam maxFan maximal fan-in/out from a node, also limits maximal parallel strands.
   * @see TestChainLoad_test
   */
  template<size_t numNodes =DEFAULT_SIZ, size_t maxFan =DEFAULT_FAN>
  class TestChainLoad
    : util::NonCopyable
    {
          
    public:
      struct Node
        : util::MoveOnly
        {
          using Tab = std::array<Node*, maxFan>;
          
          size_t hash;
          Tab pred;
          Tab succ;
          
          Node(size_t seed =0)
            : hash{seed}
            , pred{0}
            , succ{0}
            { }
          
          Node&
          addPred (Node& other)
            {
              for (Node*& entry : pred)
                if (not entry)
                  {
                    entry = &other;
                    for (Node*& backlink : other.succ)
                      if (not backlink)
                        {
                          backlink = this;
                          return *this;
                        }
                  }
              NOTREACHED ("excess node linkage");
            }
          
          Node&
          addSucc (Node& other)
            {
              for (Node*& entry : succ)
                if (not entry)
                  {
                    entry = &other;
                    for (Node*& backlink : other.pred)
                      if (not backlink)
                        {
                          backlink = this;
                          return *this;
                        }
                  }
              NOTREACHED ("excess node linkage");
            }
          
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
      using NodeStorage = std::array<Node, numNodes>;
      
      std::unique_ptr<NodeStorage> nodes_;
      
    public:
      TestChainLoad()
        : nodes_{new NodeStorage}
        { }
          
    private:
    };
  
  
  
}}} // namespace vault::gear::test
#endif /*VAULT_GEAR_TEST_TEST_CHAIN_LOAD_H*/
