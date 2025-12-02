/*
  JobHash(Test)  -  verify job definition and job identity hash
  
   Copyright (C)
     2013,            Hermann Vosseler <Ichthyostega@web.de>
  
  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file job-hash-test.cpp
 ** unit test \ref JobHash_test
 */


#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "vault/real-clock.hpp"
#include "vault/gear/job.h"
#include "steam/engine/mock-dispatcher.hpp"

#include <boost/functional/hash.hpp>


namespace steam {
namespace engine{
namespace test  {
  
  using util::isSameObject;
  using vault::gear::Job;
  using vault::gear::JobParameter;
  
  
  
  
  /**********************************************************************//**
   * @test verify the basic properties of the job and job descriptor struct;
   *       especially verify that job data is passed properly back to the
   *       closure and that a identity can be constructed based on a
   *       hash of the job's data.
   * 
   * @see Job
   * @see JobClosure
   * @see SchedulerInterface_test
   */
  class JobHash_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          verify_simple_job_properties();
          verify_job_identity();
        }
      
      
      void
      verify_simple_job_properties()
        {
          MockJob job;
          Time beforeInvocation = RealClock::now();
          job.triggerJob();
          
          CHECK (MockJob::was_invoked (job));
          CHECK (RealClock::now() > MockJob::invocationTime (job));
          CHECK (beforeInvocation < MockJob::invocationTime (job));
        }
      
      
      void
      verify_job_identity()
        {
          MockJob job1;
          MockJob job2;
          
          CHECK (job1 != job2, "random test data clash");
          
          CHECK (hash_value(job1) != hash_value(job2));
          
          Job copy(job1);
          CHECK (!isSameObject (job1, copy));
          
          CHECK (copy == job1);
          CHECK (hash_value(job1) == hash_value(copy));
          
          copy.parameter.nominalTime++;
          CHECK (hash_value(job1) != hash_value(copy));  // hash value depends on the concrete nominal job time
          
          copy = job1;
          copy.parameter.invoKey.part.a++;
          CHECK (hash_value(job1) != hash_value(copy));  // hash value depends on the internal interpretation of the invocation key
          
          
                      class OtherClosure
                        : public JobClosure
                        {
                          void invokeJobOperation (JobParameter)              { /* irrelevant */ }
                          JobKind getJobKind()  const                         { return META_JOB; }
                          InvocationInstanceID buildInstanceID(HashVal) const { return InvocationInstanceID(); }
                          
                          size_t
                          hashOfInstance(InvocationInstanceID invoKey) const
                            {
                              return boost::hash_value (invoKey.part.a);
                            }
                        };
          OtherClosure someOtherClosure;
          
          copy = job1;
          copy.jobClosure = &someOtherClosure;
          CHECK (hash_value(job1) != hash_value(copy));  // hash value indeed depends on the concrete job closure instance
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER(JobHash_test, "unit engine");
  
}}} // namespace steam::engine::test
