/*
  DUMMY-JOB.hpp  -  diagnostic job for unit tests

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

*/


#ifndef BACKEND_ENGINE_DUMMY_JOB_H
#define BACKEND_ENGINE_DUMMY_JOB_H



#include "backend/engine/job.h"
#include "lib/test/test-helper.hpp"

#include <cstdlib>
#include <boost/functional/hash.hpp>


namespace backend{
namespace engine {
  
  namespace {
    
    class DummyClosure
      : public JobClosure
      {
        void
        invokeJobOperation (JobParameter parameter)
          {
            UNIMPLEMENTED ("conjure a verifiable dummy job operation");
          }
        
        void
        signalFailure (JobParameter,JobFailureReason)
          {
            NOTREACHED ("Job failure is not subject of this test");
          }
        
        JobKind
        getJobKind()  const
          {
            return META_JOB;
          }
        
        bool
        verify (Time nominalJobTime, InvocationInstanceID invoKey)  const
          {
            UNIMPLEMENTED ("what the hell do we need to mock for this operation????");
          }
        
        size_t
        hashOfInstance(InvocationInstanceID invoKey) const
          {
            return boost::hash_value (invoKey.metaInfo.a);
          }
      };
    
    /** actual instance of the test dummy job operation */
    DummyClosure dummyClosure;
    
    const int MAX_PARAM_A(1000);
    const int MAX_PARAM_B(10);
  }
  
  
  struct DummyJob
    {
      static Job
      build()
        {
          InvocationInstanceID invoKey;
          invoKey.metaInfo.a = rand() % MAX_PARAM_A;
          invoKey.metaInfo.b = rand() % MAX_PARAM_B;
          
          Time nominalTime = lib::test::randTime();
          
          return Job(dummyClosure, invoKey, nominalTime);
        }
      
      static Job
      build (Time nominalTime, int additionalKey)
        {
          InvocationInstanceID invoKey;
          invoKey.metaInfo.a = additionalKey;
          invoKey.metaInfo.b = rand() % MAX_PARAM_B;
          
          return Job(dummyClosure, invoKey, nominalTime);
        }
      
      static bool
      was_invoked (Job const& job)
        {
          UNIMPLEMENTED ("look up invocation from logging hashtable");
        }
      
      static Time
      invocationTime (Job const& job)
        {
          UNIMPLEMENTED ("look up invocation from logging hashtable");
        }
      
    };
  

}} // namespace backend::engine
#endif
