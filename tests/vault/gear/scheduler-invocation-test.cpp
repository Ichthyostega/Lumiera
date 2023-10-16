/*
  SchedulerInvocation(Test)  -  verify queue processing in the scheduler

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

/** @file scheduler-invocation-test.cpp
 ** unit test \ref SchedulerInvocation_test
 */


#include "lib/test/run.hpp"
#include "vault/gear/scheduler-invocation.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/format-cout.hpp"
#include "lib/util.hpp"

//#include <utility>

using test::Test;
//using std::move;
using util::isSameObject;


namespace vault{
namespace gear {
namespace test {
  
//  using lib::time::FrameRate;
//  using lib::time::Offset;
//  using lib::time::Time;
  
  
  
  
  
  /********************************************************************//**
   * @test Scheduler Layer-1: queue processing and invocation by priority.
   * @see SchedulerCommutator_test
   * @see SchedulerUsage_test
   */
  class SchedulerInvocation_test : public Test
    {
      
      virtual void
      run (Arg)
        {
           simpleUsage();
           verify_Queuing();
           verify_WaterLevel();
        }
      
      
      /** @test demonstrate a simple usage scenario of data passing
       */
      void
      simpleUsage()
        {
          SchedulerInvocation sched;
          Activity activity;
          
          CHECK (not sched.peekHead());
          
          sched.instruct (activity);
          sched.feedPriorisation();
          CHECK (sched.peekHead());
          
          Activity* head = sched.pullHead();
          CHECK (not sched.peekHead());
          CHECK (isSameObject (*head, activity)); 
        }
      
      
      
      /** @test TODO verify records are passed properly through the queues
       */
      void
      verify_Queuing()
        {
          UNIMPLEMENTED ("queues");
        }
      
      
      
      /** @test TODO verify the given time point is utilised for prioritisation
       */
      void
      verify_WaterLevel()
        {
          UNIMPLEMENTED ("water level");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SchedulerInvocation_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
