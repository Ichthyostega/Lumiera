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
          
          start = Time::ZERO;
//          auto& ctx = detector.executionCtx;
          auto& ctx = Scheduler::ExecutionCtx::from(scheduler);
          probe.activate(start, ctx);
          auto [muck,_] = lib::test::microBenchmark([&](size_t){ probe.activate(start, ctx);
                                                                  return size_t(1);
                                                                }
                                                    , 200);
SHOW_EXPR(muck)          
          
          cout << "Scheduled right away..."<<endl;
          start = RealClock::now();
          post(start);
          
SHOW_EXPR(_raw(detector.invokeTime(probe)) - _raw(start))
          
          CHECK (wasInvoked(start));
          CHECK (scheduler.empty());

          cout << "pullWork() on empty queue..."<<endl;
//          start = RealClock::now();
          pullWork();
          
SHOW_EXPR(wasInvoked(start))
          CHECK (activity::WAIT == res);
          CHECK (slip_us  < 100);
          
          cout << "Due at pullWork()..."<<endl;
          TimeVar now = RealClock::now();
          start = now + t100us;
          post (start);
          CHECK (not scheduler.empty());
          
          TimeVar cow = RealClock::now();
          sleep_for (100us);
          TimeVar wow = RealClock::now();
          pullWork();
SHOW_EXPR(_raw(cow) - _raw(now))
SHOW_EXPR(_raw(wow) - _raw(cow))
SHOW_EXPR(_raw(detector.invokeTime(probe)) - _raw(wow))
          CHECK (activity::WAIT == res);
          CHECK (wasInvoked(start));
          CHECK (scheduler.empty());
          CHECK (delay_us < 500);
          CHECK (slip_us  < 500);

          cout << "next some time ahead => up-front delay"<<endl;
          now = RealClock::now();
          start = now + t500us;
          post (start);
          CHECK (not scheduler.empty());
          
          pullWork();
          CHECK (activity::PASS == res);
          CHECK (not wasInvoked(start));
          CHECK (delay_us > 500);
          CHECK (delay_us < 1000);
          pullWork();
          CHECK (wasInvoked(start));
          CHECK (delay_us < 200);  ///////////////OOO dangerously tight
          CHECK (slip_us  < 500);
          CHECK (activity::WAIT == res);
          CHECK (scheduler.empty());
          
          cout << "follow-up with some distance => follow-up delay"<<endl;
          now = RealClock::now();
          start = now + t100us;
          post (start);
          post (start+t1ms);
          sleep_for (100us);
          pullWork();
SHOW_EXPR(_raw(now))
SHOW_EXPR(_raw(start))
SHOW_EXPR(_raw(detector.invokeTime(probe)))
SHOW_EXPR(res);
SHOW_EXPR(delay_us)
SHOW_EXPR(slip_us)
SHOW_EXPR(wasInvoked(start))
SHOW_EXPR(scheduler.empty())
          CHECK (wasInvoked(start));
          CHECK (delay_us > 900);
          CHECK (slip_us  < 100);
          CHECK (activity::PASS == res);
          CHECK (not scheduler.empty());
          
          start += t1ms;
          pullWork();
          CHECK (wasInvoked(start));
          CHECK (delay_us < 500);
          CHECK (slip_us  < 500);
          CHECK (activity::WAIT == res);
          CHECK (scheduler.empty());
          
          
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
