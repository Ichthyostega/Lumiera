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
//#include "lib/time/timevalue.hpp"
//#include "lib/format-cout.hpp"
//#include "lib/util.hpp"

//#include <utility>

using test::Test;
using std::move;
//using util::isSameObject;


namespace vault{
namespace gear {
namespace test {
  
//  using lib::time::FrameRate;
//  using lib::time::Offset;
//  using lib::time::Time;
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
           walkingDeadline();
           setupLalup();
        }
      
      
      /** @test TODO demonstrate a simple usage scenario
       * @todo WIP 10/23 🔁 define ⟶ 🔁 implement
       */
      void
      simpleUsage()
        {
          LoadController ctrl;
        }
      
      
      
      /** @test verify classification of time horizon for scheduling.
       *      - if the next planned Activity lies beyond the SLEEP_HORIZON,
       *        then the current thread can be considered part of the _idle capacity_
       *      - in a similar way, WORK_HORIZON delineates the zone of repeated incoming
       *        Activities from the zone considered part of current active operation
       *      - Activities within the NOW_HORIZON can be awaited by yield-spinning
       *      - and any event from current into the past will be scheduled right away
       * @todo WIP 10/23 ✔ define ⟶ ✔ implement
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
          Time t4{next - NOW_HORIZON};
          
          CHECK (Capacity::IDLETIME == LoadController::classifyTimeHorizon (Offset{next - ut }));
          CHECK (Capacity::IDLETIME == LoadController::classifyTimeHorizon (Offset{next - t1 }));
          CHECK (Capacity::WORKTIME == LoadController::classifyTimeHorizon (Offset{next - t2 }));
          CHECK (Capacity::WORKTIME == LoadController::classifyTimeHorizon (Offset{next - t21}));
          CHECK (Capacity::NEARTIME == LoadController::classifyTimeHorizon (Offset{next - t3 }));
          CHECK (Capacity::NEARTIME == LoadController::classifyTimeHorizon (Offset{next - t31}));
          CHECK (Capacity::SPINTIME == LoadController::classifyTimeHorizon (Offset{next - t4 }));
          
          CHECK (Capacity::DISPATCH == LoadController::classifyTimeHorizon (Offset::ZERO      ));
          CHECK (Capacity::DISPATCH == LoadController::classifyTimeHorizon (Offset{t4 - next }));
        }
      
      
      
      /** @test verify the mark for _tended next head_ Activity.
       * @todo WIP 10/23 ✔ define ⟶ ✔ implement
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
       * @todo WIP 10/23 ✔ define ⟶ ✔ implement
       */
      void
      classifyCapacity()
        {
          LoadController lctrl;
          
          Time next{0,10};
          
          Time ut{1,0};
          Time t1{0,9};
          Time t2{next - SLEEP_HORIZON};
          Time t3{next - WORK_HORIZON};
          Time t4{next - NOW_HORIZON};
          Time t5{next + ut};                              //       ╭──────────────  next Activity at scheduler head
                                                          //        │     ╭────────  current time of evaluation
          // Time `next` has not been tended yet...      //         ▼     ▼
          CHECK (Capacity::TENDNEXT == lctrl.markOutgoingCapacity (next, ut ));
          
          // but after marking `next` as tended, capacity can be directed elsewhere
          lctrl.tendNext (next);
          CHECK (Capacity::IDLETIME == lctrl.markOutgoingCapacity (next, ut ));
          
          CHECK (Capacity::IDLETIME == lctrl.markOutgoingCapacity (next, t1 ));
          CHECK (Capacity::WORKTIME == lctrl.markOutgoingCapacity (next, t2 ));
          CHECK (Capacity::NEARTIME == lctrl.markOutgoingCapacity (next, t3 ));
          CHECK (Capacity::SPINTIME == lctrl.markOutgoingCapacity (next, t4 ));
          
          CHECK (Capacity::DISPATCH == lctrl.markOutgoingCapacity (next,next));
          CHECK (Capacity::DISPATCH == lctrl.markOutgoingCapacity (next, t5 ));
          
          CHECK (Capacity::IDLETIME == lctrl.markIncomingCapacity (next, ut ));
          CHECK (Capacity::IDLETIME == lctrl.markIncomingCapacity (next, t1 ));
          CHECK (Capacity::IDLETIME == lctrl.markIncomingCapacity (next, t2 ));
          CHECK (Capacity::NEARTIME == lctrl.markIncomingCapacity (next, t3 ));
          CHECK (Capacity::SPINTIME == lctrl.markIncomingCapacity (next, t4 ));
          
          CHECK (Capacity::DISPATCH == lctrl.markIncomingCapacity (next,next));
          CHECK (Capacity::DISPATCH == lctrl.markIncomingCapacity (next, t5 ));
          
          // tend-next works in limited ways also on incoming capacity
          lctrl.tendNext (Time::NEVER);
          CHECK (Capacity::IDLETIME == lctrl.markIncomingCapacity (next, ut ));
          CHECK (Capacity::IDLETIME == lctrl.markIncomingCapacity (next, t1 ));
          CHECK (Capacity::IDLETIME == lctrl.markIncomingCapacity (next, t2 ));
          CHECK (Capacity::TENDNEXT == lctrl.markIncomingCapacity (next, t3 ));
          CHECK (Capacity::SPINTIME == lctrl.markIncomingCapacity (next, t4 ));
          
          CHECK (Capacity::DISPATCH == lctrl.markIncomingCapacity (next,next));
          CHECK (Capacity::DISPATCH == lctrl.markIncomingCapacity (next, t5 ));
          
          // while being used rather generously on outgoing capacity
          CHECK (Capacity::TENDNEXT == lctrl.markOutgoingCapacity (next, ut ));
          CHECK (Capacity::TENDNEXT == lctrl.markOutgoingCapacity (next, t1 ));
          CHECK (Capacity::TENDNEXT == lctrl.markOutgoingCapacity (next, t2 ));
          CHECK (Capacity::TENDNEXT == lctrl.markOutgoingCapacity (next, t3 ));
          CHECK (Capacity::SPINTIME == lctrl.markOutgoingCapacity (next, t4 ));
          
          CHECK (Capacity::DISPATCH == lctrl.markOutgoingCapacity (next,next));
          CHECK (Capacity::DISPATCH == lctrl.markOutgoingCapacity (next, t5 ));
        }
      
      
      
