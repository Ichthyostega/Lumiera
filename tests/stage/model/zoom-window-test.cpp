/*
  ZoomWindow(Test)  -  verify translation from model to screen coordinates

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file zoom-window-test.cpp
 ** unit test \ref ZoomWindow_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "stage/model/zoom-window.hpp"
//#include "lib/util.hpp"

//#include <utility>
//#include <memory>


//using util::isSameObject;
//using std::make_unique;
//using std::move;


namespace stage{
namespace model{
namespace test {
  
  
  namespace { // Test fixture...
    
//  template<typename X>
//  struct DummyWidget
//    : public sigc::trackable
//    {
//      X val = 1 + rand() % 100;
//    };
  }
  
  
  
  
  /*************************************************************************************//**
   * @test verify consistent handling of scrolling and zoom settings for the timeline.
   *       - setting the overall range
   *       - setting the visible range
   *       - adjusting the scale factor
   *       - setting a visible position
   *       - nudging the position
   *       - nudging the scale factor
   * @see zoom-window.hpp
   */
  class ZoomWindow_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          verify_simpleUsage();
        }
      
      
      /** @test the standard use case is to.... TBW
       */
      void
      verify_simpleUsage()
        {
          ZoomWindow zoomWin;
          CHECK (zoomWin.overallSpan() == TimeSpan(Time::ZERO, Time(FSecs(23))));
          CHECK (zoomWin.visible()     == TimeSpan(Time::ZERO, Time(FSecs(23))));
          CHECK (zoomWin.px_per_sec()  == 25);
          
          zoomWin.nudgeMetric(+1);
          CHECK (zoomWin.px_per_sec()  == 50);
          CHECK (zoomWin.visible()     == TimeSpan(Time(FSecs(23,4)), Time(FSecs(23,2))));
          CHECK (zoomWin.overallSpan() == TimeSpan(Time::ZERO, Time(FSecs(23))));
        }
      
      
      /** @test  */
    };
  
  
  /** Register this test class... */
  LAUNCHER (ZoomWindow_test, "unit gui");
  
  
}}} // namespace stage::model::test
