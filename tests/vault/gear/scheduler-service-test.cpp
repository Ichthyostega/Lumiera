/*
  SchedulerService(Test)  -  component integration test for the scheduler

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

/** @file scheduler-usage-test.cpp
 ** unit test \ref SchedulerService_test
 */


#include "lib/test/run.hpp"
#include "activity-detector.hpp"
#include "vault/gear/scheduler.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"
#include "lib/test/microbenchmark.hpp"
#include "lib/test/diagnostic-output.hpp"///////////////TODO
//#include "lib/util.hpp"

//#include <utility>
#include <thread>

using test::Test;
//using std::move;
//using util::isSameObject;


namespace vault{
namespace gear {
namespace test {
  
//  using lib::time::FrameRate;
//  using lib::time::Offset;
  using lib::time::Time;
  using std::this_thread::sleep_for;
  
  namespace {                            ////////////////////////////////////////////////////////////////////TICKET #1055 want to construct lumiera Time from std::chrono literals
    Time t100us = Time{FSecs{1, 10'000}};
    Time t200us = t100us + t100us;
    Time t500us = t200us + t200us + t100us;
    Time t1ms   = Time{1,0};
  }
  
  
  
  
  /*************************************************************************//**
   * @test Scheduler component integration test: add and process dependent jobs.
   * @see SchedulerActivity_test
   * @see SchedulerInvocation_test
   * @see SchedulerCommutator_test
   * @see SchedulerLoadControl_test
   */
  class SchedulerService_test : public Test
    {
      
      virtual void
      run (Arg)
        {
           simpleUsage();
           invokeWorkFunction();
           walkingDeadline();
        }
      
      
      /** @test TODO demonstrate a simple usage scenario
       * @todo WIP 10/23 ✔ define ⟶ 🔁 implement
       */
      void
      simpleUsage()
        {
          BlockFlowAlloc bFlow;
          EngineObserver watch;
          Scheduler{bFlow, watch};
        }
      
      
      
      /** @test verify visible behaviour of the [work-pulling function](\ref Scheduler::getWork)
       *      - use a rigged Activity probe to capture the schedule time on invocation
       *      - additionally perform a timing measurement for invoking the work-function
       *      - empty invocations cost ~5µs (-O3) rsp. ~25µs (debug)
       *      - this implies we can show timing-delay effects in the millisecond range
       *      - demonstrated behaviour
       *        + an Activity already due will be dispatched immediately by post()
       * @note Invoke the Activity probe itself can take 50..150µs, due to the EventLog,
       *       which is not meant to be used in performance critical paths but only for tests,
       *       because it performs lots of heap allocations and string operations. Moreover,
       *       we see additional cache effects after an extended sleep period.
       * @todo WIP 10/23 🔁 define ⟶ implement
       */
      void
      invokeWorkFunction()
        {
          BlockFlowAlloc bFlow;
          EngineObserver watch;
          Scheduler scheduler{bFlow, watch};
          
          ActivityDetector detector;
          Activity& probe = detector.buildActivationProbe ("testProbe");
          
          TimeVar start;
          int64_t delay_us;
          int64_t slip_us;
          activity::Proc res;

          auto post = [&](Time start)
                              { // this test class is declared friend to get a backdoor to Scheduler internals...
                                auto& schedCtx = Scheduler::ExecutionCtx::from(scheduler);
                                
                                schedCtx.post (start, &probe, schedCtx);
                              };
          
          auto pullWork = [&] {
                                uint REPETITIONS = 1;
                                delay_us = lib::test::benchmarkTime([&]{ res = scheduler.getWork(); }, REPETITIONS);
                                slip_us = _raw(detector.invokeTime(probe)) - _raw(start);
                                cout << "res:"<<res<<" delay="<<delay_us<<"µs slip="<<slip_us<<"µs"<<endl;
                              };
          

          auto wasClose = [](TimeValue a, TimeValue b)
                              {                                // 500µs are considered "close"
                                return Duration{Offset{a,b}} < Duration{FSecs{1,2000}};
                              };
          auto wasInvoked = [&](Time start)
                              {
                                Time invoked = detector.invokeTime (probe);
                                return invoked >= start
                                   and wasClose (invoked, start);
                              };
          
          
          cout << "Scheduled right away..."<<endl;
          start = RealClock::now();
          post(start);                                                      // Post the testProbe to be scheduled "now"
          CHECK (wasInvoked(start));                                        // Result: invoked directly, not enqueued at all
          CHECK (scheduler.empty());
          
          
          cout << "pullWork() on empty queue..."<<endl;
          pullWork();                                                       // Call the work-Function on empty Scheduler queue
          CHECK (activity::WAIT == res);                                    // the result instructs this thread to go to sleep immediately
          
          
          cout << "Due at pullWork()..."<<endl;
          TimeVar now = RealClock::now();
          start = now + t100us;                                             // Set a schedule 100ms ahead of "now"
          post (start);
          CHECK (not scheduler.empty());                                    // was enqueued
          CHECK (not wasInvoked(start));                                    // ...but not activated yet
          
          sleep_for (100us);                                                // wait beyond the planned start point (typically waits ~150µs or more)
          pullWork();
          CHECK (wasInvoked(start));
          CHECK (slip_us  < 300);                                           // Note: typically there is a slip of 100..200µs, because sleep waits longer
          CHECK (scheduler.empty());                                        // The scheduler is empty now and this thread will go to sleep,
          CHECK (delay_us < 20000);                                         // however the sleep-cycle is first re-shuffled by a wait between 0 ... 20ms
          CHECK (activity::PASS == res);                                    // this thread is instructed to check back once
          pullWork();
          CHECK (activity::WAIT == res);                                    // ...yet since the queue is still empty, it is sent immediately to sleep
          CHECK (delay_us < 20);
          
          
          cout << "next some time ahead => up-front delay"<<endl;
          now = RealClock::now();
          start = now + t500us;                                             // Set a schedule significantly into the future...
          post (start);
          CHECK (not scheduler.empty());
          
          pullWork();                                                       // ...and invoke the work-Function immediately "now"
          CHECK (activity::PASS == res);                                    // Result: this thread was kept in sleep in the work-Function
          CHECK (not wasInvoked(start));                                    // but the next dispatch did not happen yet; we are instructed to re-invoke immediately
          CHECK (delay_us > 500);                                           // this proves that there was a delay to wait for the next schedule
          CHECK (delay_us < 1000);
          pullWork();                                                       // if we now re-invoke the work-Function as instructed...
          CHECK (wasInvoked(start));                                        // then the next schedule is already slightly overdue and immediately invoked
          CHECK (scheduler.empty());                                        // the queue is empty and thus this thread will be sent to sleep
          CHECK (delay_us < 20000);                                         // but beforehand the sleep-cycle is re-shuffled by a wait between 0 ... 20ms
          CHECK (slip_us  < 300);
          CHECK (activity::PASS == res);                                    // instruction to check back once
          pullWork();
          CHECK (activity::WAIT == res);                                    // but next call will send this thread to sleep right away
          CHECK (delay_us < 20);
          
          
          cout << "follow-up with some distance => follow-up delay"<<endl;
          now = RealClock::now();
          start = now + t100us;
          post (start);                                                     // This time the schedule is set to be "soon"
          post (start+t1ms);                                                // But another schedule is placed 1ms behind
          sleep_for (100us);                                                // wait for "soon" to pass...
          pullWork();
SHOW_EXPR(_raw(now))
SHOW_EXPR(_raw(start))
SHOW_EXPR(_raw(detector.invokeTime(probe)))
SHOW_EXPR(res);
SHOW_EXPR(delay_us)
SHOW_EXPR(slip_us)
SHOW_EXPR(wasInvoked(start))
SHOW_EXPR(scheduler.empty())
          CHECK (wasInvoked(start));                                       // Result: the first invocation happened immediately
          CHECK (slip_us  < 200);
          CHECK (delay_us > 900);                                          // yet this thread was afterwards kept in sleep to await the next one
          CHECK (activity::PASS == res);                                   // instruction to re-invoke immediately
          CHECK (not scheduler.empty());                                   // since there is still work in the queue
          
          start += t1ms;                                                   // (just re-adjust the reference point to calculate slip_us)
          pullWork();                                                      // re-invoke immediately as instructed
          CHECK (wasInvoked(start));                                       // Result: also the next Activity has been dispatched
          CHECK (slip_us < 400);                                           // not much slip
          CHECK (slip_us < 20000);                                         // ...and the post-delay is used to re-shuffle the sleep cycle as usual
          CHECK (activity::PASS == res);                                   // since queue is empty, we will call back once...
          CHECK (scheduler.empty());
          pullWork();
          CHECK (activity::WAIT == res);                                   // and then go to sleep.
          
          cout << detector.showLog()<<endl; // HINT: use this for investigation...
        }
      
      
      
      /** @test TODO
       * @todo WIP 10/23 🔁 define ⟶ implement
       */
      void
      walkingDeadline()
        {
          UNIMPLEMENTED ("walking Deadline");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SchedulerService_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
