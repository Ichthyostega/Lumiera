/*
  SchedulerCommutator(Test)  -  verify dependent activity processing in the scheduler

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

/** @file scheduler-commutator-test.cpp
 ** unit test \ref SchedulerCommutator_test
 */


#include "lib/test/run.hpp"
#include "activity-detector.hpp"
#include "vault/gear/scheduler-commutator.hpp"
#include "lib/test/microbenchmark.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"
#include "lib/thread.hpp"
#include "lib/util.hpp"

#include <chrono>

using test::Test;
using lib::test::threadBenchmark;


namespace vault{
namespace gear {
namespace test {
  
  using lib::time::Time;
  using lib::time::FSecs;
  using std::atomic_bool;
  using lib::ThreadHookable;
  using lib::thread::ThreadWrapper;
  using util::isSameObject;
  
  using std::unique_ptr;
  using std::make_unique;
  using std::this_thread::yield;
  using std::this_thread::sleep_for;
  using std::chrono_literals::operator ""us;
  
  
  namespace { // Load test parameters
    const size_t NUM_THREADS = 20;  ///< @see #torture_GroomingToken()
    const size_t REPETITIONS = 100;
  }
  
  
  
  
  
  /******************************************************************//**
   * @test Scheduler Layer-2: coordination of Activity execution.
   * @remark Layer-2 combines the queue data structure from Layer-1 with the
   *       »Activity Language« to allow _performing_ of Render Activities.
   *       This test verifies the proper integration of these building blocks
   *       - the _Grooming-Token_ is an atomic lock tied to current thread-id;
   *         it will be acquired for all operations manipulating internal state
   *       - the \ref ActivityDetector is used as a test helper to record calls
   *         and to verify the Activities are indeed activated as expected
   *       - the #integratedWorkCycle() walks through all the steps typically
   *         happening when a Render-Job is first planned and scheduled, and
   *         then retrieved and executed by the \ref WorkForce. However, these
   *         steps are invoked directly here, and with suitable instrumentation
   *         to watch processing in detail
   *       - the complete Scheduler functionality is assembled one level higher
   *         in the [Scheduler-Service](\ref scheduler.hpp)...
   * @see SchedulerActivity_test
   * @see ActivityDetector_test
   * @see SchedulerUsage_test
   */
  class SchedulerCommutator_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          demonstrateSimpleUsage();
          verify_GroomingToken();
          verify_GroomingGuard();
          torture_GroomingToken();
          verify_DispatchDecision();
          verify_findWork();
          verify_Significance();
          verify_postDispatch();
          integratedWorkCycle();
        }
      
      
      /** @test demonstrate a simple usage scenario
       */
      void
      demonstrateSimpleUsage()
        {
          SchedulerInvocation queue;
          SchedulerCommutator sched;
          Activity activity;
          Time when{3,4};
          Time dead{5,6};
          
          // use the ActivityDetector for test instrumentation...
          ActivityDetector detector;
          Time now = detector.executionCtx.getSchedTime();
          CHECK (now < dead);
          
          // prepare scenario: some activity is enqueued
          queue.instruct ({activity, when, dead});
          
          sched.postDispatch (sched.findWork(queue,now), detector.executionCtx,queue);
          CHECK (detector.verifyInvocation("CTX-tick").arg(now));
          CHECK (queue.empty());
          
//        cout << detector.showLog()<<endl; // HINT: use this for investigation...
        }
      
      
      
      /** @test verify logic to control concurrent execution
       */
      void
      verify_GroomingToken()
        {
          SchedulerCommutator sched;
          
          auto myself = std::this_thread::get_id();
          CHECK (not sched.holdsGroomingToken (myself));
          
          CHECK (sched.acquireGoomingToken());
          CHECK (    sched.holdsGroomingToken (myself));
          
          sched.dropGroomingToken();
          CHECK (not sched.holdsGroomingToken (myself));
          ___ensureGroomingTokenReleased(sched);
        }
      
      /** @internal helper to ensure consistent Grooming-Token state */
      static void
      ___ensureGroomingTokenReleased (SchedulerCommutator& sched)
        {
          auto myself = std::this_thread::get_id();
          CHECK (not sched.holdsGroomingToken(myself));
          CHECK (sched.acquireGoomingToken());
          sched.dropGroomingToken();
        }
      
      
      
