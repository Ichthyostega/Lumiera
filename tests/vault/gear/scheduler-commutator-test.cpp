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
#include "vault/gear/scheduler-commutator.hpp"
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
           simpleUsage();
           walkingDeadline();
           setupLalup();
        }
      
      
      /** @test TODO demonstrate a simple usage scenario
       */
      void
      simpleUsage()
        {
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
  LAUNCHER (SchedulerCommutator_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
