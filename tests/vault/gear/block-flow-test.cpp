/*
  BlockFlow(Test)  -  verify scheduler memory management scheme

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

/** @file block-flow-test.cpp
 ** unit test \ref BlockFlow_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "vault/gear/block-flow.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/format-cout.hpp"
//#include "lib/util.hpp"

//#include <utility>

using test::Test;
//using std::move;
//using util::isSameObject;
using lib::test::randTime;


namespace vault{
namespace gear {
namespace test {
  
//  using lib::time::FrameRate;
//  using lib::time::Offset;
//  using lib::time::Time;
  
  
  
  
  
  /*****************************************************************//**
   * @test document the memory management scheme used by the Scheduler.
   * @see SchedulerActivity_test
   * @see SchedulerUsage_test
   */
  class BlockFlow_test : public Test
    {
      
      virtual void
      run (Arg)
        {
           simpleUsage();
           calculateDeadline();
           setupLalup();
        }
      
      
      /** @test TODO demonstrate a simple usage scenario
       */
      void
      simpleUsage()
        {
          BlockFlow bFlow;
          Time deadline = randTime();
          Activity tick = bFlow.createActivity(Activity::TICK, deadline);
          ///////////////////////////////////////////////////////////////////////////////OOO diagnostic function to check allocation
          
          bFlow.discardBefore (deadline + Time{0,5});
          ///////////////////////////////////////////////////////////////////////////////OOO diagnostic function to check de-allocation
        }
      
      
      
      /** @test TODO
       */
      void
      calculateDeadline()
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
  LAUNCHER (BlockFlow_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
