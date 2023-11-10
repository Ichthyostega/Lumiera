/*
  SchedulerLoadControl(Test)  -  verify scheduler load management facility

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

/** @file scheduler-load-control-test.cpp
 ** unit test \ref SchedulerLoadControl_test
 */


#include "lib/test/run.hpp"
#include "vault/gear/load-controller.hpp"
#include "vault/real-clock.hpp"

#include <chrono>

using test::Test;


namespace vault{
namespace gear {
namespace test {
  
  using std::move;
  using std::chrono::microseconds;
  
  using Capacity = LoadController::Capacity;
  using Wiring = LoadController::Wiring;
  
  
  
  
  
  /*************************************************************************//**
   * @test verify behaviour patterns relevant for Scheduler load control.
   * @see SchedulerCommutator_test
   * @see SchedulerService_test
   * @see SchedulerStress_test
   */
  class SchedulerLoadControl_test : public Test
    {
      
      virtual void
      run (Arg)
        {
           simpleUsage();
           classifyHorizon();
           tendNextActivity();
           classifyCapacity();
           scatteredReCheck();
           indicateAverageLoad();
        }
      
      
      /** @test TODO demonstrate a simple usage scenario
       * @todo WIP 10/23 🔁 define ⟶ 🔁 implement
       */
      void
      simpleUsage()
        {
          LoadController ctrl;
          /////////////////////////TODO a simple usage example focusing on load diagnostics
        }
      
      
      
      /** @test verify classification of time horizon for scheduling.
       *      - if the next planned Activity lies beyond the SLEEP_HORIZON,
       *        then the current thread can be considered part of the _idle capacity_
       *      - in a similar way, WORK_HORIZON delineates the zone of repeated incoming
       *        Activities from the zone considered part of current active operation
       *      - Activities within the NOW_HORIZON can be awaited by yield-spinning
       *      - and any event from current into the past will be scheduled right away
       */
      void
      classifyHorizon()
        {
          Time next{0,10};
          
          Time ut{1,0};
          Time t1{0,9};
          Time t2{next - SLEEP_HORIZON};
          Time t21{t2 + ut};
          Time t3{next - WORK_HORIZON};
          Time t31{t3 + ut};
          Time t4{next - NEAR_HORIZON};
          
          CHECK (Capacity::IDLEWAIT == LoadController::classifyTimeHorizon (Offset{next - ut }));
          CHECK (Capacity::IDLEWAIT == LoadController::classifyTimeHorizon (Offset{next - t1 }));
          CHECK (Capacity::WORKTIME == LoadController::classifyTimeHorizon (Offset{next - t2 }));
          CHECK (Capacity::WORKTIME == LoadController::classifyTimeHorizon (Offset{next - t21}));
          CHECK (Capacity::NEARTIME == LoadController::classifyTimeHorizon (Offset{next - t3 }));
          CHECK (Capacity::NEARTIME == LoadController::classifyTimeHorizon (Offset{next - t31}));
          CHECK (Capacity::SPINTIME == LoadController::classifyTimeHorizon (Offset{next - t4 }));
          
          CHECK (Capacity::DISPATCH == LoadController::classifyTimeHorizon (Offset::ZERO      ));
          CHECK (Capacity::DISPATCH == LoadController::classifyTimeHorizon (Offset{t4 - next }));
        }
      
      
      
      /** @test verify the mark for _tended next head_ Activity.
       */
      void
      tendNextActivity()
        {
          LoadController lctrl;
          
          Time t1{1,0};
          Time t2{2,0};
          Time t3{3,0};
          
          CHECK (not lctrl.tendedNext (t2));
          
          lctrl.tendNext (t2);
          CHECK (    lctrl.tendedNext (t2));
          CHECK (not lctrl.tendedNext (t3));
          
          lctrl.tendNext (t3);
          CHECK (    lctrl.tendedNext (t3));
          
          // However — this is not a history memory...
          CHECK (not lctrl.tendedNext (t1));
          CHECK (not lctrl.tendedNext (t2));
          CHECK (    lctrl.tendedNext (t3));
          
          lctrl.tendNext (t1);
          CHECK (    lctrl.tendedNext (t1));
          CHECK (not lctrl.tendedNext (t2));
          CHECK (not lctrl.tendedNext (t3));
          
          lctrl.tendNext (t2);
          CHECK (not lctrl.tendedNext (t1));
          CHECK (    lctrl.tendedNext (t2));
          CHECK (not lctrl.tendedNext (t3));
        }
      
      
      
