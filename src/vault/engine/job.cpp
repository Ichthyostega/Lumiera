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
 ** Within this translation unit, the actual invocation of a frame rendering job
 ** takes place, after reconstruction of the job's execution environment (closure).
 ** 
 ** @see JobTicket
 ** @see ProcNode
 ** @see nodeinvocation.hpp
 **
 */


#include "vault/engine/job.h"
#include "lib/util.hpp"

#include <boost/functional/hash.hpp>
#include <typeinfo>


namespace backend {
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
  
  
  using lib::HashVal;
  using util::isSameObject;
  
  
  
  /**
   * emit the VTable for JobClosure within this compilation unit,
   * which is still part of the Vault. The actual job implementation
   * classes are defined in the Steam-Layer
   */
  JobClosure::~JobClosure() { }
  
  
  
  
  
  void
  Job::triggerJob()  const
  {
    myClosure(this).invokeJobOperation (parameter);
  }
  
  
  void
  Job::signalFailure (JobFailureReason reason)  const
  {
    myClosure(this).signalFailure (parameter, reason);
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
        && myClosure(this).verify (getNominalTime(),
                                   getInvocationInstanceID());
  }
  
  
  bool
  Job::usesClosure (JobClosure const& otherClosure)  const
    {
      return isSameObject (myClosure(this), otherClosure);
    }

  
  
  /** hash value based on all relevant job data.
   *  Job records hashing to the same value shall be considered equivalent.
   *  Since the interpretation of the #InvocationInstanceID is a private detail
   *  of the JobClosure, calculating this hash requires a virtual call into the
   *  concrete JobClosure. This is not considered problematic, as the normal
   *  job operation and scheduling doesn't rely on the job's hash. Only some
   *  diagnostic facilities do. */
  HashVal
  hash_value (Job const& job)
  {
    return myClosure(&job).hash_value (job.parameter);
  }
  
  HashVal
  JobClosure::hash_value (JobParameter parameter)  const
    {
      HashVal hash = this->hashOfInstance (parameter.invoKey);
      boost::hash_combine(hash, typeid(*this).hash_code());
      boost::hash_combine(hash, parameter.nominalTime);
      return hash;
    }
  
  
}} // namespace backend::engine

namespace {
  using backend::engine::Job;
    
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
lumiera_job_failure (LumieraJobDefinition jobDef, JobFailureReason reason)
{
  REQUIRE (jobDef);
  forwardInvocation(*jobDef).signalFailure(reason);
}

size_t
lumiera_job_get_hash (LumieraJobDefinition jobDef)
{
  REQUIRE (jobDef);
  return hash_value (forwardInvocation (*jobDef));
}
}
