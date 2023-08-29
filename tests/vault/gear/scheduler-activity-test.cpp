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
          verifyActivity_Gate_pass();
          verifyActivity_Gate_dead();
          verifyActivity_Gate_block();
          verifyActivity_Gate_opened();
          
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
       *        - invoke the λ-post to dispatch the chain through the queue
       *        - the chain to be executed is given as `next`
       *        - time window for scheduling as data field
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
          
          CHECK (detector.verifyInvocation("CTX-post").arg("11.011", "Act(POST", "≺test::CTX≻"));
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
          
          uint64_t x1=rand(), x2=rand();
          Time nomTime = lib::test::randTime();
          Activity feed{x1,x2};
          Activity feed2{x1+1,x1+2};
          feed.next = &feed2;
          Activity invoke{detector.buildMockJobFunctor("job"), nomTime, feed};
          
          Time realTime = RealClock::now();
          CHECK (activity::PASS == invoke.activate (realTime, detector.executionCtx));
          
          CHECK (detector.verifyInvocation ("job").arg(nomTime, x1));
        }
      
      
      
      /** @test behaviour of Activity::NOTIFY when _activated_
       *        - notification is dispatched as special message to an indicated target Activity
       *        - when activated, a `NOTIFY`-Activity _posts itself_ through the Execution Context hook
       *        - this way, further processing will happen in management mode (single threaded)
       */
      void
      verifyActivity_Notify_activate()
        {
          Activity chain;
          Activity notify{&chain};
          
          ActivityDetector detector;
          Time tt{111,11};
          notify.activate (tt, detector.executionCtx);
          
          CHECK (detector.verifyInvocation("CTX-post").arg("11.111", "Act(NOTIFY", "≺test::CTX≻"));
        }
      
      
      
      /** @test behaviour of Activity::NOTIFY when activation leads to a _dispatch_
       *        - when _posting_ a `NOTIFY`, a dedicated _notification_ function is invoked on the chain
       *        - what actually happens then depends on the receiver; here we just activate a test-Tap
       */
      void
      verifyActivity_Notify_dispatch()
        {
          ActivityDetector detector;
                                   // use a diagnostic Tap as chain to detect passing of notification
          Activity notify{detector.buildActivationProbe("notifyTargetActivity")};
          
          Time tt{111,11};
          notify.dispatch (tt, detector.executionCtx);
          
          CHECK (detector.verifyInvocation("notifyTargetActivity").arg("11.111"));
        }
      
      
      
      /** @test behaviour of Activity::GATE:
       *        if conditions are met, the activation is just passed,
       *        so the executor (in the Scheduler) will just invoke the chain
       */
      void
      verifyActivity_Gate_pass()
        {
          Activity chain;
          Activity gate{0};
          gate.next = &chain;
          
          ActivityDetector detector;
          Activity& wiring = detector.buildGateWatcher (gate);
          
          Time tt{333,33};
          CHECK (activity::PASS == wiring.activate (tt, detector.executionCtx));
          CHECK (detector.verifyInvocation("tap-GATE").arg("33.333 ⧐ Act(GATE"));
        }
      
      
      
      /** @test behaviour of Activity::GATE:
       *        the rest of the chain is just skipped in case of deadline violation
       */
      void
      verifyActivity_Gate_dead()
        {
          Activity chain;
          Activity gate{0, Time{333,33}};
          gate.next = &chain;
          
          ActivityDetector detector;
          Activity& wiring = detector.buildGateWatcher (gate);
          
          Time t1{330,33};   // still before the deadline
          Time t2{333,33};   // exactly at deadline => rejected
          Time t3{335,33};   // after the deadline  => rejected
          
          CHECK (activity::PASS == wiring.activate (t1, detector.executionCtx));
          CHECK (detector.verifyInvocation("tap-GATE").arg("33.330 ⧐ Act(GATE").seq(0));
          
          detector.incrementSeq();
          CHECK (activity::SKIP == wiring.activate (t2, detector.executionCtx));
          CHECK (detector.verifyInvocation("tap-GATE").arg("33.333 ⧐ Act(GATE").seq(1));
          
          detector.incrementSeq();
          CHECK (activity::SKIP == wiring.activate (t3, detector.executionCtx));
          CHECK (detector.verifyInvocation("tap-GATE").arg("33.335 ⧐ Act(GATE").seq(2));
        }
      
      
      
      /** @test behaviour of Activity::GATE:
       *        the count-down condition determines if activation _passes_
       *        or will _spin around_ for later re-try
       */
      void
      verifyActivity_Gate_block()
        {
          Activity chain;
          Activity gate{23};
          gate.next = &chain;
          
          ActivityDetector detector;
          Activity& wiring = detector.buildGateWatcher (gate);
          
          Time tt{333,33};
          CHECK (activity::SKIP == wiring.activate (tt, detector.executionCtx));
          CHECK (23 == gate.data_.condition.rest);  //  prerequisite-count not altered
          
          Time reScheduled = detector.executionCtx.wait(tt);
          CHECK (tt < reScheduled);
          
          CHECK (detector.verifyInvocation("tap-GATE").arg("33.333 ⧐ Act(GATE")
                         .beforeInvocation("CTX-post").arg(reScheduled, "Act(GATE", "≺test::CTX≻"));
        }
      
      
      
      /** @test behaviour of Activity::GATE on notification
       *        - Gate configured initially such that it blocks
       *          (without violating deadline)
       *        - thus a regular activation signals to skip the chain,
       *          but also re-schedules a further check into the future
       *        - when receiving a notification, the latch is decremented
       *        - if this causes the Gate to open, the chain is immediately
       *          scheduled for activation, but the Gate also locked forever
       *        - neither a further activation, or a further notification
       *          has any effect after this point...
       */
      void
      verifyActivity_Gate_opened()
        {
          Activity chain;
          Activity gate{1};
          gate.next = &chain;
          // Conditionals in the gate block invocations
          CHECK (gate.data_.condition.isHold());
          CHECK (gate.data_.condition.rest == 1);
          CHECK (gate.data_.condition.dead == Time::NEVER);
          
          ActivityDetector detector;
          Activity& wiring = detector.buildGateWatcher (gate);
          
          Time tt{333,33};
          Time reScheduled = detector.executionCtx.wait(tt); // retrieve the next time to retry
          CHECK (tt < reScheduled);
          
          // an attempt to activate blocks (and re-schedules for later retry)
          CHECK (activity::SKIP == wiring.activate (tt, detector.executionCtx));
          CHECK (1 == gate.data_.condition.rest); // unchanged (and locked)...
          CHECK (detector.verifyInvocation("tap-GATE").arg("33.333 ⧐ Act(GATE")
                         .beforeInvocation("CTX-post").arg(reScheduled, "Act(GATE", "≺test::CTX≻"));
          
          detector.incrementSeq();
          // Gate receives a notification from some prerequisite Activity
          CHECK (activity::PASS == wiring.notify(tt, detector.executionCtx));
          CHECK (0 == gate.data_.condition.rest); // condition has been decremented...
          
          CHECK (detector.verifyInvocation("tap-GATE").seq(0).arg("33.333 ⧐ Act(GATE")
                         .beforeInvocation("CTX-post").seq(0).arg(reScheduled, "Act(GATE", "≺test::CTX≻")
                         .beforeInvocation("tap-GATE").seq(1).arg("33.333 --notify-↯> Act(GATE")
                         .beforeInvocation("CTX-post").seq(1).arg(tt, "afterGATE", "≺test::CTX≻"));
          CHECK (gate.data_.condition.dead == Time::MIN);
          
          detector.incrementSeq();
          Time ttt{444,44};
          // when the re-scheduled check happens later, it is blocked to prevent double activation
          CHECK (activity::SKIP == wiring.activate (ttt, detector.executionCtx));
          CHECK (detector.verifyInvocation("tap-GATE").seq(2).arg("44.444 ⧐ Act(GATE"));
          CHECK (detector.ensureNoInvocation("CTX-post").seq(2));
          CHECK (gate.data_.condition.dead == Time::MIN);
          
          detector.incrementSeq();
          // even a further notification has no effect now....
          wiring.notify(ttt, detector.executionCtx);
          // conditionals were not touched:
          CHECK (gate.data_.condition.dead == Time::MIN);
          CHECK (gate.data_.condition.rest == 0);
          // the log shows the further notification (at Seq=3) but no dispatch happens anymore
          CHECK (detector.verifySeqIncrement(3)
                         .beforeInvocation("tap-GATE").seq(3).arg("44.444 --notify-↯> Act(GATE"));
          CHECK (detector.ensureNoInvocation("CTX-post").seq(3).arg(tt, "afterGATE", "≺test::CTX≻"));
          
//        cout << detector.showLog()<<endl; // HINT: use this for investigation...
        }
      
      
      
      /** @test TODO verify the Activity term builder
       * @todo WIP 8/23 🔁 define ⟶ implement
       */
      void
      termBuilder()
        {
          ActivityDetector detector;
          
          BlockFlowAlloc bFlow;
          ActivityLang activityLang{bFlow};
          
          Job job = detector.buildMockJob();
          Time start{0,1};
          Time dead{0,10};
          auto term = activityLang.buildCalculationJob (job,start,dead);
          
          Activity& post = term.post();
          CHECK (Activity::POST == post.verb_);
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
