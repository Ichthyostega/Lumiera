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
#include "lib/test/diagnostic-output.hpp"///////////////////////////TODO TOD-Oh

//#include <utility>
#include <chrono>

using test::Test;
using lib::test::threadBenchmark;
//using std::move;
using util::isSameObject;


namespace vault{
namespace gear {
namespace test {
  
//  using lib::time::FrameRate;
//  using lib::time::Offset;
  using lib::time::Time;
  using std::atomic_bool;
  using lib::ThreadHookable;
  using std::unique_ptr;
  using std::make_unique;
  using std::this_thread::yield;
  using std::this_thread::sleep_for;
  using std::chrono_literals::operator ""us;
  
  namespace {// Test parameters
    const size_t NUM_THREADS = 20;
    const size_t REPETITIONS = 100;
    
    inline void
    ___ensureGroomingTokenReleased(SchedulerCommutator& sched)
    {
      auto myself = std::this_thread::get_id();
      CHECK (not sched.holdsGroomingToken(myself));
      CHECK (sched.acquireGoomingToken());
      sched.dropGroomingToken();
    }
  }
  
  
  
  
  
  /******************************************************************//**
   * @test Scheduler Layer-2: dependency notification and triggering.
   * @see SchedulerActivity_test
   * @see SchedulerUsage_test
   */
  class SchedulerCommutator_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          demonstrateSimpleUsage();
          verify_GroomingToken();
          torture_GroomingToken();
          verify_DispatchDecision();
          verify_findWork();
          verify_postDispatch();
          integratedWorkCycle();
        }
      
      
      /** @test TODO demonstrate a simple usage scenario
       * @todo WIP 10/23 ✔ define ⟶ 🔁 implement
       */
      void
      demonstrateSimpleUsage()
        {
          SchedulerInvocation queues;
          SchedulerCommutator sched;
          Activity activity;
          Time when{1,2,3};
          
          // prepare scenario: some activity is enqueued
          queues.instruct (activity, when);
          
          // use the ActivityDetector for test instrumentation...
          ActivityDetector detector;
          
//        sched.postDispatch (sched.findWork(queues), detector.executionCtx);  ///////////////////////OOO findWork umschreiben
          cout << detector.showLog()<<endl; // HINT: use this for investigation...
        }
      
      
      
      /** @test verify logic to control concurrent execution
       * @todo WIP 10/23 ✔ define ⟶ ✔ implement
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
      
      
      
      /** @test ensure the GroomingToken mechanism indeed creates an
       *   exclusive section protected against concurrent corruption.
       * @todo WIP 10/23 ✔ define ⟶ ✔ implement
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
                                      yield();
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
      
      /** @internal Helper to block the GroomingToken in another thread */
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
                                 .atExit([&]{ groomingHog_.reset(); })
                                 .threadID("grooming-hog"));
          sleep_for (500us);
        }
      
      /** @internal stop the background thread to unblock the GrooingToken */
      void
      unblockGroomingToken()
        {
          stopTheHog_ = true;
          while (groomingHog_)
            yield();
        }
      
      
      
      /** @test TODO verify the decision logic where and when
       *        to perform the dispatch of an Scheduler Activity chain.
       * @todo WIP 10/23 ✔ define ⟶ ✔ implement
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
          CHECK (sched.decideDispatchNow (t1, now));
          CHECK (sched.holdsGroomingToken (myself));
          
          CHECK (sched.decideDispatchNow (t1, now));
          CHECK (sched.holdsGroomingToken (myself));
          
          CHECK (sched.decideDispatchNow (t2, now));
          CHECK (sched.holdsGroomingToken (myself));
          
          CHECK (not sched.decideDispatchNow (t3, now));
          CHECK (sched.holdsGroomingToken (myself));
          sched.dropGroomingToken();
          
          CHECK (not sched.decideDispatchNow (t3, now));
          CHECK (not sched.holdsGroomingToken (myself));
          
          blockGroomingToken(sched);
          CHECK (not sched.acquireGoomingToken());
          
          CHECK (not sched.decideDispatchNow (t1, now));
          CHECK (not sched.holdsGroomingToken (myself));
          
          CHECK (not sched.decideDispatchNow (t2, now));
          CHECK (not sched.holdsGroomingToken (myself));
          
          unblockGroomingToken();
          
          CHECK (sched.decideDispatchNow (t2, now));
          CHECK (sched.holdsGroomingToken (myself));
        }
      
      
      
      /** @test TODO verify logic of queue updates and work prioritisation.
       * @todo WIP 10/23 🔁 define ⟶ implement
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
          
          queue.instruct (a3, t3);                                 // activity scheduled into the future
          CHECK (not sched.findWork (queue, now));                 // for time `now` not found
          
          queue.instruct (a1, t1);
          CHECK (isSameObject (a1, *sched.findWork(queue, now)));  // but past activity is found
          CHECK (not sched.findWork (queue, now));                 // activity was retrieved
          
          queue.instruct (a2, t2);
          CHECK (isSameObject (a2, *sched.findWork(queue, now)));  // activity scheduled for `now` is found
          CHECK (not sched.findWork (queue, now));
          CHECK (not queue.empty());                               // yet the future activity a3 is still queued...
          
          CHECK (isSameObject (a3, *sched.findWork(queue, t3)));   // ...and will be found when querying "later"
          CHECK (not sched.findWork (queue, t3));
          CHECK (    queue.empty());                               // Everything retrieved and queue really empty
          
          queue.instruct (a2, t2);
          queue.instruct (a1, t1);
          CHECK (isSameObject (a1, *sched.findWork(queue, now)));  // the earlier activity is found first
          CHECK (isSameObject (a2, *sched.findWork(queue, now)));
          CHECK (not sched.findWork (queue, now));
          CHECK (    queue.empty());
          
          queue.instruct (a2, t2);                                 // prepare activity which /would/ be found...
          blockGroomingToken(sched);                               // but prevent this thread from acquiring the GroomingToken
          CHECK (not sched.findWork (queue, now));                 // thus search aborts out immediately
          CHECK (not queue.empty());
          
          unblockGroomingToken();                                  // yet when we're able to get the GroomingToken
          CHECK (isSameObject (a2, *sched.findWork(queue, now)));  // the task can be retrieved
          CHECK (queue.empty());
        }
      
      
      
      /** @test TODO verify entrance point for performing an Activity chain.
       * @todo WIP 10/23 🔁 define ⟶ implement
       */
      void
      verify_postDispatch()
        {
          UNIMPLEMENTED ("postDispatch");
        }
      
      
      
      /** @test TODO build the integrated sequence of worker activation
       * @todo WIP 10/23 🔁 define ⟶ implement
       */
      void
      integratedWorkCycle()
        {
          UNIMPLEMENTED ("integratedWorkCycle");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SchedulerCommutator_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