      /** @test verify allocation decision for free capacity
       *      - due and overdue Activities are prioritised
       *      - keep spinning when next Activity to schedule is very close
       *      - otherwise, priority is to tend for the next known Activity
       *      - beyond that, free capacity is redistributed according to horizon
       *      - for incoming free capacity there is a preference to keep it sleeping,
       *        to allow for disposing of excess capacity after extended sleep time
       */
      void
      classifyCapacity()
        {
          LoadController lctrl;
          
          Time next{0,10};
          Time nil{Time::NEVER};
          
          Time mt{1,0};
          Time t1{0,9};
          Time t2{next - SLEEP_HORIZON};
          Time t3{next - WORK_HORIZON};
          Time t4{next - NEAR_HORIZON};
          Time t5{next + mt};                              //       ╭──────────────  next Activity at scheduler head
                                                          //        │     ╭────────  current time of evaluation
          // Time `next` has not been tended yet...      //         ▼     ▼
          CHECK (Capacity::TENDNEXT == lctrl.markOutgoingCapacity (next, mt ));
          
          // but after marking `next` as tended, capacity can be directed elsewhere
          lctrl.tendNext (next);
          CHECK (Capacity::WORKTIME == lctrl.markOutgoingCapacity (next, mt ));
          
          CHECK (Capacity::WORKTIME == lctrl.markOutgoingCapacity ( nil, mt ));
          CHECK (Capacity::WORKTIME == lctrl.markOutgoingCapacity (next, t1 ));
          CHECK (Capacity::WORKTIME == lctrl.markOutgoingCapacity (next, t2 ));
          CHECK (Capacity::NEARTIME == lctrl.markOutgoingCapacity (next, t3 ));
          CHECK (Capacity::SPINTIME == lctrl.markOutgoingCapacity (next, t4 ));
          
          CHECK (Capacity::DISPATCH == lctrl.markOutgoingCapacity (next,next));
          CHECK (Capacity::DISPATCH == lctrl.markOutgoingCapacity (next, t5 ));
          
          CHECK (Capacity::IDLEWAIT == lctrl.markIncomingCapacity ( nil, mt ));
          CHECK (Capacity::IDLEWAIT == lctrl.markIncomingCapacity (next, t1 ));
          CHECK (Capacity::IDLEWAIT == lctrl.markIncomingCapacity (next, t2 ));
          CHECK (Capacity::NEARTIME == lctrl.markIncomingCapacity (next, t3 ));
          CHECK (Capacity::SPINTIME == lctrl.markIncomingCapacity (next, t4 ));
          
          CHECK (Capacity::DISPATCH == lctrl.markIncomingCapacity (next,next));
          CHECK (Capacity::DISPATCH == lctrl.markIncomingCapacity (next, t5 ));
          
          // tend-next works in limited ways also on incoming capacity
          lctrl.tendNext (Time::NEVER);   // mark as not yet tended...
          CHECK (Capacity::IDLEWAIT == lctrl.markIncomingCapacity ( nil, mt ));
          CHECK (Capacity::IDLEWAIT == lctrl.markIncomingCapacity (next, t1 ));
          CHECK (Capacity::IDLEWAIT == lctrl.markIncomingCapacity (next, t2 ));
          CHECK (Capacity::TENDNEXT == lctrl.markIncomingCapacity (next, t3 ));
          CHECK (Capacity::SPINTIME == lctrl.markIncomingCapacity (next, t4 ));
          
          CHECK (Capacity::DISPATCH == lctrl.markIncomingCapacity (next,next));
          CHECK (Capacity::DISPATCH == lctrl.markIncomingCapacity (next, t5 ));
          
          // while being used rather generously on outgoing capacity
          CHECK (Capacity::WORKTIME == lctrl.markOutgoingCapacity ( nil, mt ));  //  re-randomisation before long-term sleep
          CHECK (Capacity::TENDNEXT == lctrl.markOutgoingCapacity (next, t1 ));
          CHECK (Capacity::TENDNEXT == lctrl.markOutgoingCapacity (next, t2 ));
          CHECK (Capacity::TENDNEXT == lctrl.markOutgoingCapacity (next, t3 ));
          CHECK (Capacity::SPINTIME == lctrl.markOutgoingCapacity (next, t4 ));
          
          CHECK (Capacity::DISPATCH == lctrl.markOutgoingCapacity (next,next));
          CHECK (Capacity::DISPATCH == lctrl.markOutgoingCapacity (next, t5 ));
        }
      
      
      
