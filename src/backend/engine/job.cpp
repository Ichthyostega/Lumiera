/*
  Job  -  render job closure

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


/** @file job.cpp 
 ** Implementation of render job invocation.
 ** Within this translation unit, the actual invocation of a frame rendering
 ** job takes place, after reconstruction of the job's execution environment (closure).
 ** 
 ** @see JobTicket
 ** @see ProcNode
 ** @see nodeinvocation.hpp
 **
 */


#include "proc/engine/job.hpp"
#include "proc/engine/job-ticket.hpp"


namespace proc {
namespace engine {
  
  namespace { // Details...
    
    inline JobClosure&
    myClosure (const Job * const self)
    {
      ASSERT (self);
      ASSERT (self->jobClosure);
      return *static_cast<JobClosure*> (self->jobClosure);
    }
    
  } // (END) Details...
  
  
//  using mobject::Placement;
//  using mobject::session::Effect;
  
  
  
  /**
   * emit the VTable for JobClosure within this compilation unit,
   * which is still part of the backend. The actual job implementation
   * classes are defined in the Proc-Layer
   */
  JobClosure::~JobClosure() { }
  
  
  
  
  
  /** @todo WIP-WIP 2/12  
   */
  void
  Job::triggerJob()  const
  {
    myClosure(this).invokeJobOperation (parameter);
  }
  
  
  void
  Job::signalFailure()  const
  {
    myClosure(this).signalFailure (parameter);
  }
  
  
  /** find out about the classification of this job.
   *  Typically its not necessary for the normal scheduling of
   *  Jobs to know anything beyond the contents of the #lumiera_jobDescriptor,
   *  but the JobClosure is able to answer any additional introspection queries
   */
  JobKind
  Job::getKind()  const
  {
    REQUIRE (isValid());
    return myClosure(this).getJobKind();
  }
    
  
  /** Render Job self verification.
   *  performs a parameter consistency check
   *  including a call-back to the defining JobTicket
   */
  bool
  Job::isValid()  const
  {
    return this->jobClosure
        && myClosure(this).verify (getNominalTime());
  }
  
  
}} // namespace proc::engine

namespace {
  using proc::engine::Job;
    
  inline Job& 
  forwardInvocation (lumiera_jobDefinition& jobDef)
  {
    Job& job = static_cast<Job&> (jobDef);
    
    REQUIRE (job.isValid());
    return job;
  }
}



extern "C" { /* ==== implementation C interface for job invocation ======= */
  
void
lumiera_job_invoke  (LumieraJobDefinition jobDef)
{
  REQUIRE (jobDef);
  forwardInvocation(*jobDef).triggerJob();
}

void
lumiera_job_failure (LumieraJobDefinition jobDef, JobFailureReason)
{
  REQUIRE (jobDef);
  forwardInvocation(*jobDef).signalFailure();   ////TODO forward the JobFailureReason parameter
}
}