      /** @test verify the re-distribution of free capacity by targeted delay
       * @todo WIP 10/23 🔁 define ⟶ implement
       */
      void
      scatteredReCheck()
        {
          Wiring setup;
          setup.maxCapacity = 16;
          LoadController lctrl{move(setup)};
          
          auto isBetween = [](auto lo, auto hi, auto val)
                              {
                                return lo <= val and val < hi;
                              };
          
          TimeVar now = RealClock::now();
          Time next{now + FSecs(10)};
          lctrl.tendNext (next);
          CHECK (Time::ZERO ==                              lctrl.scatteredDelayTime (now, Capacity::DISPATCH) );
          CHECK (Time::ZERO ==                              lctrl.scatteredDelayTime (now, Capacity::SPINTIME) );
          CHECK (      next ==                              lctrl.scatteredDelayTime (now, Capacity::TENDNEXT) );
          CHECK (isBetween (      next, next+WORK_HORIZON , lctrl.scatteredDelayTime (now, Capacity::NEARTIME)));
          CHECK (isBetween (      next, next+SLEEP_HORIZON, lctrl.scatteredDelayTime (now, Capacity::WORKTIME)));
          CHECK (isBetween (Time::ZERO, SLEEP_HORIZON     , lctrl.scatteredDelayTime (now, Capacity::IDLETIME)));
        }
      
      
      /** @test TODO
       * @todo WIP 10/23 🔁 define ⟶ implement
       */
      void
      walkingDeadline()
        {
          UNIMPLEMENTED ("walking Deadline");
        }
      
      
      
      /** @test TODO
       * @todo WIP 10/23 🔁 define ⟶ implement
       */
      void
      setupLalup()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SchedulerLoadControl_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
