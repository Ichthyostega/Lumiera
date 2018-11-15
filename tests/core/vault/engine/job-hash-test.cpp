/*
  JobHash(Test)  -  verify job definition and job identity hash
  
  Copyright (C)         Lumiera.org
    2013,               Hermann Vosseler <Ichthyostega@web.de>
  
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

/** @file job-hash-test.cpp
 ** unit test \ref JobHash_test
 */


#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "vault/real-clock.hpp"
#include "vault/engine/dummy-job.hpp"

#include <boost/functional/hash.hpp>


namespace backend {
namespace engine {
namespace test {
  
  using util::isSameObject;
  
  
  
  
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
          verify_simple_job_properties();
          verify_job_identity();
        }
      
      
      void
      verify_simple_job_properties()
        {
          Job job = DummyJob::build();
          CHECK (job.isValid());
          
          Time beforeInvocation = RealClock::now();
          job.triggerJob();
          
          CHECK (DummyJob::was_invoked (job));
          CHECK (RealClock::now() > DummyJob::invocationTime (job));
          CHECK (beforeInvocation < DummyJob::invocationTime (job));
        }
      
      
      void
      verify_job_identity()
        {
          Job job1 = DummyJob::build();
          Job job2 = DummyJob::build();
          
          CHECK (job1 != job2, "random test data clash");
          
          CHECK (hash_value(job1) != hash_value(job2));
          
          Job copy(job1);
          CHECK (!isSameObject (job1, copy));
          
          CHECK (copy == job1);
          CHECK (hash_value(job1) == hash_value(copy));
          
          copy.parameter.nominalTime++;
          CHECK (hash_value(job1) != hash_value(copy));  // hash value depends on the concrete nominal job time
          
          copy = job1;
          copy.parameter.invoKey.metaInfo.a++;
          CHECK (hash_value(job1) != hash_value(copy));  // hash value depends on the internal interpretation of the invocation key
          
          
                      class OtherClosure
                        : public JobClosure
                        {
                          void invokeJobOperation (JobParameter)             { /* irrelevant */ }
                          void signalFailure (JobParameter,JobFailureReason) { /* irrelevant */ }
                          JobKind getJobKind()  const                        { return META_JOB; }
                          bool verify (Time, InvocationInstanceID)  const    { return false; }
                          
                          size_t
                          hashOfInstance(InvocationInstanceID invoKey) const
                            {
                              return boost::hash_value (invoKey.metaInfo.a);
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
  
}}} // namespace backend::engine::test
