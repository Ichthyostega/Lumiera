/*
  JobTicket  -  execution plan for render jobs

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

* *****************************************************/


/** @file job-ticket.cpp
 ** Implementation details of preparing and performing job invocations.
 ** 
 ** @todo WIP-WIP-WIP 3/2012
 ** @see Job
 **
 */


#include "steam/engine/job-ticket.hpp"


namespace steam {
namespace engine {
  
  namespace { // Details...
    
  } // (END) Details...
  
  
//  using mobject::Placement;
//  using mobject::session::Effect;
using vault::engine::JobParameter;
using vault::engine::JobClosure;
  
using lib::HashVal;
  
  
  class FrameJobClosure
    : public JobClosure
    {
      // data members?
      
    private: /* === JobClosure Interface === */
      
      JobKind
      getJobKind()  const
        {
          return CALC_JOB;
        }
      
      bool
      verify (Time nominalTime, InvocationInstanceID invoKey)  const
        {
          UNIMPLEMENTED ("access the underlying JobTicket and verify the given job time is within the relevant timeline segment");
          return false;
        }
      
      InvocationInstanceID
      buildInstanceID (HashVal seed)  const override
        {
          UNIMPLEMENTED ("systematically generate an invoKey, distinct for the nominal time");
        }
      
      size_t
      hashOfInstance (InvocationInstanceID invoKey)  const override
        {
          UNIMPLEMENTED ("interpret the invoKey and create a suitable hash");
        }
      
      void
      invokeJobOperation (JobParameter parameter)  override
        {
          UNIMPLEMENTED ("representation of the job functor");
        }
      
      
      void
      signalFailure (JobParameter parameter, JobFailureReason reason)  override
        {
          UNIMPLEMENTED ("what needs to be done when a job cant be invoked?");
        }
      
    public:
      
      
    };
  
  
  /** special »do nothing« JobTicket marker */
  const JobTicket JobTicket::NOP{};            //////////////////////////////////////////////////////////////TICKET #725 : do we actually need that for the final data structure?

  
  /**  */
  Job
  JobTicket::createJobFor (FrameCoord coordinates)
  {
    REQUIRE (this->isValid(), "Attempt to generate render job for incomplete or unspecified render plan.");
    UNIMPLEMENTED ("job planning and generation");
  }
  
  
  /**
   * Helper for tests: verify the given invocation parameters match this JobTicket.
   */
  bool
  JobTicket::verifyInstance (JobFunctor& functor, Time nominalTime)  const
  {
    for (Provision const& p : provision_)
      if (util::isSameObject (p.jobFunctor, functor))
        {
          TODO ("actually re-compute the invocation ID !!!!!!!!!!!!");
          return true;
        }
    return false;
  }

  
  
  
}} // namespace steam::engine
