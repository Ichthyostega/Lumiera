/*
    MockDispatcher  -  diagnostic render job and frame dispatcher

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

/** @file mock-dispatcher.cpp
 ** Implementation of a dummy render job for unit tests.
 ** Based on using a specifically rigged DummyClosure as JobFunctor,
 ** where the actual Job invocation does nothing other than storing
 ** the invocation mark and parameters into a invocationLog_ table.
 ** Together with the likewise specifically rigged steam::engine::test::MockJobTicket,
 ** the invocation hash can be marked, which allows to prove after the invocation
 ** that a given Segment or JobTicket actually generated a specific Job, which was
 ** then invoked with specific parameters.
 ** 
 ** # Usage front-end
 ** 
 ** A MockJob can directly created, and then sliced down to the Job baseclass,
 ** since it has no additional data fields. The static functions in MockJob allow
 ** to verify that a given job instance was created from this setup, that it was
 ** invoked, and verify invocation time and extract data
 */


#include "steam/engine/mock-dispatcher.hpp"
#include "vault/gear/nop-job-functor.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/time/timevalue.hpp"
#include "vault/real-clock.hpp"
#include "lib/hash-combine.hpp"
#include "lib/null-value.hpp"
#include "lib/depend.hpp"
#include "lib/util.hpp"

#include <functional>
#include <unordered_map>


namespace steam {
namespace engine{
namespace test  {
  
  namespace { // MockJob and DummyClosure implementation details...
    
    using lib::rani;
    using lib::HashVal;
    using lib::NullValue;
    using lib::time::TimeVar;
    using std::unordered_map;
    using util::access_or_default;
    
    using vault::gear::JobParameter;
    
    
    const int MAX_PARAM_A(1000);   ///< random test values 0...1000
    const int MAX_PARAM_B(10);     ///< random test values -10...+10
    
    
    /**
     * MockJob objects are backed by this closure.
     * Invocations of this job functor are recorded in a hashtable
     * @note as of 5/2023, we use a simplistic map-based implementation,
     *       causing a consecutive invocation of the same job instance
     *       with identical JobParameter to overwrite the previous log entry.
     */
    class DummyClosure
      : public JobClosure
      {
        void
        invokeJobOperation (JobParameter parameter)  override
          {
            invocationLog_[hash_value (parameter)] = Invocation(parameter);
          }
        
        JobKind
        getJobKind()  const override
          {
            return META_JOB;
          }
        
        /**
         * Generate a specifically marked invocationKey for use in unit-tests.
         * @remark in the actual implementation, this function generates a distinct
         *         base hash to tag specific processing provided by this JobFunctor;
         *         the seed usually factors in the media stream format; on invocation
         *         the nominal frame time will additionally be hashed in. Yet for
         *         unit testing, we typically use this dummy JobFunctor and it is
         *         expedient if this hash-chaining calculation is easy predictable;
         * @return a zero-initialised invocationKey, assigning seed to the lower part
         */
        InvocationInstanceID
        buildInstanceID (HashVal seed)  const override
          {
            InvocationInstanceID instance;
            instance.part.a = seed;
            return instance;
          }
        
        size_t
        hashOfInstance (InvocationInstanceID invoKey) const override
          {
            std::hash<size_t> hashr;
            HashVal res = hashr (invoKey.frameNumber);
            lib::hash::combine (res, hashr (invoKey.part.t));
            return res;
          }
        
        
        /* === Logging/Reporting of job invocation === */
        
        struct Invocation
          {
            TimeVar nominal;
            TimeVar real;
            int a,b;
            
            Invocation (JobParameter param)
              : nominal{TimeValue(param.nominalTime)}
              , real{RealClock::now()}
              , a{param.invoKey.part.a}
              , b{param.invoKey.part.b}
              { }
            
            Invocation()
              : nominal{Time::ANYTIME}
              , real{Time::NEVER}
              , a{MAX_PARAM_A}, b{0}
              { }
          };
        
        /** recording MockJob invocations */
        unordered_map<HashVal,Invocation> invocationLog_;
        
        
      public:
        Invocation const&
        queryInvocation (JobParameter param)  const
          {
            return access_or_default (invocationLog_, hash_value(param)
                                     ,NullValue<Invocation>::get());
          }
        
        void
        clearLog()
          {
            invocationLog_.clear();
          }
      };
    
    
    
    /** actual instance of the test dummy job functor */
    DummyClosure dummyClosure;
    
    /** access to the fallback-implementation for empty segments */
    lib::Depend<vault::gear::NopJobFunctor> nopFunctor;
    
  }// (End)Implementation details
  
  
  
  
  
  
  
  
  Job
  MockJob::build()
  {
    InvocationInstanceID invoKey;
    invoKey.part.a = rani (MAX_PARAM_A);
    invoKey.part.b = rani (2*MAX_PARAM_B - MAX_PARAM_B);
    
    Time nominalTime = lib::test::randTime();
    
    return Job(dummyClosure, invoKey, nominalTime);
  }
  
  
  Job
  MockJob::build (Time nominalTime, int additionalKey)
  {
    InvocationInstanceID invoKey;
    invoKey.part.a = additionalKey;
    invoKey.part.b = rani (2*MAX_PARAM_B - MAX_PARAM_B);
    
    return Job(dummyClosure, invoKey, nominalTime);
  }
  
  
  bool
  MockJob::was_invoked (Job const& job)
  {
    REQUIRE (job.usesClosure (dummyClosure));
    
    return Time::NEVER != dummyClosure.queryInvocation(job.parameter).real;
  }
  
  
  Time
  MockJob::invocationTime (Job const& job)
  {
    REQUIRE (job.usesClosure (dummyClosure));
    
    return dummyClosure.queryInvocation(job.parameter).real;
  }
  
  
  Time
  MockJob::invocationNominalTime (Job const& job)
  {
    REQUIRE (job.usesClosure (dummyClosure));
    
    return dummyClosure.queryInvocation(job.parameter).nominal;
  }
  
  
  int
  MockJob::invocationAdditionalKey (Job const& job)
  {
    REQUIRE (job.usesClosure (dummyClosure));
    
    return dummyClosure.queryInvocation(job.parameter).a;
  }
  
  
  /** @internal for collaboration with other Mock/Dummy facilities */
  JobClosure&
  MockJob::getFunctor()
  {
    return dummyClosure;
  }

  /** @internal likewise to support the MockDispatcher diagnostics;
   *            locate here since this is a dedicated translation unit
   * @return `true` iff the job was defined in the typical way used by
   *         JobTicket to generate fill jobs for empty segments.
   * @see JobTicket::JobTicket::createJobFor(nominalTime)
   */
  bool
  MockJob::isNopJob (Job const& job)
  {
    InvocationInstanceID empty; ///////////////////////////////////////////////////////////////////////TICKET #1287 : temporary workaround until we get rid of the C base structs
    JobClosure& jobFunctor = static_cast<JobClosure&> (*job.jobClosure);     //////////////////////////TICKET #1287 : fix actual interface down to JobFunctor (after removing C structs)
    return lumiera_invokey_eq (&util::unConst(job).parameter.invoKey, &empty)
       and util::isSameObject (jobFunctor, nopFunctor());
  }

  
  
}}} // namespace steam::engine::test
