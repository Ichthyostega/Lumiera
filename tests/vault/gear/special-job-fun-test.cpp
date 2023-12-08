/*
  SpecialJobFun(Test)  -  verify a disposable configurable job functor

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file special-job-fun-test.cpp
 ** unit test \ref SpecialJobFun_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
//#include "vault/real-clock.hpp"
//#include "lib/time/timevalue.hpp"
#include "vault/gear/special-job-fun.hpp"
#include "lib/format-cout.hpp" ////////////////////////////////////TODO Moo-oh
#include "lib/test/diagnostic-output.hpp"//////////////////////////TODO TOD-oh
#include "lib/test/testdummy.hpp"
//#include "lib/util.hpp"

//#include <array>
//#include <functional>

//using lib::time::Time;
//using lib::time::FSecs;

//using util::isnil;
//using util::isSameObject;
//using lib::test::randStr;
//using lib::test::randTime;
using lib::test::Dummy;
//using std::array;


namespace vault{
namespace gear {
namespace test {
  
  namespace { // shorthands and parameters for test...
    
    
  }//(End)test definitions
  
  
  
  
  /*****************************************************************//**
   * @test verify a self-managing one-time render job functor.
   * @see TestChainLoad_test::usageExample
   * @see TestChainLoad::ScheduleCtx::continuation()
   * @see special-job-fun.hpp
   */
  class SpecialJobFun_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          verifyLifecycle();
        }
      
      
      /** @test demonstrate simple usage by λ-binding
       * @todo WIP 12/23 🔁 define ⟶ ✔ implement
       */
      void
      simpleUsage()
        {
          bool hit{false};         // create directly from λ
          SpecialJobFun specialFun{[&](JobParameter){ hit=true; }};
          
          CHECK (specialFun);
          Job funJob{specialFun
                    ,InvocationInstanceID()
                    ,Time::ANYTIME
                    };
          
          funJob.triggerJob();
          CHECK (hit);
          CHECK (not specialFun);
        }
      
      
      
      /** @test verify storage and lifecycle management
       *      - use a instance-tracking marker implanted into the functor
       *      - verify no memory is leaked and the tracker instance is deallocated
       *      - verify the single tracker instance indeed lives in the JobFunctor
       *      - investigate the usage count of the front-end handle
       *      - verify the front-end can be copied without impact on the JobFunctor
       *      - verify the heap allocated functor keeps itself alive
       *        even when the front-end handle is already gone.
       *      - verify the functor de-allocates itself after latst invocation
       * @todo WIP 12/23 ✔ define ⟶ ✔ implement
       */
      void
      verifyLifecycle()
        {
          CHECK (0 == Dummy::checksum());
          {                        //  Note: ▽▽▽ tracker in λ-closure
            SpecialJobFun funTrack{[tracker=Dummy(23)]
                                   (JobParameter param) mutable
                                    {
                                      int mark = param.invoKey.part.a;
                                      tracker.setVal (mark);
                                    }};    // △△△ invocation should alter checksum
            
            // one Dummy instance was implanted
            CHECK (23 == Dummy::checksum());
            InvocationInstanceID hiddenMessage;
            hiddenMessage.part.a = 55;
            Job funJob{funTrack
                      ,hiddenMessage
                      ,Time::ANYTIME
                      };
            
            CHECK (23 == Dummy::checksum());
            funJob.triggerJob();
            CHECK (55 == Dummy::checksum());                             // the `funJob` front-end handle still keeps it alive
          }                                                              // but when this front-end goes out of scope...
          CHECK (0 == Dummy::checksum());                                // the implanted tracker is also gone
          
          {                                                              // another investigation with the same technique...
            auto trackingLambda = [tracker=Dummy(23)]
                                   (JobParameter param) mutable
                                    {
                                      int mark = param.invoKey.part.a;
                                      tracker.setVal (mark);
                                    };
            CHECK (23 == Dummy::checksum());
            
            SpecialJobFun frontEnd{move(trackingLambda)};                // this time the λ is moved in..
            CHECK (23 == Dummy::checksum());                             // the embedded tracker was copied into the Functor in heap memory
            CHECK (2 == frontEnd.use_count());                           // Note: both the front-end and the Functor in heap hold a use-reference
            
            auto otherHandle = frontEnd;                                 // copy of front-end...
            CHECK (3 == frontEnd.use_count());                           // ...so there are three usages of the front-end handle now
            CHECK (23 == Dummy::checksum());                             // ...but only one tracker instance (in heap)
            
            frontEnd = SpecialJobFun();                                  // re-assign one front-end handle with an empty instance
            CHECK (0 == frontEnd.use_count());                           // thus `frontEnd` is no longer attached to the active instance
            CHECK (2 == otherHandle.use_count());                        // but the other copy still is
            CHECK (not frontEnd);
            CHECK (otherHandle);
            
            InvocationInstanceID hiddenMessage;
            hiddenMessage.part.a = 55;
            Job funJob{otherHandle                                       // Note: don't pass the handle here, rather a JobFunctor& is extracted
                      ,hiddenMessage
                      ,Time::ANYTIME
                      };

            CHECK (2 == otherHandle.use_count());
            CHECK (23 == Dummy::checksum());
            
            otherHandle = SpecialJobFun();                               // now kill even the last front-end handle we had
            CHECK (0 == otherHandle.use_count());                        // thus _we_ have no way to reach the Functor in heap
            CHECK (23 == Dummy::checksum());                             // yet it stays alive, since it was not invoked yet
            
            funJob.triggerJob();                                         // after invocation, the Functor in heap memory self-destructs
            CHECK (0 == Dummy::checksum());                              // since it did hold the last reference
          }
          CHECK (0 == Dummy::checksum());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SpecialJobFun_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
