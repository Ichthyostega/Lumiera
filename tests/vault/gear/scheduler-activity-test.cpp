/*
  SchedulerActivity(Test)  -  verify activities processed in the scheduler

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

/** @file scheduler-activity-test.cpp
 ** unit test \ref SchedulerActivity_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "activity-detector.hpp"
#include "vault/gear/activity-lang.hpp"
#include "vault/real-clock.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"  /////////////////////////////////////TODO
//#include "lib/util.hpp"

//#include <utility>

using test::Test;
using lib::time::Time;
using lib::time::FSecs;
//using std::move;
//using util::isSameObject;


namespace vault{
namespace gear {
namespace test {
  
//  using lib::time::FrameRate;
//  using lib::time::Offset;
//  using lib::time::Time;
  
  
  
  
  
  /*****************************************************************//**
   * @test verify behaviour of the Scheduler _Activity Language._
   * @see SchedulerCommutator_test
   * @see SchedulerUsage_test
   */
  class SchedulerActivity_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          
          verifyActivity_Post();
          verifyActivity_Invoke();
          verifyActivity_Notify_activate();
          verifyActivity_Notify_dispatch();
          verifyActivity_Gate();
          
          termBuilder();
          
          scenario_RenderJob();
          scenario_IOJob();
          scenario_MetaJob();
        }
      
      
      /** @test demonstrate simple Activity usage */
      void
      simpleUsage()
        {
          // Activities are »POD with constructor«
          Activity start{Activity::WORKSTART};
          CHECK (start.verb_ == Activity::WORKSTART);
          CHECK (start.next  == nullptr);
          CHECK (start.data_.timing.instant == Time::NEVER);       //////////////////////////////////////////TICKET #1317 : the purpose of this time data is not clear yet
          CHECK (start.data_.timing.quality == 0);
          
          // use the ActivityDetector for test instrumentation...
          ActivityDetector detector;
          
          // Activities can be invoked within an ExecutionContext
          Time now = RealClock::now();
          start.activate (now, detector.executionCtx);
          
          // In this case, activation causes invocation of λ-work on the context
          CHECK (detector.verifyInvocation("CTX-work").arg(now, 0));
          
//        cout << detector.showLog()<<endl; // HINT: use this for investigation...
        }
      
      
      
      /** @test behaviour of Activity::POST
       * @todo WIP 8/23 🔁 define ⟶ implement
       */
      void
      verifyActivity_Post()
        {
          Activity chain;
          Activity post{Time{0,11}, Time{0,22}, &chain};
          CHECK (Activity::TICK == chain.verb_);
          CHECK (Activity::POST == post.verb_);
          CHECK (Time(0,11) == post.data_.timeWindow.life);
          CHECK (Time(0,22) == post.data_.timeWindow.dead);
          CHECK ( & chain   == post.next);
          
          ActivityDetector detector;
          Time tt{11,11};
          post.activate (tt, detector.executionCtx);
          
          cout << detector.showLog() <<endl;
          CHECK (detector.verifyInvocation("CTX-post").arg("11.011", "Act(TICK", "≺test::CTX≻"));   ///////////////////OOO need somehow to transport the time window...
        }
      
      
      
      /** @test behaviour of Activity::INVOKE
       *        - setup requires two FEED-Activities to be chained up as arguments
       *        - use the rigged execution context provided by ActivityDetector
       *        - can verify this way that the activation leads to JobFunctor invocation
       */
      void
      verifyActivity_Invoke()
        {
          ActivityDetector detector;
          
          size_t x1=rand(), x2=rand();
          Time nomTime = lib::test::randTime();
          Activity feed{x1,x2};
          Activity feed2{x1+1,x1+2};
          feed.next = &feed2;
          Activity invoke{detector.buildMockJobFunctor("job"), nomTime, feed};
          
          Time realTime = RealClock::now();
          CHECK (activity::PASS == invoke.activate (realTime, detector.executionCtx));
          
          CHECK (detector.verifyInvocation ("job").arg(nomTime, x1));
        }
      
      
      
      /** @test TODO behaviour of Activity::NOTIFY
       *        - notification is dispatched as special message to an indicated target Activity
       *        - when activated, a `NOTIFY`-Activity _posts itself_ through the Execution Context hook
       *        - this way, further processing will happen in management mode (single threaded)
       * @todo WIP 8/23 🔁 define ⟶ implement
       */
      void
      verifyActivity_Notify_activate()
        {
          Activity chain;
          Activity notify{&chain};
          
          ActivityDetector detector;
          Time tt{111,11};
          notify.activate (tt, detector.executionCtx);
          cout << detector.showLog() <<endl;
        }
      
      
      
      /** @test TODO behaviour of Activity::NOTIFY
       *        - when _posting_ a `NOTIFY`, a dedicated _notification_ function is invoked on the chain
       *        - what actually happens then depends on the receiver; here we just activate a test-Tap
       * @todo WIP 8/23 🔁 define ⟶ implement
       */
      void
      verifyActivity_Notify_dispatch()
        {
          ActivityDetector detector;
          
          Activity notify{detector.buildActivationProbe("targetActivity")};
          
          Time tt{111,11};
          notify.dispatch (tt, detector.executionCtx);
          cout << detector.showLog() <<endl;
        }
      
      
      
      /** @test TODO behaviour of Activity::GATE
       * @todo WIP 7/23 ⟶ define ⟶ implement
       */
      void
      verifyActivity_Gate()
        {
        }
      
      
      
      /** @test TODO verify the Activity term builder
       * @todo WIP 7/23 ⟶ define ⟶ implement
       */
      void
      termBuilder()
        {
        }
      
      
      
      /** @test TODO usage scenario: Activity graph for a render job
       * @todo WIP 7/23 ⟶ define ⟶ implement
       */
      void
      scenario_RenderJob()
        {
        }
      
      
      
      /** @test TODO usage scenario: Activity graph for an async Job
       * @todo WIP 7/23 ⟶ define ⟶ implement
       */
      void
      scenario_IOJob()
        {
        }
      
      
      
      /** @test TODO usage scenario: Activity graph for administrative job
       * @todo WIP 7/23 ⟶ define ⟶ implement
       */
      void
      scenario_MetaJob()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SchedulerActivity_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
