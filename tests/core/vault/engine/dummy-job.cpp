/*
    DummyJob  -  diagnostic job for unit tests

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

/** @file dummy-job.cpp
 ** Implementation of a dummy render job for unit tests
 */


#include "vault/engine/dummy-job.hpp"

#include "lib/test/test-helper.hpp"
#include "lib/time/timevalue.hpp"
#include "vault/real-clock.hpp"
#include "lib/null-value.hpp"
#include "lib/hash-value.h"
#include "lib/util.hpp"

#include <cstdlib>
#include <unordered_map>
#include <functional>


namespace vault{
namespace engine {
  
  namespace { // DummyJob implementation details...
    
    using lib::HashVal;
    using lib::NullValue;
    using lib::time::TimeVar;
    using std::unordered_map;
    using util::access_or_default;
    
    
    const int MAX_PARAM_A(1000);   ///< random test values 0...1000
    const int MAX_PARAM_B(10);     ///< random test values -10...+10
    
    
    /**
     * test dummy jobs are backed by this closure.
     * DummyJob invocations are recorded in a hashtable
     * @note as of 9/2013, we use a very simplistic implementation,
     *       causing a consecutive invocation of the same job instance
     *       to overwrite the previous log entry.
     */
    class DummyClosure
      : public JobClosure
      {
        void
        invokeJobOperation (JobParameter parameter)  override
          {
            invocationLog_[hash_value (parameter)] = Invocation(parameter);
          }
        
        void
        signalFailure (JobParameter,JobFailureReason)  override
          {
            NOTREACHED ("Job failure is not subject of this test");
          }
        
        JobKind
        getJobKind()  const override
          {
            return META_JOB;
          }
        
        bool
        verify (Time nominalJobTime, InvocationInstanceID invoKey)  const override
          {
            return Time::ANYTIME < nominalJobTime
                && 0 <= invoKey.part.a
                && invoKey.part.a < MAX_PARAM_A
                && -MAX_PARAM_B <= invoKey.part.b
                && invoKey.part.b < MAX_PARAM_B;
          }
        
        /**
         * Generate a specifically marked invocationKey for use in unit-tests.
         * @remark in the actual implementation, this function generates a distinct
         *         base hash do tag specific processing provided by this JobFunctor;
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
        
        /** recording DummyJob invocations */
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
    
    
    
    /** actual instance of the test dummy job operation */
    DummyClosure dummyClosure;
    
  }// (End)Implementation details
  
  
  
  
  
  
  
  
  Job
  DummyJob::build()
  {
    InvocationInstanceID invoKey;
    invoKey.part.a = rand() % MAX_PARAM_A;
    invoKey.part.b = rand() % (2*MAX_PARAM_B)  - MAX_PARAM_B;
    
    Time nominalTime = lib::test::randTime();
    
    return Job(dummyClosure, invoKey, nominalTime);
  }
  
  
  Job
  DummyJob::build (Time nominalTime, int additionalKey)
  {
    InvocationInstanceID invoKey;
    invoKey.part.a = additionalKey;
    invoKey.part.b = rand() % (2*MAX_PARAM_B)  - MAX_PARAM_B;
    
    return Job(dummyClosure, invoKey, nominalTime);
  }
  
  
  bool
  DummyJob::was_invoked (Job const& job)
  {
    REQUIRE (job.usesClosure (dummyClosure));
    
    return Time::NEVER != dummyClosure.queryInvocation(job.parameter).real;
  }
  
  
  Time
  DummyJob::invocationTime (Job const& job)
  {
    REQUIRE (job.usesClosure (dummyClosure));
    
    return dummyClosure.queryInvocation(job.parameter).real;
  }
  
  
  Time
  DummyJob::invocationNominalTime (Job const& job)
  {
    REQUIRE (job.usesClosure (dummyClosure));
    
    return dummyClosure.queryInvocation(job.parameter).nominal;
  }
  
  
  int
  DummyJob::invocationAdditionalKey (Job const& job)
  {
    REQUIRE (job.usesClosure (dummyClosure));
    
    return dummyClosure.queryInvocation(job.parameter).a;
  }
  
  
  /** @internal for collaboration with other Mock/Dummy facilities */
  JobClosure&
  DummyJob::getFunctor()
  {
    return dummyClosure;
  }

  
  
}} // namespace vault::engine
