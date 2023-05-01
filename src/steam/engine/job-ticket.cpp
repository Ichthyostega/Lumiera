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
#include "vault/engine/nop-job-functor.hpp"
#include "lib/depend.hpp"
#include "lib/util.hpp"


namespace steam {
namespace engine {
  
  namespace { // Details...
    lib::Depend<vault::engine::NopJobFunctor> nopFunctor;
  } // (END) Details...
  
  
//  using mobject::Placement;
//  using mobject::session::Effect;
  using vault::engine::JobParameter;
  using vault::engine::JobClosure;
  
  using lib::HashVal;
  
  using lib::unConst;
  
  
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

  
  /**
   * Generate a render job to invoke the render network configuration
   * as defined and backing this JobTicket. The generated job descriptor record
   * will include a specific InvocationInstanceID, which factors in the nominal
   * frame time, but also the specific backing render configuration.
   * The actual implementation of the job functionality will be delegated
   * to the JobClosure, which was set up for this Segment and JobTiket
   * after the last Builder run created this part of the render network.
   */
  Job
  JobTicket::createJobFor (FrameCoord coordinates)  const
  {
    Time nominalTime = coordinates.absoluteNominalTime;
    if (isnil (provision_))
      {                    //////////////////////////////////////////////////////////////////////////////////TICKET #1296 : can we get away just with a do-nothing functor?
        return Job(nopFunctor(), InvocationInstanceID(), nominalTime);
      }
    else
      {
        REQUIRE (this->isValid(), "Attempt to generate render job for incomplete or unspecified render plan.");
        REQUIRE (coordinates.channelNr < provision_.size(), "Inconsistent Job planning; channel beyond provision");
        Provision const& provision = provision_[coordinates.channelNr];
        JobClosure& functor = static_cast<JobClosure&> (unConst(provision.jobFunctor));      ////////////////TICKET #1287 : fix actual interface down to JobFunctor (after removing C structs)
        InvocationInstanceID invoKey{timeHash (nominalTime, provision.invocationSeed)};
        
        return Job(functor, invoKey, nominalTime);
      }
  }
  
  /**
   * Tag the precomputed invocation ID with the nominal frame time
   */
  InvocationInstanceID
  JobTicket::timeHash (Time nominalTime, InvocationInstanceID const& seed)
  {
    InvocationInstanceID res{seed};
    HashVal timeMark = res.part.t;
    lib::hash::combine (timeMark, hash_value (nominalTime));
    res.part.t = timeMark;
    return res;
  }
  
  
  namespace {     ///////////////////////////////////////////////////////////////////////////////////////////TICKET #1287 : temporary workaround until we get rid of the C base structs
    inline bool
    operator== (InvocationInstanceID const& l, InvocationInstanceID const& r)
    {
      return lumiera_invokey_eq (unConst(&l), unConst(&r));
    }
  }
  
  /**
   * Helper for tests: verify the given invocation parameters match this JobTicket.
   */
  bool
  JobTicket::verifyInstance (JobFunctor& functor, InvocationInstanceID const& invoKey, Time nominalTime)  const
  {
    for (Provision const& p : provision_)
      if (util::isSameObject (p.jobFunctor, functor)
          and invoKey == timeHash (nominalTime, p.invocationSeed)
         )
        return true;
    return false;
  }

  
  
  
}} // namespace steam::engine
