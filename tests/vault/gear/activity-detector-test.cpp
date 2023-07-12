/*
  ActivityDetector(Test)  -  verify diagnostic setup to watch scheduler activities

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

/** @file activity-detector-test.cpp
 ** unit test \ref ActivityDetector_test
 */


#include "lib/test/run.hpp"
#include "activity-detector.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/format-cout.hpp"
//#include "lib/util.hpp"

//#include <utility>

//using test::Test;
//using lib::time::Time;
//using lib::time::FSecs;
//using std::move;
//using util::isSameObject;


namespace vault{
namespace gear {
namespace test {
  
//  using lib::time::FrameRate;
//  using lib::time::Offset;
//  using lib::time::Time;
  
  
  
  
  
  /*****************************************************************//**
   * @test verify instrumentation setup to watch scheduler Activities.
   * @see SchedulerActivity_test
   * @see SchedulerUsage_test
   */
  class ActivityDetector_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          
          verifyDummyFuncttor();
          detect_activation();
          detect_gate();
        }
      
      
      /** @test TODO demonstrate a simple usage scenario
       * @todo WIP 7/23 ⟶ define ⟶ implement
       */
      void
      simpleUsage()
        {
          ActivityDetector spectre;
        }
      
      
      
      /** @test TODO diagnostic setup to detect a JobFunctor activation
       * @todo WIP 7/23 ⟶ define ⟶ implement
       */
      void
      verifyDummyFuncttor()
        {
        }
      
      
      
      /** @test TODO diagnostic setup to detect Activity activation and propagation
       * @todo WIP 7/23 ⟶ define ⟶ implement
       */
      void
      detect_activation()
        {
        }
      
      
      
      /** @test TODO diagnostic setup to watch Activity::GATE activation
       * @todo WIP 7/23 ⟶ define ⟶ implement
       */
      void
      detect_gate()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ActivityDetector_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