      /** @test verify extended logic to protect a scope
       *      - if the thread already holds the grooming token, nothing happens
       *      - otherwise, it is acquired (blocking) and dropped on exit
       */
      void
      verify_GroomingGuard()
        {
          SchedulerCommutator sched;
          
          // Case-1: if a thread already holds the token....
          CHECK (sched.acquireGoomingToken());
          CHECK (sched.holdsGroomingToken (thisThread()));
          {
            auto guard = sched.requireGroomingTokenHere();
            CHECK (sched.holdsGroomingToken (thisThread()));
          }// leave scope -> nothing happens in this case
          CHECK (sched.holdsGroomingToken (thisThread()));
          
          // Case-2: when not holding the token...
          sched.dropGroomingToken();
          {
              // acquire automatically (this may block)
            auto guard = sched.requireGroomingTokenHere();
            CHECK (sched.holdsGroomingToken (thisThread()));
          }// leave scope -> dropped automatically
          CHECK (not sched.holdsGroomingToken (thisThread()));
          
          ___ensureGroomingTokenReleased(sched);
        }
      
      
      
      /** @test ensure the GroomingToken mechanism indeed creates mutual
       *        exclusion to protected against concurrent corruption.
       * @remark uses lib::test::threadBenchmark() to put the test-subject
       *        under pressure by strongly contended parallel execution.
       */
      void
      torture_GroomingToken()
        {
          SchedulerCommutator sched;
          
          size_t checkSum{0};
          auto pause_and_sum = [&](size_t i) -> size_t
                                  {
                                    auto oldSum = checkSum;
                                    sleep_for (500us);
                                    checkSum = oldSum + i;
                                    return 1;
                                  };
          auto protected_sum = [&](size_t i) -> size_t
                                  {
                                    while (not sched.acquireGoomingToken())
                                      yield();    // contend until getting exclusive access
                                    pause_and_sum(i);
                                    sched.dropGroomingToken();
                                    return 1;
                                  };
          
          threadBenchmark<NUM_THREADS> (pause_and_sum, REPETITIONS);
          
          size_t brokenSum = checkSum;
          checkSum = 0;
          
          threadBenchmark<NUM_THREADS> (protected_sum, REPETITIONS);
          
          CHECK (brokenSum < checkSum);
          CHECK (checkSum = NUM_THREADS * REPETITIONS*(REPETITIONS-1)/2);
          ___ensureGroomingTokenReleased(sched);
        }
      
      
      
      atomic_bool stopTheHog_{false};
      unique_ptr<ThreadHookable> groomingHog_;
      using Launch = ThreadHookable::Launch;
      
      /** @internal Helper to block the GroomingToken from another thread */
      void
      blockGroomingToken (SchedulerCommutator& sched)
        {
          REQUIRE (not groomingHog_);
          if (sched.holdsGroomingToken(std::this_thread::get_id()))
              sched.dropGroomingToken();
          
          stopTheHog_ = false;
          groomingHog_ = make_unique<ThreadHookable>(
                           Launch{[&]{
                                       CHECK (sched.acquireGoomingToken());
                                       do sleep_for (100us);
                                       while (not stopTheHog_);
                                       sched.dropGroomingToken();
                                     }}
                                 .atExit([&](ThreadWrapper& handle)
                                     {
                                       handle.detach_thread_from_wrapper();
                                       groomingHog_.reset();
                                     })
                                 .threadID("grooming-hog"));
          sleep_for (500us);
          ENSURE (groomingHog_);
        }
      
      /** @internal stop the background thread to unblock the GrooingToken */
      void
      unblockGroomingToken()
        {
          stopTheHog_ = true;
          while (groomingHog_)
            yield();
        }
      
      
      
      
      
      /** @test verify the logic to decide where and when to perform
       *        the dispatch of a Scheduler Activity chain.
       */
      void
      verify_DispatchDecision()
        {
          SchedulerCommutator sched;
          ___ensureGroomingTokenReleased(sched);
          
          Time t1{10,0};
          Time t2{20,0};
          Time t3{30,0};
          Time now{t2};
          
          auto myself = std::this_thread::get_id();
          CHECK (sched.decideDispatchNow (t1, now));               // time is before now => good to execute
          CHECK (sched.holdsGroomingToken (myself));               // Side-Effect: acquired the Grooming-Token
          
          CHECK (sched.decideDispatchNow (t1, now));               // also works if Grooming-Token is already acquired
          CHECK (sched.holdsGroomingToken (myself));
          
          CHECK (sched.decideDispatchNow (t2, now));               // Boundary case time == now  => good to execute
          CHECK (sched.holdsGroomingToken (myself));
          
          CHECK (not sched.decideDispatchNow (t3, now));           // Task in the future shall not be dispatched now
          CHECK (sched.holdsGroomingToken (myself));               // ...and this case has no impact on the Grooming-Token
          sched.dropGroomingToken();
          
          CHECK (not sched.decideDispatchNow (t3, now));
          CHECK (not sched.holdsGroomingToken (myself));
          
          blockGroomingToken(sched);
          CHECK (not sched.acquireGoomingToken());
          
          CHECK (not sched.decideDispatchNow (t1, now));           // unable to acquire => can not decide positively
          CHECK (not sched.holdsGroomingToken (myself));
          
          CHECK (not sched.decideDispatchNow (t2, now));
          CHECK (not sched.holdsGroomingToken (myself));
          
          unblockGroomingToken();
          
          CHECK (sched.decideDispatchNow (t2, now));
          CHECK (sched.holdsGroomingToken (myself));
        }
      
      
      