      /** @test verify the re-distribution of free capacity by targeted delay
       *      - the implementation uses the next-tended start time as anchor point
       *      - capacity classes which should be scheduled right away will actually
       *        never call this function — yet still a sensible value is returned here
       *      - capacity targeted at current work will be redistributed behind the
       *        next-tended time, and within a time span corresponding to the work realm
       *      - capacity targeted towards more future work will be distributed within
       *        the horizon defined by the sleep-cycle
       *      - especially for capacity sent to sleep, this redistribution works
       *        without being shifted behind the next-tended time, since in that case
       *        the goal is to produce a random distribution of the »sleeper« callbacks.
       *      - the offset is indeed randomised, using current time for randomisation
       * @see LoadController::scatteredDelayTime()
       */
      void
      scatteredReCheck()
        {
          auto is_between = [](auto lo, auto hi, auto val)
                              {
                                return lo <= val and val < hi;
                              };
          
          LoadController lctrl;
          
          TimeVar now = RealClock::now();
          Offset ten{FSecs(10)};
          Time next{now + ten};
          lctrl.tendNext(next);
          
          CHECK (Time::ZERO ==                              lctrl.scatteredDelayTime (now, Capacity::DISPATCH) );
          CHECK (Time::ZERO ==                              lctrl.scatteredDelayTime (now, Capacity::SPINTIME) );
          CHECK (       ten ==                              lctrl.scatteredDelayTime (now, Capacity::TENDNEXT) );
          CHECK (is_between (       ten, ten+ WORK_HORIZON, lctrl.scatteredDelayTime (now, Capacity::NEARTIME)));
          CHECK (is_between (       ten, ten+SLEEP_HORIZON, lctrl.scatteredDelayTime (now, Capacity::WORKTIME)));
          CHECK (is_between (       ten, ten+SLEEP_HORIZON, lctrl.scatteredDelayTime (now, Capacity::IDLEWAIT)));
          
          lctrl.tendNext(Time::ANYTIME); // reset to ensure we get no base offset
          
          // Offset is randomised based on the current time
          // Verify this yields an even distribution
          double avg{0};
          const size_t REPETITIONS = 1e6;
          for (size_t i=0; i< REPETITIONS; ++i)
            avg += _raw(lctrl.scatteredDelayTime (RealClock::now(), Capacity::IDLEWAIT));
          avg /= REPETITIONS;
          
          auto expect = _raw(SLEEP_HORIZON)/2;
          auto error = fabs(avg/expect - 1);
          CHECK (0.002 > error);        // observing a quite stable skew ~ 0.8‰ on my system
        }                              //  let's see if this error bound triggers eventually...
      
      
      
      
      /** @test verify fusion of sampled observations to guess average scheduler load
       *      - use a rigged wiring of the load controller to verify calculation
       *        based on known values of current _concurrency_ and _schedule pressure_
       *      - scheduling on average 200µs behind nominal schedule is considered
       *        the regular balanced state and thus defined as 100% schedule pressure
       *      - if congestion builds up to 1/10 of WORK_HORIZON, 200% overload is indicated
       *      - on the other hand, if workers appear on average 200µs before the typical
       *        balanced state, the resulting headroom is defined to constitute 50% pressure
       *      - the pressure value is multiplied with the degree of concurrency
       *      - the pressure is sampled from the lag (distance of current time to the
       *        next activity to schedule), which is observed whenever a worker
       *        calls in to retrieve more work. These calls happen randomly.
       */
      void
      indicateAverageLoad()
        {
          uint maxThreads = 10;
          uint currThreads = 0;
          
          LoadController::Wiring setup;
          setup.maxCapacity       = [&]{ return maxThreads; };
          setup.currWorkForceSize = [&]{ return currThreads; };
          // rigged setup to verify calculated load indicator
          LoadController lctrl{move(setup)};
          
          CHECK (0 == lctrl.averageLag());
          CHECK (0 == lctrl.effectiveLoad());
          
          // Manipulate the sampled average lag (in µs)
          lctrl.setCurrentAverageLag (200);
          // Scheduling 200µs behind nominal start time -> 100% schedule pressure
          
          currThreads = 5;
          CHECK (0.5 == lctrl.effectiveLoad());
          currThreads = 8;
          CHECK (0.8 == lctrl.effectiveLoad());
          currThreads = 10;
          CHECK (1.0 == lctrl.effectiveLoad());
          
          // congestion +500µs -> 200% schedule pressure
          lctrl.setCurrentAverageLag (200+500);
          CHECK (2.0 == lctrl.effectiveLoad());
          
          lctrl.setCurrentAverageLag (200+500+500);
          CHECK (3.0 == lctrl.effectiveLoad());  // -> 300%
          
          // if average headroom 500µs -> 50% load
          lctrl.setCurrentAverageLag (200-500);
          CHECK (0.5 == lctrl.effectiveLoad());
          CHECK (-300 == lctrl.averageLag());
          
          lctrl.setCurrentAverageLag (200-500-500-500);
          CHECK (0.25 == lctrl.effectiveLoad());
          CHECK (-1300 == lctrl.averageLag());
          
          // load indicator is always modulated by concurrency level
          currThreads = 2;
          CHECK (0.05 == lctrl.effectiveLoad());
          
          // average lag is sampled from the situation when workers call in
          Time head = Time::ZERO;
          TimeVar curr = Time{1,0};
          lctrl.markIncomingCapacity (head,curr);
          CHECK (-882 == lctrl.averageLag());
          
          lctrl.markIncomingCapacity (head,curr);
          CHECK (-540 == lctrl.averageLag());

          curr = Time{0,1};
          lctrl.markIncomingCapacity (head,curr);
          lctrl.markIncomingCapacity (head,curr);
          CHECK (1291 == lctrl.averageLag());
          
          curr = head - Time{0,2};
          lctrl.markIncomingCapacity (head,curr);
          CHECK (-2581 == lctrl.averageLag());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SchedulerLoadControl_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
