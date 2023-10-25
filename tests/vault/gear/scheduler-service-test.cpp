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
//#include "lib/util.hpp"

//#include <utility>

using test::Test;
//using std::move;
//using util::isSameObject;


namespace vault{
namespace gear {
namespace test {
  
//  using lib::time::FrameRate;
//  using lib::time::Offset;
  using lib::time::Time;
  
  
  
  
  
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
      
      
      
      /** @test TODO verify visible behaviour of the work-pulling function
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
          
          // this test class is declared friend to get a backdoor to Scheduler internals...
          auto& schedCtx = Scheduler::ExecutionCtx::from(scheduler);
          Time now = schedCtx.getSchedTime();
          schedCtx.post (now, &probe, schedCtx);
          
          CHECK (activity::WAIT == scheduler.getWork());
          
          cout << detector.showLog()<<endl; // HINT: use this for investigation...
          CHECK (detector.verifyInvocation("testProbe"));
          ////////////////////////////////////////////////////////////////////////////////////TODO need a way to get the actual time passed to the Probe
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
