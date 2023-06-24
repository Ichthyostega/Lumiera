/*
  JOB-TICKET.hpp  -  execution plan for render jobs

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file job-ticket.hpp
 ** Execution plan to generate render jobs within a specific render process.
 ** A JobTicket is a preconfigured generator for render jobs, which in turn
 ** are functors to perform the calculations for a specific data frame.
 ** @see job.hpp
 ** 
 ** @warning as of 4/2023 a complete rework of the Dispatcher is underway ///////////////////////////////////////////TICKET #1275
 ** 
 */


#ifndef STEAM_ENGINE_JOB_TICKET_H
#define STEAM_ENGINE_JOB_TICKET_H

#include "steam/common.hpp"
#include "vault/gear/job.h"
#include "steam/engine/exit-node.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/linked-elements.hpp"
#include "lib/util-foreach.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/itertools.hpp"
#include "lib/util.hpp"

#include <utility>
#include <stack>


namespace steam {
namespace engine {
  
using vault::gear::Job;
using vault::gear::JobFunctor;
using vault::gear::JobClosure;        /////////////////////////////////////////////////////////////////////TICKET #1287 : fix actual interface down to JobFunctor (after removing C structs)
using lib::LinkedElements;
using lib::time::Duration;
using lib::time::Time;
using util::isnil;
using lib::HashVal;
using lib::LUID;
  
  
  /**
   * execution plan for pulling a specific exit node.
   * Usable as blue print for generating actual render jobs.
   * Job tickets are created on demand, specialised for each segment
   * of the low-level model, and for each individual feed (corresponding
   * to a single model port). Once created, they are final for this segment,
   * stored together with the other descriptor objects (ProcNode and WiringDescriptor)
   * and finally discarded in bulk, in case that segment of the low-level model becomes
   * obsolete and is replaced by a newly built new version of this model segment.
   * 
   * Job tickets are created by a classical recursive descent call on the exit node,
   * which figures out everything to be done for generating data from this node.
   * To turn a JobTicket into an actual job, we need the additional information
   * regarding the precise frame number (=nominal time) and a handle for the
   * DataSink exposing buffers to output generated data. Thus effectively
   * the JobTicket acts as _higher order function:_ a function generating
   * on invocation another, specific function (= the job).
   * 
   * @note JobTicket is effectively immutable after construction
   * @todo 6/23 WIP rework and integration for »PlaybackVerticalSlice«
   */
  class JobTicket
    : util::NonCopyable
    {
      /** @internal management of prerequisites */
      struct Prerequisite
        {
          Prerequisite* next{nullptr};  // for intrusive list
          JobTicket&    prereqTicket;
          
          template<class ALO>
          Prerequisite (ExitNode const& node, ALO& allocateTicket)
            : prereqTicket{allocateTicket (node, allocateTicket)}
          { }
        };
      using Prerequisites = LinkedElements<Prerequisite>;
      
      
      /** @internal what handling this task entails */
      struct Provision
        {
          JobFunctor&          jobFunctor;
          ExitNode const&      exitNode;
          InvocationInstanceID invocationSeed;
          Prerequisites        prerequisites{};
          
          Provision (JobFunctor& func, ExitNode const& node, HashVal seed =0)
            : jobFunctor{func}
            , exitNode{node}
            , invocationSeed(static_cast<JobClosure&>(func).buildInstanceID(seed))      ////////////////TICKET #1287 : fix actual interface down to JobFunctor (after removing C structs)
          { }                                                                          /////////////////TICKET #1293 : Hash-Chaining for invocation-ID... size_t hash? or a LUID?
        };
      
      /// @internal reference to all information required for actual Job creation
      Provision provision_;
      
      
      JobTicket();      ///< @internal as NIL marker, a JobTicket can be empty
      
      template<class ALO>
      static Provision buildProvisionSpec (ExitNode const&, ALO&);
      
      
      
    public:
      template<class ALO>
      JobTicket (ExitNode const& exitNode, ALO& allocator)
        : provision_{buildProvisionSpec (exitNode, allocator)}
        { }
      
      static JobTicket NOP;
      
      
      bool
      empty()  const
        {
          return isnil (provision_.exitNode);
        }
      
      bool
      isValid()  const
        {
          if (empty()) return false;
          
            InvocationInstanceID empty; /////////////////////////////////////////////////////////////////////TICKET #1287 : temporary workaround until we get rid of the C base structs
          return not lumiera_invokey_eq (&util::unConst(provision_).invocationSeed, &empty)
             and provision_.exitNode.isValid()
             and util::and_all (provision_.prerequisites
                               ,[](auto& pq){ return pq.prereqTicket.isValid(); });
        }
      
      
      /**
       * Core operation: iterate over the prerequisites,
       * required to carry out a render operation based on this blueprint
       * @return iterator exposing the prerequisites as JobTicket&
       */
      auto
      getPrerequisites ()
        {
          return lib::transformIterator (this->empty()? Prerequisites::iterator()
                                                      : provision_.prerequisites.begin()
                                        ,[](Prerequisite& prq) -> JobTicket&
                                           {
                                             return prq.prereqTicket;
                                           });
        }
      
      /**
       * Core operation: build a concrete render job based on this blueprint
       */
      Job createJobFor (Time nominalTime);
      
      /**
       * Core operation: guess expected runtime for rendering
       */
      Duration getExpectedRuntime();
      
      
    protected:
      static InvocationInstanceID timeHash (Time, InvocationInstanceID const&);
      bool verifyInstance (JobFunctor&, InvocationInstanceID const&, Time)  const;

    };
  
  
  
  

  /** @internal prepare and assemble the working data structure to build a JobTicket.
   * @tparam ALO type of an allocator front-end for generating prerequisite JobTicket(s)
   * @param  exitNode a (possibly recursive) tree of ExitNode, detailing points where to
   *                  pull and process data from the render nodes network; these can refer
   *                  to nested ExitNodes(s), which need to be processed beforehand, as
   *                  prerequisite for invoking the given (dependent) ExitNode.
   * @return the final wired instance of the data structure to back the new JobTicket
   * @remark Note especially that those data structures linked together for use by the
   *         JobTicket are themselves allocated "elsewhere", and need to be attached
   *         to a memory management scheme (typically an AllocationCluster for some
   *         Segment of the Fixture datastructure). This data layout can be tricky
   *         to get right, and is chosen here for performance reasons, assuming
   *         that there is a huge number of segments, and these are updated
   *         frequently after each strike of edit operations, yet traversed
   *         and evaluated on a sub-second scale for ongoing playback.
   */
  template<class ALO>
  inline JobTicket::Provision
  JobTicket::buildProvisionSpec (ExitNode const& exitNode, ALO& allocTicket)
  {
    REQUIRE (not isnil (exitNode));  // has valid functor
    HashVal invoSeed = exitNode.getPipelineIdentity();
    JobFunctor& func = exitNode.getInvocationFunctor();
    Provision provisionSpec{func, exitNode, invoSeed};
    for (ExitNode const& preNode: exitNode.getPrerequisites())
      provisionSpec.prerequisites.emplace(preNode, allocTicket);    //////////////////////////////////////////TICKET #1292 : need to pass in generic Allocator as argument
    return provisionSpec;
  }
  
  
  
}}// namespace steam::engine
#endif /*STEAM_ENGINE_JOB_TICKET_H*/