      /** @test verify logic of queue updates and work prioritisation.
       */
      void
      verify_findWork()
        {
          SchedulerInvocation queue;
          SchedulerCommutator sched;
          
          Time t1{10,0};
          Time t2{20,0};
          Time t3{30,0};
          Time now{t2};

          CHECK (not sched.findWork (queue, now));                 // empty queue, no work found
          
          Activity a1{1u,1u};
          Activity a2{2u,2u};
          Activity a3{3u,3u};
          
          queue.instruct ({a3, t3});                               // activity scheduled into the future
          CHECK (not sched.findWork (queue, now));                 // ... not found with time `now`
          CHECK (t3 == queue.headTime());
          
          queue.instruct ({a1, t1});
          CHECK (isSameObject (a1, *sched.findWork(queue, now)));  // but past activity is found
          CHECK (not sched.findWork (queue, now));                 // activity was retrieved
          
          queue.instruct ({a2, t2});
          CHECK (isSameObject (a2, *sched.findWork(queue, now)));  // activity scheduled for `now` is found
          CHECK (not sched.findWork (queue, now));                 // nothing more found for `now`
          CHECK (t3 == queue.headTime());
          CHECK (not queue.empty());                               // yet the future activity a3 is still queued...
          
          CHECK (isSameObject (a3, *sched.findWork(queue, t3)));   // ...and will be found when querying "later"
          CHECK (not sched.findWork (queue, t3));
          CHECK (    queue.empty());                               // Everything retrieved and queue really empty
          
          queue.instruct ({a2, t2});
          queue.instruct ({a1, t1});
          CHECK (isSameObject (a1, *sched.findWork(queue, now)));  // the earlier activity is found first
          CHECK (t2 == queue.headTime());
          CHECK (isSameObject (a2, *sched.findWork(queue, now)));
          CHECK (not sched.findWork (queue, now));
          CHECK (    queue.empty());
          
          queue.instruct ({a2, t2});                               // prepare activity which /would/ be found...
          blockGroomingToken(sched);                               // but prevent this thread from acquiring the GroomingToken
          CHECK (not sched.findWork (queue, now));                 // thus search aborts immediately
          CHECK (not queue.empty());
          
          unblockGroomingToken();                                  // yet when we're able to get the GroomingToken
          CHECK (isSameObject (a2, *sched.findWork(queue, now)));  // the task can be retrieved
          CHECK (queue.empty());
        }
      
      
      
