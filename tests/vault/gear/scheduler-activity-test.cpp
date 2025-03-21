/*
  SchedulerActivity(Test)  -  verify activities processed in the scheduler

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file scheduler-activity-test.cpp
 ** unit test \ref SchedulerActivity_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "activity-detector.hpp"
#include "vault/gear/activity-lang.hpp"
#include "vault/real-clock.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"


using test::Test;
using lib::time::Time;
using lib::time::FSecs;


namespace vault{
namespace gear {
namespace test {
  
  
  
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
          seedRand();
          
          simpleUsage();
          
          verifyActivity_Post();
          verifyActivity_Invoke();
          verifyActivity_Notify();
          verifyActivity_Gate_pass();
          verifyActivity_Gate_dead();
          verifyActivity_Gate_block();
          verifyActivity_Gate_notify();
          verifyActivity_Gate_opened();
          
          termBuilder();
          dispatchChain();
          
          scenario_RenderJob();
          scenario_Notification();
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
       *        - the start time from this window becomes the `when` parameter
       *        - the `now` parameter from the activation is thus ignored
       */
      void
      verifyActivity_Post()
        {
          Activity chain;
          Activity post{Time{0,11}, Time{0,22}, &chain};
          CHECK (chain.is (Activity::TICK));
          CHECK (post .is (Activity::POST));
          CHECK (Time(0,11) == post.data_.timeWindow.life);
          CHECK (Time(0,22) == post.data_.timeWindow.dead);
          CHECK ( & chain   == post.next);
          
          ActivityDetector detector;
          Time tt{5,5};
          post.activate (tt, detector.executionCtx);
          
          CHECK (detector.verifyInvocation("CTX-post").arg("11.000","22.000", "Act(POST", "≺test::CTX≻"));
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
          
          uint64_t x1=rani(), x2=rani();
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
       *        - when activated, a `NOTIFY`-Activity invokes the λ-post _with its target,_
       *        - in the actual setup (Scheduler) this leads to [dispatching](\ref Activity::dispatch)
       *          of said target Activity
       *        - what happens then depends on the target; usually the target is a `GATE`
       *        - in first example here, we just use a `TICK`-Activity
       *        - for a `GATE` there is special treatment to inject the _timing window of the target_
       *          into the CTX-post invocation; this is essential to handle long notification-chains properly.
       */
      void
      verifyActivity_Notify()
        {
          Activity chain;
          Activity notify{&chain};
          
          ActivityDetector detector;
          Time tt{111,11};
          notify.activate (tt, detector.executionCtx);
          CHECK (detector.verifyInvocation("CTX-post").arg("11.111", Time::NEVER, "Act(TICK", "≺test::CTX≻"));
          
          detector.incrementSeq();
          // now we use a `GATE` as target
          Time ts{333,33};
          Time td{555,55};
          Activity gate{1,td};
          notify.data_.notification.target = &gate;
          notify.data_.notification.timing = ts;   // start time hint can be packaged into the notification
          
          notify.activate (tt, detector.executionCtx);
          CHECK (detector.verifySeqIncrement(1)
                         .beforeInvocation("CTX-post").arg("33.333", "55.555", "Act(GATE", "≺test::CTX≻"));
                                                   // NOTE: △△△ start △△△ deadline
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
       *        the count-down condition determines if activation _passes;_
       *        otherwise the Gate will just return activity::SKIP
       * @remark in the original design, the Gate would poll for changes
       *        by re-scheduling itself into the Future; this behaviour
       *        turned out to be unnecessary and problematic.
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
          
          CHECK (detector.verifyInvocation("tap-GATE").arg("33.333 ⧐ Act(GATE"));
        }
      
      
      
      /** @test behaviour of Activity::GATE:
       *      - if it is _dispatched_ as new chain, instead of just _activating_
       *        it of part of an ongoing chain, the Gate will receive a *notification*
       *      - this results in _decrementing_ the prerequisite latch in the Gate
       *      - what happens then depends on current state; in this test case
       *        the Gate is decremented yet remains closed
       */
      void
      verifyActivity_Gate_notify()
        {
          Activity chain;
          Activity gate{23};
          gate.next = &chain;
          
          ActivityDetector detector;
          Activity& entrance = detector.buildGateWatcher (gate);
          
          Time tt{333,33};
          CHECK (activity::SKIP == entrance.dispatch (tt, detector.executionCtx));
          CHECK (22 == gate.data_.condition.rest);  //  prerequisite-count decremented
          
          CHECK (detector.verifyInvocation("tap-GATE").arg("33.333 --notify-↯> Act(GATE"));
        }
      
      
      
      /** @test behaviour of Activity::GATE on notification
       *        - Gate configured initially such that it blocks
       *          (without violating deadline)
       *        - thus a regular activation signals to skip the chain.
       *        - when receiving a notification, the latch is decremented
       *        - if this causes the Gate to open, the follow-up chain will
       *          be activated immediately, but the Gate also locked forever
       *        - neither a further activation, nor a further notification
       *          has any effect after this point...
       */
      void
      verifyActivity_Gate_opened()
        {
          Time tt{333,33};
          Time td{555,55};
          
          Activity chain;
          Activity gate{1,td};
          gate.next = &chain;
          // Conditionals in the gate block invocations
          CHECK (gate.data_.condition.isHold());
          CHECK (gate.data_.condition.rest == 1);
          CHECK (gate.data_.condition.dead == td);
          
          ActivityDetector detector;
          Activity& entrance = detector.buildGateWatcher (gate);
          
          // an attempt to activate blocks (returning SKIP, nothing else happens)
          CHECK (activity::SKIP == entrance.activate (tt, detector.executionCtx));
          CHECK (1 == gate.data_.condition.rest);  // unchanged (and locked)...
          CHECK (detector.verifyInvocation("tap-GATE").arg("33.333 ⧐ Act(GATE"));
          
          detector.incrementSeq();
          // Gate receives a notification from some prerequisite Activity
          CHECK (activity::PASS == entrance.dispatch(tt, detector.executionCtx));
          CHECK (0 == gate.data_.condition.rest); // condition has been decremented...
          
          CHECK (detector.verifyInvocation("tap-GATE").seq(0).arg("33.333 ⧐ Act(GATE")
                         .beforeInvocation("tap-GATE").seq(1).arg("33.333 --notify-↯> Act(GATE"));
          CHECK (gate.data_.condition.dead == Time::MIN);
          
          detector.incrementSeq();
          Time ttt{444,44};
          // when another activation happens later, it is blocked to prevent double activation
          CHECK (activity::SKIP == entrance.activate (ttt, detector.executionCtx));
          CHECK (detector.verifyInvocation("tap-GATE").seq(2).arg("44.444 ⧐ Act(GATE"));
          CHECK (detector.ensureNoInvocation("CTX-post").seq(2)
                         .afterInvocation("tap-GATE").seq(2));
          CHECK (gate.data_.condition.dead == Time::MIN);
          
          detector.incrementSeq();
          // even a further notification has no effect now....
          CHECK (activity::SKIP == entrance.dispatch (ttt, detector.executionCtx));
          // conditionals were not touched:
          CHECK (gate.data_.condition.dead == Time::MIN);
          CHECK (gate.data_.condition.rest == 0);
          // the log shows the further notification (at Seq=3) but no dispatch happens anymore
          CHECK (detector.verifySeqIncrement(3)
                         .beforeInvocation("tap-GATE").seq(3).arg("44.444 --notify-↯> Act(GATE"));
          
//        cout << detector.showLog()<<endl; // HINT: use this for investigation...
        }
      
      
      
      /** @test verify the Activity term builder
       *        - use the builder syntax to define a simple Activity chain
       *        - verify the basic outfitting and sane connectivity
       *        - verify values reported by the BlockFlow allocator
       *        - ensure the defined Job can be properly invoked
       */
      void
      termBuilder()
        {
          ActivityDetector detector;
          
          BlockFlowAlloc bFlow;
          ActivityLang activityLang{bFlow};

          Time start{0,1};
          Time dead{0,10};
          Activity* act{nullptr};
          {
            auto term = activityLang.buildCalculationJob (detector.buildMockJob(), start,dead);
            
            act = & term.post();
          }// NOTE: generated Activity chain remains valid after term goes out of scope
          
          // Values reported for the BlockFlow allocator look sane...
          CHECK (watch(bFlow).cntElm()    == 7);               // POST, GATE, WORKSTART, INVOKE, FEED, FEED, WORKSTOP
          CHECK (watch(bFlow).cntEpochs() == 1);               // all placed into a single epoch...
          CHECK (watch(bFlow).find(*act) > dead);              // which terminates shortly after the given deadline
          CHECK (watch(bFlow).find(*act) < dead+Time(500,0));
          
          // Time window parameters have been included
          CHECK (act->is (Activity::POST));
          CHECK (start == act->data_.timeWindow.life);
          CHECK (dead  == act->data_.timeWindow.dead);
          
          // sane wiring, leading to an INVOCATE eventually
          while (not act->is (Activity::INVOKE))
            act = act->next;
          
          CHECK (act->is (Activity::INVOKE));
          CHECK (watch(bFlow).find(*act) != Time::NEVER);      // can also be found within the BlockFlow allocator
          
          // this invocation is properly defined and executable
          Time now{55,5};
          CHECK (activity::PASS == act->activate (now, detector.executionCtx));
          CHECK (detector.verifyInvocation("mockJob"));
        }
      
      
      
      /** @test verify the ability to _dispatch and perform_ a chain of activities.
       *        - use a directly wired, arbitrary chain
       *        - dispatch will activate all Activities
       *        - however, when the Gate is configured to be blocked
       *          (waiting on prerequisites), then the rest of the chain is not activated.
       *        - the dispatch function also handles the notifications;
       *          when a notification towards the Gate is dispatched, the Gate is
       *          decremented and thereby opened; activation of the rest of the chain
       *          is then planned (but not executed synchronously in the same call)
       */
      void
      dispatchChain()
        {
          Time tt{11,1};
          Time td{22,2};
          Activity tick;
          Activity gate{0,td};
          gate.next = &tick;
          Activity post{tt, &gate};
          // so now we have POST ⟶ GATE ⟶ TICK;
          
          ActivityDetector detector;
          detector.executionCtx.getSchedTime = [&]{ return tt; };
          // insert instrumentation to trace activation
          detector.watchGate (post.next, "Gate");
          
          CHECK (activity::PASS == ActivityLang::dispatchChain (&post, detector.executionCtx));         // start execution (case/seq == 0)
          CHECK (detector.verifyInvocation("Gate")      .arg("1.011 ⧐ Act(GATE")                        // ...first the Gate was activated
                         .beforeInvocation("after-Gate").arg("1.011 ⧐ Act(TICK")                        // ...then activation passed out of Gate...
                         .beforeInvocation("CTX-tick")  .arg("1.011"));                                 // ...and finally the TICK invoked the λ-tick

          detector.incrementSeq();
          gate.data_.condition.incDependencies(); // Gate is blocked
          CHECK (activity::PASS == ActivityLang::dispatchChain (&post, detector.executionCtx));         // start execution (case/seq == 1)
          CHECK (detector.verifyInvocation("Gate").seq(1).arg("1.011 ⧐ Act(GATE"));                     // ...the Gate was activated, but blocked...
          CHECK (detector.ensureNoInvocation("after-Gate").seq(1)                                       // verify activation was not passed out behind Gate
                         .afterInvocation("Gate").seq(1));
          CHECK (detector.ensureNoInvocation("CTX-tick").seq(1)                                         // verify also the λ-tick was not invoked this time
                         .afterInvocation("Gate").seq(1));
          
          detector.incrementSeq();
          // Notification via instrumented connection to the Gate (activate(NOTIFY) -> λ-post(target) -> notify GATE
          CHECK (activity::PASS == ActivityLang::dispatchChain (post.next, detector.executionCtx));     // dispatch a notification (case/seq == 2)
          CHECK (0 == gate.data_.condition.rest);                                                       // Effect of the notification is to decrement the latch
          CHECK (detector.verifyInvocation("Gate")      .seq(2).arg("1.011 --notify-↯> Act(GATE")       // ...notification dispatched towards the Gate
                         .beforeInvocation("after-Gate").seq(2).arg("1.011 ⧐ Act(TICK")                 // ...this opened the Gate, passing activation...
                         .beforeInvocation("CTX-tick")  .seq(2).arg("1.011"));                          // ...to the chain, finally invoking λ-tick
        }
      
      
      
      
      /** @test usage scenario: Activity graph for a simple render job
       *        - build an activity term based on the »CalculationJob« wiring template
       *        - dispatch the generated Activity chain and verify sequence of invocations
       */
      void
      scenario_RenderJob()
        {
          Time nominal{7,7};
          
          Time start{0,1};
          Time dead{0,10};
          
          ActivityDetector detector;
          Job testJob{detector.buildMockJob("testJob", nominal, 12345)};
          
          TimeVar now = Time{5,5};
          detector.executionCtx.getSchedTime = [&]{ // increase "current" time on each access
                                                    now += FSecs(1,20);
                                                    return now;
                                                  };
          BlockFlowAlloc bFlow;
          ActivityLang activityLang{bFlow};
          
          // Build the Activity-Term for a simple calculation job...
          Activity& anchor = activityLang.buildCalculationJob (testJob, start,dead)
                                         .post(); // retrieve the entrance point to the chain
          
          // insert instrumentation to trace activation
          detector.watchGate (anchor.next, "theGate");
          
          CHECK (activity::PASS == ActivityLang::dispatchChain (&anchor, detector.executionCtx));
          
          CHECK (detector.verifyInvocation("theGate").arg("5.105 ⧐ Act(GATE")
                         .beforeInvocation("after-theGate").arg("⧐ Act(WORKSTART")
                         .beforeInvocation("CTX-work").arg("5.155","")
                         .beforeInvocation("testJob") .arg("7.007",12345)
                         .beforeInvocation("CTX-done").arg("5.355",""));
        }
      
      
      
      /** @test usage scenario: Notification from prerequisite Jobs within time window
       *        - build [similar](\ref #scenario_RenderJob) »CalculationJob« wiring
       *        - configure extended dependency notification capabilities
       *        - Case-1 : a Notification decreases the latch, but blocks otherwise
       *        - Case-2 : when the primary chain is activated after the Notification,
       *          then the tail chain behind the Gate is dispatched
       */
      void
      scenario_Notification()
        {
          Time nominal{7,7};
          Time start{0,1};
          Time dead{0,10};
          
          ActivityDetector detector;
          Job testJob{detector.buildMockJob("testJob", nominal, 12345)};
          
          BlockFlowAlloc bFlow;
          ActivityLang activityLang{bFlow};
          
          // emulate a blocking prerequisite dependency
          Activity trigger{Activity::NOTIFY};
          
          // Build the Activity-Term...
          auto term = activityLang.buildCalculationJob (testJob, start,dead)
                                  .expectNotification (trigger)               // ...require notification from prerequisite
                                  .requireDirectActivation();                 // ...additionally insert inhibition to avoid activation
                                                                              //    before the primary-chain has been scheduled
          Activity& anchor = term.post();
          CHECK (anchor.is (Activity::POST));
          CHECK (anchor.next->is (Activity::NOTIFY));
          CHECK (anchor.next->next->is (Activity::GATE));
          CHECK (anchor.next->next->next->is (Activity::WORKSTART));
          CHECK (anchor.next->next->next->next->is (Activity::INVOKE));
          CHECK (anchor.next->next->next->next->next->is (Activity::FEED));
          CHECK (anchor.next->next->next->next->next->next->is (Activity::FEED));
          CHECK (anchor.next->next->next->next->next->next->next->is (Activity::WORKSTOP));
          CHECK (anchor.next->next->next->next->next->next->next->next == nullptr);
          
          // insert test-instrumentation to trace activation
          detector.watchGate (anchor.next->next, "theGate");
          detector.insertActivationTap(trigger.data_.notification.target, "trigger");
          detector.insertActivationTap(anchor.next->data_.notification.target, "deBlock");
          
          // rig the λ-post to forward dispatch as expected in real usage
          detector.executionCtx.post.implementedAs(
            [&](Time, Time, Activity* postedAct, auto& ctx)
               {
                 return ActivityLang::dispatchChain (postedAct, ctx);
               });
          
          ///// Case-1 : send a notification from prerequisite, but prior to activating primary-chain
          CHECK (activity::PASS == ActivityLang::dispatchChain (&trigger, detector.executionCtx));
          CHECK (detector.verifyInvocation("CTX-post").seq(0).arg("01.000","10.000","trigger","≺test::CTX≻") // notification is POSTed (with time and deadline from target)
                         .beforeInvocation("trigger") .seq(0).arg("5.555 --notify-↯> Act(GATE")              // notification dispatched to the Gate (note: now using curr-sched-time 5.555)
                                                             .arg("<2, until 0:00:10.000"));                 // Note: the Gate-latch expects 2 notifications
          CHECK (detector.ensureNoInvocation("testJob")                                                      // ==>   the latch was decremented but no invocation yet
                         .afterInvocation("trigger"));
          
          ///// Case-2 : now activate the primary-chain
          detector.incrementSeq();
          CHECK (activity::PASS == ActivityLang::dispatchChain (&anchor, detector.executionCtx));
          CHECK (detector.verifyInvocation("CTX-post").seq(1).arg("01.000","10.000","deBlock","≺test::CTX≻") // at begin, the internal self-notification is POSTed (with time and deadline this chain)
                         .beforeInvocation("deBlock") .seq(1).arg("5.555 --notify-↯> Act(GATE")              // and this call is immediately dispatched towards the Gate (using curr-sched-time 5.555)
                                                             .arg("<1, until 0:00:10.000")                   // Note: at this point, the Gate-latch expects 1 notifications ==> latch decremented, Gate OPENS
                         .beforeInvocation("after-theGate")  .arg("5.555 ⧐ Act(WORKSTART")                   // ...causing the activation to pass behind the Gate
                         .beforeInvocation("CTX-work").seq(1).arg("5.555","")                                // ...through WORKSTART
                         .beforeInvocation("testJob") .seq(1).arg("7.007",12345)                             // ...then invoke the JobFunctor itself (with the nominal Time{7,7})
                         .beforeInvocation("CTX-done").seq(1).arg("5.555","")                                // ...and finally the WORKSTOP
                         .beforeInvocation("theGate") .seq(1).arg("5.555 ⧐ Act(GATE")                        // RETURN to the primary-chain activation (after the internal self-notification)
                                                             .arg("<0, until -85401592:56:01.825"));         // -- however, after opening the Gate, the notification has blocked it permanently
          CHECK (detector.ensureNoInvocation("testJob")                                                      // ==>  thus no further (redundant) activation of the JobFunctor
                         .afterInvocation("CTX-done").seq(1));
          
          detector.incrementSeq();
          CHECK (activity::PASS == ActivityLang::dispatchChain (&trigger, detector.executionCtx));           // and any further external trigger is likewise blocked:
          CHECK (detector.verifyInvocation("CTX-post").seq(2).arg("01.000",Time::NEVER,"trigger","≺test::CTX≻")  // notification is POSTed (in the real Scheduler the deadline will block already here)
                         .beforeInvocation("trigger") .seq(2).arg("5.555 --notify-↯> Act(GATE")              // ... but even if it would reach the Gate...
                                                             .arg("<0, until -85401592:56:01.825"));         // ... the Gate has been closed permanently (by setting the deadline to Time::MIN)
          CHECK (detector.ensureNoInvocation("testJob")                                                      // ==>  no further invocation
                         .afterInvocation("trigger").seq(2));
          
//        cout << detector.showLog()<<endl; // HINT: use this for investigation...
        }
      
      
      
      /** @test usage scenario: Activity graph for an async Job
       *        - use a simple [calculation job term](\ref #scenario_RenderJob) as follow-up receiver
       *        - build an activity Term based on the »Async Load Job« wiring and link it to the receiver
       *        - also retrieve the Activity record used as re-entrance point after completing async IO
       */
      void
      scenario_IOJob()
        {
          Time nominal{7,7};
          Time start{0,1};
          Time dead{0,10};
          
          ActivityDetector detector;
          Job loadJob{detector.buildMockJob("loadJob", nominal, 12345)};
          Job calcJob{detector.buildMockJob("calcJob")};

          BlockFlowAlloc bFlow;
          ActivityLang activityLang{bFlow};
          
          auto followup = activityLang.buildCalculationJob (calcJob, start,dead);
          auto loadTerm = activityLang.buildAsyncLoadJob (loadJob, start,dead)
                                      .appendNotificationTo (followup);
          
          Activity& anchor = loadTerm.post();
          Activity& notify = loadTerm.callback();
          
          CHECK (anchor.is (Activity::POST));
          CHECK (anchor.next->is (Activity::WORKSTART));
          CHECK (anchor.next->next->is (Activity::INVOKE));
          CHECK (anchor.next->next->next->is (Activity::FEED));
          CHECK (anchor.next->next->next->next->is (Activity::FEED));
          CHECK (anchor.next->next->next->next->next == nullptr);                                            // Note: chain is severed here
          
          CHECK (notify.is (Activity::WORKSTOP));                                                            // ...WORKSTOP will be emitted from callback
          CHECK (notify.next->is (Activity::NOTIFY));                                                        // ...followed by notification of dependent job(s)
          CHECK (notify.next->next == nullptr);
          
          CHECK (notify.next->data_.notification.target == followup.post().next);                            // was wired to the GATE of the follow-up activity Term
          CHECK (followup.post().next->is (Activity::GATE));
          
          // rig the λ-post to forward dispatch as expected in real usage
          detector.executionCtx.post.implementedAs(
            [&](Time, Time, Activity* postedAct, auto& ctx)
               {
                 return ActivityLang::dispatchChain (postedAct, ctx);
               });
          
          
          ///// Case-1 : trigger off the async IO job
          CHECK (activity::PASS == ActivityLang::dispatchChain (&anchor, detector.executionCtx));
          CHECK (detector.verifyInvocation("CTX-work").seq(0).arg("5.555", "")                               // activation of WORKSTART
                         .beforeInvocation("loadJob") .seq(0).arg("7.007", 12345));                          // activation of JobFunctor
          CHECK (detector.ensureNoInvocation("CTX-done").seq(0)                                              // IO operation just runs, no further activity yet
                         .afterInvocation("loadJob").seq(0));
          
          
          ///// Case-2 : activate the rest of the chain after IO is done
          detector.incrementSeq();
          CHECK (activity::PASS == ActivityLang::dispatchChain (&notify, detector.executionCtx));
          CHECK (detector.verifyInvocation("CTX-done").seq(1).arg("5.555", "")                               // activation of WORKSTOP via callback
                         .beforeInvocation("CTX-post").seq(1).arg("01.00","10.00","GATE","≺test::CTX≻")      // the notification posts the GATE of the follow-up chain
                         .beforeInvocation("CTX-work").seq(1).arg("5.555", "")                               // GATE passes -> activation of the follow-up work commences
                         .beforeInvocation("calcJob") .seq(1)
                         .beforeInvocation("CTX-done").seq(1).arg("5.555", ""));
        }
      
      
      
      /** @test usage scenario: Activity graph for administrative job
       *        - by default, neither Gate, nor start/stop notification used
       *        - rather, the `INVOKE` and the argument-`FEED` is posted directly
       *  @remark the job itself is thus performed in »management mode«
       *        (holding the `GroomingToken`), and may modify the queue
       *        to generate new jobs.
       */
      void
      scenario_MetaJob()
        {
          Time nominal{7,7};
          Time start{0,1};
          Time dead{0,10};
          
          ActivityDetector detector;
          Job testJob{detector.buildMockJob("metaJob", nominal, 12345)};
          
          BlockFlowAlloc bFlow;
          ActivityLang activityLang{bFlow};
          
          // Build Activity-Term with a chain defining a meta-job...
          Activity& anchor = activityLang.buildMetaJob (testJob, start,dead)
                                         .post();
          
          CHECK (anchor.is (Activity::POST));
          CHECK (anchor.next->is (Activity::INVOKE));
          CHECK (anchor.next->next->is (Activity::FEED));
          CHECK (anchor.next->next->next->is (Activity::FEED));
          CHECK (anchor.next->next->next->next == nullptr);
          
          // insert test-instrumentation
          detector.insertActivationTap(anchor.next);
          
          CHECK (activity::PASS == ActivityLang::dispatchChain (&anchor, detector.executionCtx));

          CHECK (detector.verifyInvocation("tap-INVOKE").arg("5.555 ⧐ Act(INVOKE")
                         .beforeInvocation("metaJob") .arg("7.007",12345));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SchedulerActivity_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
