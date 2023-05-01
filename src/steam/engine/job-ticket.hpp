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
//#include "steam/state.hpp"
#include "vault/engine/job.h"
#include "steam/engine/frame-coord.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/time/timequant.hpp"
#include "lib/hierarchy-orientation-indicator.hpp"
#include "lib/linked-elements.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/meta/trait.hpp"
#include "lib/util.hpp"

#include <utility>
#include <stack>


namespace steam {
namespace engine {
  
//using lib::time::TimeSpan;
//using lib::time::Duration;
//using lib::time::FSecs;
//using lib::time::Time;
using vault::engine::Job;
using vault::engine::JobFunctor;
using vault::engine::JobClosure;        /////////////////////////////////////////////////////////////////////TICKET #1287 : fix actual interface down to JobFunctor (after removing C structs)
using lib::LinkedElements;
using lib::OrientationIndicator;
using util::isnil;
using lib::HashVal;
using lib::LUID;
//  
//class ExitNode;
  
  
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
   * regarding the precise frame number (=nominal time) and the channel number
   * to calculate (in case the actual feed is multichannel, which is the default).
   * This way, the JobTicket acts as _higher order function:_ a function
   * generating on invocation another, specific function (= the job).
   * 
   * @todo 4/23 WIP-WIP-WIP defining the invocation sequence and render jobs
   * @todo maybe the per-channel specialisation can be elided altogether...?
   */
  class JobTicket
    : util::NonCopyable
    {
      struct Prerequisite
        {
          Prerequisite* next{nullptr};
          JobTicket& descriptor;
          
          Prerequisite (JobTicket& ticket)
            : descriptor{ticket}
          { }
        };
      using Prerequisites = LinkedElements<Prerequisite>;
      
      /** what handling this task entails */
      struct Provision
        {
          Provision* next{nullptr};
          JobFunctor&   jobFunctor;
          InvocationInstanceID invocationSeed;
          Prerequisites requirements{};
          ////////////////////TODO some channel or format descriptor here
          Provision (JobFunctor& func, HashVal seed =0)
            : jobFunctor{func}
            , invocationSeed(static_cast<JobClosure&>(func).buildInstanceID(seed))      ////////////////TICKET #1287 : fix actual interface down to JobFunctor (after removing C structs)
          { }
        };
      
      
      LinkedElements<Provision> provision_;
      
      
      template<class IT>
      static LinkedElements<Provision> buildProvisionSpec (IT);
      
    private:
      JobTicket() { }   ///< @internal as NIL marker, a JobTicket can be empty
      
    protected:
      template<class IT>
      JobTicket(IT featureSpec_perChannel)
        : provision_{buildProvisionSpec (featureSpec_perChannel)}
        { }

    public:
      class ExplorationState;
      friend class ExplorationState;
      
      static const JobTicket NOP;

      
      
      ExplorationState startExploration()                        const;
      ExplorationState discoverPrerequisites (uint channelNr =0) const;
      
      Job createJobFor (FrameCoord coordinates)  const;
      
      
      bool
      isValid()  const
        {
          if (isnil (provision_))
            return false;
          
          TODO ("validity self check");
          return true;
        }
      
    protected:
      static InvocationInstanceID timeHash (Time, InvocationInstanceID const&);
      bool verifyInstance (JobFunctor&, InvocationInstanceID const&, Time)  const;

    };
  
  
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
      
      
      JobTicket*
      operator->() const
        {
          REQUIRE (!empty() && toExplore_.top().isValid());
          REQUIRE (toExplore_.top()->descriptor.isValid());
          
          return & toExplore_.top()->descriptor;
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
  
  
  

  /** @internal prepare and assemble the working data structure to build a JobTicket.
   * @tparam IT iterator to yield a sequence of specifications for each channel,
   *            given as `std::pair` of a JobFunctor and a further Sequence of
   *            JobTicket prerequisites. 
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
  template<class IT>
  inline LinkedElements<JobTicket::Provision>
  JobTicket::buildProvisionSpec (IT featureSpec)
  {                                                                       /* ---- validate parameter type ---- */
    static_assert (lib::meta::can_IterForEach<IT>::value);                // -- can be iterated
    using Spec = typename IT::value_type;
    static_assert (lib::meta::is_Tuple<Spec>());                          // -- payload of iterator is a tuple
    using Func = typename std::tuple_element<0, Spec>::type;
    using Seed = typename std::tuple_element<1, Spec>::type;
    using Preq = typename std::tuple_element<2, Spec>::type;
    static_assert (lib::meta::is_basically<Func, JobFunctor>());          // -- first component specifies the JobFunctor to use
    static_assert (lib::meta::is_basically<Seed, HashVal>());             // -- second component provides a specific seed for Invocation-IDs
    static_assert (lib::meta::can_IterForEach<Preq>::value);              // -- third component is again an iterable sequence
    static_assert (std::is_same<typename Preq::value_type, JobTicket>()); // -- which yields JobTicket prerequisites
    REQUIRE (not isnil (featureSpec)
            ,"require at least specification for one channel");
    
    LinkedElements<Provision> provisionSpec;    //////////////////////////////////////////////////TICKET #1292 : need to pass in Allocator as argument
    for ( ; featureSpec; ++featureSpec)
      {
        JobFunctor& func = std::get<0> (*featureSpec);
        HashVal invoSeed = std::get<1> (*featureSpec);
        auto& provision  = provisionSpec.emplace<Provision> (func, invoSeed);
        for (Preq pre = std::get<2> (*featureSpec); pre; ++pre)
            provision.requirements.emplace<Prerequisite> (*pre);
      }
    provisionSpec.reverse();        // retain order of given definitions per channel
    ENSURE (not isnil (provisionSpec));
    return provisionSpec;
  }
  
  
  
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
    REQUIRE (channelNr < provision_.size() or not isValid());
    
    return isnil (provision_)? ExplorationState()
                             : ExplorationState (provision_[channelNr].requirements);
  }

  
  
  
}} // namespace steam::engine
#endif
