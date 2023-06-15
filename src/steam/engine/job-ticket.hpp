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
#include "vault/engine/job.h"
#include "steam/engine/frame-coord.hpp"
#include "steam/engine/exit-node.hpp"
#include "lib/hierarchy-orientation-indicator.hpp"
#include "lib/linked-elements.hpp"
#include "lib/util-foreach.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/itertools.hpp"
#include "lib/util.hpp"

#include <utility>
#include <stack>


namespace steam {
namespace engine {
  
using vault::engine::Job;
using vault::engine::JobFunctor;
using vault::engine::JobClosure;        /////////////////////////////////////////////////////////////////////TICKET #1287 : fix actual interface down to JobFunctor (after removing C structs)
using lib::LinkedElements;
using lib::OrientationIndicator;
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
   * @todo 4/23 WIP-WIP-WIP defining the invocation sequence and render jobs
   */
  class JobTicket
    : util::NonCopyable
    {
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
      
      
      /** what handling this task entails */
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
      
      
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 :: to be refactored...
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 : likely to become obsolete
      class ExplorationState;
      friend class ExplorationState;
      ExplorationState startExploration()                        const;     ////////////////////////////TICKET #1276 : likely to become obsolete
      ExplorationState discoverPrerequisites (uint channelNr =0) const;     ////////////////////////////TICKET #1276 : likely to become obsolete
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 : likely to become obsolete
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 :: to be refactored...
      
      Job createJobFor (FrameCoord coordinates)  const;
      
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
      
    protected:
      static InvocationInstanceID timeHash (Time, InvocationInstanceID const&);
      bool verifyInstance (JobFunctor&, InvocationInstanceID const&, Time)  const;

    };
  
  
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 :: to be refactored...
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 : likely to become obsolete
  class JobTicket::ExplorationState
    {
      typedef Prerequisites::iterator SubTicketSeq;
      typedef std::stack<SubTicketSeq> SubTicketStack;               //////////////////////////TODO use a custom container to avoid heap allocations
      
      SubTicketStack toExplore_;
      OrientationIndicator orientation_;
      
    public:
      ExplorationState() { }
      
      ExplorationState (Prerequisites& prerequisites)
        {
          if (not isnil (prerequisites))
            toExplore_.push (prerequisites.begin());
        }
      
      // using default copy operations
      
      
      bool
      empty()  const
        {
          return toExplore_.empty();
        }
      
      
      void
      markTreeLocation()
        {
          orientation_.markRefLevel (toExplore_.size());
          orientation_.resetToRef();
          ENSURE (0 == orientation_);
        }
      
      
      void
      pullNext()
        {
          if (empty())
            throw lumiera::error::Logic ("Exploration of Job prerequisites floundered. "
                                         "Attempt to iterate beyond the end of prerequisite list"
                                        ,lumiera::error::LUMIERA_ERROR_ITER_EXHAUST);
          ASSERT (toExplore_.top().isValid());
          
          ++(toExplore_.top());
          while ( !toExplore_.empty()
                && toExplore_.top().empty())
            toExplore_.pop();
          
          ENSURE (empty() || toExplore_.top().isValid());
        }
      
      
      void
      push (ExplorationState subExploration)                 // note: passing deliberately by value
        {
          if (subExploration.empty()) return;
          
          pushAllPrerequisites (subExploration.toExplore_);
        }
      
      
      JobTicket const *
      operator->() const
        {
          REQUIRE (!empty() && toExplore_.top().isValid());
          REQUIRE (toExplore_.top()->prereqTicket.isValid());
          
          return & toExplore_.top()->prereqTicket;
        }
      
      
    private:
      void
      pushAllPrerequisites (SubTicketStack& furtherPrerequisites)
        {
          REQUIRE (!isnil (furtherPrerequisites));
          
          if (1 == furtherPrerequisites.size())
            {
              this->toExplore_.push (furtherPrerequisites.top());
            }
          else 
            { // pathological case: several levels of prerequisites
              // --> push recursively to retain level ordering
              SubTicketSeq deepestLevel (furtherPrerequisites.top());
              furtherPrerequisites.pop();
              pushAllPrerequisites (furtherPrerequisites);
              this->toExplore_.push (deepestLevel);
            }
        }
    };
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 : likely to become obsolete
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 :: to be refactored...
  
  
  

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
  
  

#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 :: to be refactored...
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 : likely to become obsolete
  /// @deprecated : could be expendable ... likely incurred solely by the use of Monads as design pattern 
  inline JobTicket::ExplorationState
  JobTicket::startExploration()  const
  {
    UNIMPLEMENTED ("somehow build a self-referential pseudo-prerequisite, and seed an ExplorationState with that");
    /////////////////////TODO problem is: we need a JobTicket::Prerequisite instance, where the descriptor points to "self" (this JobTicket)
    /////////////////////TODO           : but this instance needs to reside somewhere at a safe location, since we want to embed a LinkedElements-iterator
    /////////////////////TODO           : into the ExplorationState. And obviously we do not want that instance in each JobTicket, only in the top level ones
    
    /////////////////////TODO : on second thought -- better have a top-level entry point to the evaluation of a frame
    /////////////////////TODO                        basically this inherits from Prerequisite and lives somehow in the dispatcher-table or segment
  }
  
  
  
  inline JobTicket::ExplorationState
  JobTicket::discoverPrerequisites (uint channelNr)  const
  {
    return empty()? ExplorationState()
                  : ExplorationState (util::unConst(provision_).prerequisites);
  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 : likely to become obsolete
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 :: to be refactored...
  
  
}} // namespace steam::engine
#endif
