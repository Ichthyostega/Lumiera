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
#include "lib/util.hpp"

#include <stack>


namespace steam {
namespace engine {
  
//using lib::time::TimeSpan;
//using lib::time::Duration;
//using lib::time::FSecs;
//using lib::time::Time;
using vault::engine::Job;
using lib::LinkedElements;
using lib::OrientationIndicator;
using util::isnil;
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
   * @todo 1/12 WIP-WIP-WIP defining the invocation sequence and render jobs
   */
  class JobTicket
    : util::NonCopyable
    {
      struct Provision
        {
          Provision* next;
          ////////////////////TODO some channel or format descriptor here
        };
      
      struct Prerequisite
        {
          Prerequisite* next;
          JobTicket* descriptor;
        };
      
      struct Prerequisites ///< per channel
        {
          Prerequisites* next;
          LinkedElements<Prerequisite> requiredJobs_;
        };
      
      
      
      LinkedElements<Provision>   channelConfig_;
      LinkedElements<Prerequisites> requirement_;
      
      
    public:
      class ExplorationState;
      friend class ExplorationState;
      
      
      JobTicket()
        {
          UNIMPLEMENTED ("job representation, planning and scheduling");
        }
      
      
      ExplorationState startExploration()                      const;
      ExplorationState discoverPrerequisites (uint channelNr)  const;
      
      Job createJobFor (FrameCoord coordinates);
      
      
      bool
      isValid()  const
        {
          if (channelConfig_.size() != requirement_.size())
            return false;
          
          UNIMPLEMENTED ("validity self check");
        }
    };
  
  
  class JobTicket::ExplorationState
    {
      typedef LinkedElements<Prerequisite>::iterator SubTicketSeq;
      typedef std::stack<SubTicketSeq> SubTicketStack;               //////////////////////////TODO use a custom container to avoid heap allocations
      
      SubTicketStack toExplore_;
      OrientationIndicator orientation_;
      
    public:
      ExplorationState() { }
      
      ExplorationState (Prerequisites& prerequisites)
        {
          if (!isnil (prerequisites.requiredJobs_))
            toExplore_.push (prerequisites.requiredJobs_.begin());
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
          REQUIRE (toExplore_.top()->descriptor);
          REQUIRE (toExplore_.top()->descriptor->isValid());
          
          return toExplore_.top()->descriptor;
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
    REQUIRE (channelNr < requirement_.size());
    
    return ExplorationState (requirement_[channelNr]);
  }

  
  
  
}} // namespace steam::engine
#endif