      /** @test verify that obsoleted or rejected entries are dropped transparently
       *      - add entries providing extra information regarding significance
       *      - verify that missing the deadline is detected
       *      - entries past deadline will be dropped when pulling for work
       *      - entries tagged with an ManifestationID can be disabled and
       *        will be automatically disposed.
       *      - an entry marked as _compulsory_ will block that process
       *        when missing it's deadline
       */
      void
      verify_Significance()
        {
          SchedulerInvocation queue;
          SchedulerCommutator sched;
          
          Time t1{10,0};   Activity a1{1u,1u};
          Time t2{20,0};   Activity a2{2u,2u};
          Time t3{30,0};   Activity a3{3u,3u};
          Time t4{40,0};   Activity a4{4u,4u};
                     //     start,deadline, manif.ID,       isCompulsory
          queue.instruct ({a1, t1, t4, ManifestationID{5}});
          queue.instruct ({a2, t2, t2});
          queue.instruct ({a3, t3, t3, ManifestationID{23}, true});
          queue.instruct ({a4, t4, t4});
          queue.activate(ManifestationID{5});
          queue.activate(ManifestationID{23});
          
          queue.feedPrioritisation();
          CHECK (t1 == queue.headTime());
          CHECK (isSameObject (a1, *queue.peekHead()));
          CHECK (not queue.isMissed(t1));
          CHECK (not queue.isOutdated(t1));
          
          queue.drop(ManifestationID{5});
          CHECK (t1 == queue.headTime());
          CHECK (not queue.isMissed(t1));
          CHECK (    queue.isOutdated(t1));
          
          CHECK (not sched.findWork(queue, t1));
          CHECK (t2 == queue.headTime());
          CHECK (isSameObject (a2, *queue.peekHead()));
          CHECK (not queue.isMissed  (t2));
          CHECK (not queue.isOutdated(t2));
          CHECK (    queue.isMissed  (t3));
          CHECK (    queue.isOutdated(t3));
          
          CHECK (not sched.findWork(queue, t2+Time{5,0}));
          CHECK (t3 == queue.headTime());
          CHECK (isSameObject (a3, *queue.peekHead()));
          CHECK (not queue.isMissed   (t3));
          CHECK (not queue.isOutdated (t3));
          CHECK (not queue.isOutOfTime(t3));
          CHECK (    queue.isMissed   (t4));
          CHECK (    queue.isOutdated (t4));
          CHECK (    queue.isOutOfTime(t4));
          
          CHECK (not sched.findWork(queue, t4));
          CHECK (t3 == queue.headTime());
          CHECK (not queue.isMissed   (t3));
          CHECK (not queue.isOutdated (t3));
          CHECK (not queue.isOutOfTime(t3));
          CHECK (    queue.isMissed   (t4));
          CHECK (    queue.isOutdated (t4));
          CHECK (    queue.isOutOfTime(t4));
          
          queue.drop(ManifestationID{23});
          CHECK (t3 == queue.headTime());
          CHECK (not queue.isMissed   (t3));
          CHECK (    queue.isOutdated (t3));
          CHECK (not queue.isOutOfTime(t3));
          CHECK (    queue.isMissed   (t4));
          CHECK (    queue.isOutdated (t4));
          CHECK (not queue.isOutOfTime(t4));
          
          CHECK (isSameObject (a3, *queue.peekHead()));
          CHECK (isSameObject (a4, *sched.findWork(queue, t4)));
          CHECK (queue.empty());
        }
      
      
      
      /** @test verify entrance point for performing an Activity chain.
       */
      void
      verify_postDispatch()
        {
          // rigged execution environment to detect activations--------------
          ActivityDetector detector;
          Activity& activity = detector.buildActivationProbe ("testActivity");
          auto makeEvent = [&](Time start) { return ActivationEvent{activity, start, start+Time{0,1}}; };
                                                                                  // set a dummy deadline to pass the sanity check
          SchedulerInvocation queue;
          SchedulerCommutator sched;
          
          Time now = detector.executionCtx.getSchedTime();
          Time past {Time::ZERO};
          Time future{now+now};
          
              // no one holds the GroomingToken
          ___ensureGroomingTokenReleased(sched);
          auto myself = std::this_thread::get_id();
          CHECK (not sched.holdsGroomingToken (myself));
          
          // no effect when empty / no Activity given (usually this can happen due to lock contention)
          CHECK (activity::KICK == sched.postDispatch (ActivationEvent(), detector.executionCtx, queue));
          CHECK (not sched.holdsGroomingToken (myself));
          
          // Activity immediately dispatched when on time and GroomingToken can be acquired
          CHECK (activity::PASS == sched.postDispatch (makeEvent(past), detector.executionCtx, queue));
          CHECK (detector.verifyInvocation("testActivity").timeArg(now)); // was invoked immediately
          CHECK (    sched.holdsGroomingToken (myself));
          CHECK (    queue.empty());
          detector.incrementSeq(); // Seq-point-1 in the detector log
          
          // future Activity is enqueued by short-circuit directly into the PriorityQueue if possible
          CHECK (activity::PASS == sched.postDispatch (makeEvent(future), detector.executionCtx, queue));
          CHECK (    sched.holdsGroomingToken (myself));
          CHECK (not queue.empty());
          CHECK (isSameObject (activity, *queue.peekHead())); //  appears at Head, implying it's in Priority-Queue
          
          queue.pullHead();
          sched.dropGroomingToken();
          CHECK (not sched.holdsGroomingToken (myself));
          CHECK (queue.empty());
          
          // ...but GroomingToken is not acquired explicitly; Activity is just placed into the Instruct-Queue
          CHECK (activity::PASS == sched.postDispatch (makeEvent(future), detector.executionCtx, queue));
          CHECK (not sched.holdsGroomingToken (myself));
          CHECK (not queue.peekHead());           // not appearing at Head this time,
          CHECK (not queue.empty());             //  rather waiting in the Instruct-Queue
          
          
          blockGroomingToken(sched);
          CHECK (activity::PASS == sched.postDispatch (makeEvent(now), detector.executionCtx, queue));
          CHECK (not sched.holdsGroomingToken (myself));
          CHECK (not queue.peekHead());     // was enqueued, not executed
          
          // Note: this test achieved one single direct invocation;
          // all further cases after Seq-point-1 were queued only
          CHECK (detector.ensureNoInvocation("testActivity")
                         .afterSeqIncrement(1));
          
          // As sanity-check: after the point where we purged the queue,
          // two further cases where enqueued; we could retrieve them if
          // re-acquiring the GroomingToken and using suitable query-time
          unblockGroomingToken();
          queue.feedPrioritisation();
          CHECK (now == queue.headTime());
          CHECK (isSameObject (activity, *sched.findWork(queue, now)));
          CHECK (sched.holdsGroomingToken (myself));  // findWork() acquired the token
          CHECK (future == queue.headTime());
          CHECK (not queue.isDue(now));
          CHECK (    queue.isDue(future));
          CHECK (sched.findWork(queue, future));
          CHECK (    queue.empty());
        }
      
      
      
