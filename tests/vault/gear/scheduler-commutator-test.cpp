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
//#include "lib/util.hpp"
#include "lib/test/diagnostic-output.hpp"///////////////////////////TODO TOD-Oh

//#include <utility>
#include <thread>
#include <chrono>

using test::Test;
using lib::test::threadBenchmark;
//using std::move;
//using util::isSameObject;


namespace vault{
namespace gear {
namespace test {
  
//  using lib::time::FrameRate;
//  using lib::time::Offset;
  using lib::time::Time;
  using std::this_thread::yield;
  using std::this_thread::sleep_for;
  using std::chrono_literals::operator ""us;
  
  namespace {// Test parameters
    const size_t NUM_THREADS = 20;
    const size_t REPETITIONS = 100;
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
        }
      
      
      
      /** @test TODO verify the decision logic where and when
       *        to perform the dispatch of an Scheduler Activity chain.
       * @todo WIP 10/23 🔁 define ⟶ implement
       */
      void
      verify_DispatchDecision()
        {
          UNIMPLEMENTED ("DispatchDecision");
        }
      
      
      
      /** @test TODO verify logic of queue updates and work prioritisation.
       * @todo WIP 10/23 🔁 define ⟶ implement
       */
      void
      verify_findWork()
        {
          UNIMPLEMENTED ("findWork");
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
