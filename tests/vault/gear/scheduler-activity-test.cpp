/*
  SchedulerActivity(Test)  -  verify activities processed in the scheduler

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

/** @file scheduler-activity-test.cpp
 ** unit test \ref SchedulerActivity_test
 */


#include "lib/test/run.hpp"
#include "vault/gear/activity-lang.hpp"
#include "lib/time/timevalue.hpp"
//#include "lib/format-cout.hpp"
//#include "lib/util.hpp"

//#include <utility>

using test::Test;
using lib::time::Time;
using lib::time::FSecs;
//using std::move;
//using util::isSameObject;


namespace vault{
namespace gear  {
namespace test {
  
//  using lib::time::FrameRate;
//  using lib::time::Offset;
//  using lib::time::Time;
  
  
  
  
  
  /*****************************************************************//**
   * @test verify behaviour of the Scheduler _Activity Language._
   * @see SchedulerCommutator_test
   * @see SchedulerUsage_test
   */
  class SchedulerActivity_test : public Test
    {
      
      virtual void
      run (Arg)
        {
           simpleUsage();
           walkingDeadline();
           setupLalup();
        }
      
      
      /** @test TODO demonstrate a simple usage scenario
       */
      void
      simpleUsage()
        {
          // Activities are »POD with constructor«
          Activity gate{5, Time{FSecs{3,2}}};
          CHECK (gate.verb_ == Activity::GATE);
          CHECK (gate.next == nullptr);
          CHECK (gate.data_.condition.rest == 5);
          CHECK (gate.data_.condition.dead == Time(500,1));
        }
      
      
      
      /** @test TODO
       */
      void
      walkingDeadline()
        {
        }
      
      
      
      /** @test TODO
       */
      void
      setupLalup()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SchedulerActivity_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