      /** @test step-wise perform the typical sequence of planning and worker activation
       *        - use the Render-Job scenario from SchedulerActivity_test::scenario_RenderJob()
       *        - use similar instrumentation to trace Activities
       *        - specifically rig the diagnostic executionCtx to drop the GroomingToken at λ-work
       *        - Step-1 : schedule the Activity-term
       *        - Step-2 : later search for work, retrieve and dispatch the term
       *        - verify the expected sequence of Activities actually occurred
       * @see ActivityLang::buildCalculationJob()
       * @see ActivityDetector::buildMockJob()
       */
      void
      integratedWorkCycle()
        { //   ·==================================================================== setup a rigged Job
          Time nominal{7,7};
          Time start{0,1};
          Time dead{0,10};
          
          ActivityDetector detector;
          Job testJob{detector.buildMockJob("testJob", nominal, 12345)};
          
          BlockFlowAlloc bFlow;
          ActivityLang activityLang{bFlow};
          
          // Build the Activity-Term for a simple calculation job...
          Activity& anchor = activityLang.buildCalculationJob (testJob, start,dead)
                                         .post(); // retrieve the entrance point to the chain
          
          // insert instrumentation to trace activation
          detector.watchGate (anchor.next, "theGate");

          
          //    ·=================================================================== setup test subject
          SchedulerInvocation queue;
          SchedulerCommutator sched;
          
              // no one holds the GroomingToken
          ___ensureGroomingTokenReleased(sched);
          auto myself = std::this_thread::get_id();
          CHECK (not sched.holdsGroomingToken (myself));
          
          TimeVar now{Time::ZERO};
          
          // rig the ExecutionCtx to allow manipulating "current scheduler time"
          detector.executionCtx.getSchedTime = [&]{ return Time{now}; };
          // rig the λ-work to verify GroomingToken and to drop it then
          detector.executionCtx.work.implementedAs(
            [&](Time, size_t)
               {
                 CHECK (sched.holdsGroomingToken (myself));
                 sched.dropGroomingToken();
               });
          
          
          //    ·=================================================================== actual test sequence
          // Add the Activity-Term to be scheduled for planned start-Time
          sched.postDispatch (ActivationEvent{anchor, start}, detector.executionCtx, queue);
          CHECK (detector.ensureNoInvocation("testJob"));
          CHECK (not sched.holdsGroomingToken (myself));
          CHECK (not queue.empty());
          
          // later->"now"
          now = Time{555,5};
          detector.incrementSeq();
          
          // Assuming a worker runs "later" and retrieves work...
          ActivationEvent act = sched.findWork(queue,now);
          CHECK (sched.holdsGroomingToken (myself));       // acquired the GroomingToken
          CHECK (isSameObject(*act, anchor));              // "found" the rigged Activity as next piece of work
          
          sched.postDispatch (act, detector.executionCtx, queue);
          
          CHECK (queue.empty());
          CHECK (not sched.holdsGroomingToken (myself));   // the λ-work was invoked and dropped the GroomingToken
          
          CHECK (detector.verifySeqIncrement(1)
                         .beforeInvocation("theGate").arg("5.555 ⧐ Act(GATE")
                         .beforeInvocation("after-theGate").arg("⧐ Act(WORKSTART")
                         .beforeInvocation("CTX-work").arg("5.555","")
                         .beforeInvocation("testJob") .arg("7.007",12345)
                         .beforeInvocation("CTX-done").arg("5.555",""));
          
//        cout << detector.showLog()<<endl; // HINT: use this for investigation...
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SchedulerCommutator_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
