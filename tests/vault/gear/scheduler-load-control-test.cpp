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
//#include "lib/time/timevalue.hpp"
//#include "lib/format-cout.hpp"
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
//  using lib::time::Time;
  
  
  
  
  
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
           walkingDeadline();
           setupLalup();
        }
      
      
      /** @test TODO demonstrate a simple usage scenario
       * @todo WIP 10/23 ✔ define ⟶ 🔁 implement
       */
      void
      simpleUsage()
        {
          BlockFlowAlloc bFlow;
          LoadController lcontrl{bFlow};
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
