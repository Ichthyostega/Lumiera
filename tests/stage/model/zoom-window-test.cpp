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



namespace stage{
namespace model{
namespace test {
  
  
  namespace { // simplified notation for expected results...
    
    inline Time _t(int secs)       { return Time(FSecs(secs)); }
    inline Time _t(int s, int div) { return Time(FSecs(s,div)); }
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
          // Explanation of the notation used in this test...
          CHECK (_t(10) == Time{FSecs(10)});                                     // Time point at t = 10sec
          CHECK (_t(10,3) == Time{FSecs(10,3)});                                 // Time point at t = 10/3sec (fractional number)
          CHECK (FSecs(10,3) == FSecs(10)/3);                                    // fractional number arithmetics
          CHECK (FSecs(10)/3 == 10_r/3);                                         // _r is a user defined literal to denote 64-bit fractional
          CHECK (10_r/3 == Rat(10,3));
          CHECK (Rat(10/3) == boost::rational<int64_t>(10,3));                   // using Rat = boost::rational<int64_t>
          CHECK (rational_cast<float> (10_r/3) == 3.33333f);                     // rational_cast performs the division with indicated type
          
          verify_simpleUsage();
          verify_setup();
          verify_calibration();
          verify_metric();
          verify_window();
          verify_scroll();
        }
      
      
      /** @test simple usage example: double the zoom level, then scroll to the left */
      void
      verify_simpleUsage()
        {
          ZoomWindow zoomWin;
          CHECK (zoomWin.overallSpan() == TimeSpan(_t(0), FSecs(23)));
          CHECK (zoomWin.visible()     == TimeSpan(_t(0), FSecs(23)));
          CHECK (zoomWin.px_per_sec()  == 25);
          
          zoomWin.nudgeMetric(+1);
          CHECK (zoomWin.px_per_sec()  == 50);
          CHECK (zoomWin.visible()     == TimeSpan(_t(23,4), FSecs(23,2)));
          CHECK (zoomWin.overallSpan() == TimeSpan(_t(0),    FSecs(23)));
          
          zoomWin.nudgeVisiblePos(-1);
          CHECK (zoomWin.px_per_sec()  == 50);
          CHECK (zoomWin.visible()     == TimeSpan(_t(0), FSecs(23,2)));
          CHECK (zoomWin.overallSpan() == TimeSpan(_t(0), FSecs(23)));
        }
      
      
      /** @test verify the possible variations for initial setup of the zoom window
       *        - can be defined either the canvas duration, or an explicit extension
       *          given in pixels, or both
       *        - window extension, when given, defines the visible span
       *        - otherwise the whole canvas is visible, thereby defining the metric
       */
      void
      verify_setup()
        {
          ZoomWindow win1;
          CHECK (win1.overallSpan() == TimeSpan(_t(0), FSecs(23)));
          CHECK (win1.visible()     == win1.overallSpan());
          CHECK (win1.px_per_sec()  == 25);
          CHECK (win1.pxWidth()  == 23*25);
          
          ZoomWindow win2{TimeSpan{_t(-1), _t(+1)}};
          CHECK (win2.overallSpan() == TimeSpan(_t(-1), FSecs(2)));
          CHECK (win2.visible()     == win2.overallSpan());
          CHECK (win2.px_per_sec()  == 25);
          CHECK (win2.pxWidth()  ==  2*25);
          
          ZoomWindow win3{555};
          CHECK (win3.overallSpan() == TimeSpan(_t(0), FSecs(23)));
          CHECK (win3.pxWidth()     == 555);
          CHECK (win3.px_per_sec()  == 555_r/23);
          CHECK (win3.visible()     == win3.overallSpan());
          
          ZoomWindow win4{555, TimeSpan{_t(-10), _t(-5)}};
          CHECK (win4.overallSpan() == TimeSpan(_t(10), FSecs(5)));
          CHECK (win4.pxWidth()     == 555);
          CHECK (win4.px_per_sec()  == 111);
          CHECK (win4.visible()     == win4.overallSpan());
        }
      
      
      /** @test verify defining and retaining of the effective extension in pixels
       *        - changes to the extension are applied by adjusting the visible window
       *        - visible window's start position is maintained
       *        - unless the resulting window would exceed the overall canvas,
       *          in which case the window is shifted, retaining metrics
       *        - however, if resulting window can not be made to fit, it is truncated
       *          to current canvas and metric is adjusted to keep overall pixel extension
       */
      void
      verify_calibration()
        {
          ZoomWindow win;
          CHECK (win.overallSpan() == TimeSpan(_t(0), FSecs(23)));
          CHECK (win.visible()     == TimeSpan(_t(0), FSecs(23)));
          CHECK (win.pxWidth()  == 23*25);
          
          win.calibrateExtension(25);
          CHECK (win.overallSpan() == TimeSpan(_t(0), FSecs(23)));
          CHECK (win.visible()     == TimeSpan(_t(0), FSecs(1)));
          CHECK (win.px_per_sec()  == 25);
          CHECK (win.pxWidth()     == 25);
          
          win.setOverallRange(TimeSpan{_t(-50), _t(50)});
          CHECK (win.overallSpan() == TimeSpan(_t(-50), FSecs(100)));
          CHECK (win.visible()     == TimeSpan(_t(0), FSecs(1)));
          CHECK (win.px_per_sec()  == 25);
          CHECK (win.pxWidth()     == 25);
          
          win.calibrateExtension(100);
          CHECK (win.overallSpan() == TimeSpan(_t(-50), FSecs(100)));
          CHECK (win.visible()     == TimeSpan(_t(0), FSecs(4)));
          CHECK (win.px_per_sec()  == 25);
          CHECK (win.pxWidth()     == 100);
          
          win.setRanges (TimeSpan{_t(-50), _t(10)}, TimeSpan{_t(-10), FSecs(10)});
          CHECK (win.overallSpan() == TimeSpan(_t(-50), FSecs(60)));
          CHECK (win.visible()     == TimeSpan(_t(-10), _t(0)));
          CHECK (win.px_per_sec()  == 10);
          CHECK (win.pxWidth()     == 100);
          
          win.calibrateExtension(500);
          CHECK (win.overallSpan() == TimeSpan(_t(-50), FSecs(60)));
          CHECK (win.visible()     == TimeSpan(_t(-40), FSecs(50)));
          CHECK (win.px_per_sec()  == 10);
          CHECK (win.pxWidth()     == 500);
          
          win.setOverallDuration (Duration{FSecs(30)});
          CHECK (win.overallSpan() == TimeSpan(_t(-50), _t(-20)));
          CHECK (win.visible()     == TimeSpan(_t(-50), FSecs(30)));
          CHECK (win.px_per_sec()  == 500_r/30);
          CHECK (win.pxWidth()     == 500);
          
          win.calibrateExtension(300);
          CHECK (win.overallSpan() == TimeSpan(_t(-50), _t(-20)));
          CHECK (win.visible()     == TimeSpan(_t(-50), FSecs(30)*3/5));
          CHECK (win.px_per_sec()  == 500_r/30);
          CHECK (win.pxWidth()     == 300);
        }
      
      
      /** @test zoom in and out, thereby adjusting the metric
       *        - window extension in pixels is always retained
       *        - window is shifted when surpassing canvas bounds
       *        - metric is adjusted to keep excess window within pixel extension
       *        - otherwise zooming is centred around an anchor position, favouring centre
       */
      void
      verify_metric()
        {
          ZoomWindow win{1280, TimeSpan{_t(0), FSecs(64)}};
          CHECK (win.px_per_sec()  == 20);
          
          win.nudgeMetric(+1);
          CHECK (win.overallSpan() == TimeSpan(_t(0), _t(64)));
          CHECK (win.visible()     == TimeSpan(_t(-32,2), FSecs(32)));
          CHECK (win.px_per_sec()  == 40);
          CHECK (win.pxWidth()     == 1280);
          
          win.setVisiblePos(0.0);
          CHECK (win.visible()     == TimeSpan(_t(0), FSecs(32)));               // zoom window moved to left side of overall range
          
          win.nudgeMetric(+15);
          CHECK (win.overallSpan() == TimeSpan(_t(0), _t(64)));
          CHECK (win.visible()     == TimeSpan(_t(0), FSecs(32,32768)));         // now anchor position is at left bound
          CHECK (win.px_per_sec()  == 40*32768);
          CHECK (win.pxWidth()     == 1200);
          // Note: already getting close to the time grid...
          CHECK (win.visible().end() == TimeValue(976));
          CHECK (Time(FSecs(32,32768)) == TimeValue(976));
          CHECK (rational_cast<double> (32_r/32768 * Time::SCALE) == 976.5625);
          
          win.nudgeMetric(+1);
          CHECK (win.overallSpan() == TimeSpan(_t(0), _t(64)));
          CHECK (win.px_per_sec()  == ZOOM_MAX_RESOLUTION);                      // further zoom has been capped at 2px per µ-tick
          CHECK (win.visible()     == TimeSpan(_t(0), FSecs(1280_r/ZOOM_MAX_RESOLUTION)));
          CHECK (win.pxWidth()     == 1280);
          
          win.nudgeMetric(+1);
          CHECK (win.px_per_sec()  == ZOOM_MAX_RESOLUTION);
          win.setMetric(10*ZOOM_MAX_RESOLUTION);
          CHECK (win.px_per_sec()  == ZOOM_MAX_RESOLUTION);
          
          // so this is the deepest zoom possible....
          CHECK (win.visible().duration() == TimeValue(640));
          CHECK (TimeValue(640) == _t(1280,ZOOM_MAX_RESOLUTION));
          
          // and this the absolutely smallest possible zoom window
          win.calibrateExtension(2);
          CHECK (win.overallSpan() == TimeSpan(_t(0), _t(64)));
          CHECK (win.visible().duration() == TimeValue(1));
          CHECK (win.px_per_sec()  == ZOOM_MAX_RESOLUTION);
          CHECK (win.pxWidth()     == 2);

          win.calibrateExtension(1);
          CHECK (win.visible().duration() == TimeValue(1));                      // window is guaranteed to be non-empty
          CHECK (win.px_per_sec()  == ZOOM_MAX_RESOLUTION / 2);                  // zoom scale has thus been lowered to prevent window from vanishing
          CHECK (win.pxWidth()     == 1);

          win.calibrateExtension(1280);
          CHECK (win.visible().duration() == TimeValue(1280));
          CHECK (win.px_per_sec()  == ZOOM_MAX_RESOLUTION / 2);
          CHECK (win.pxWidth()     == 1280);
          CHECK (win.overallSpan() == TimeSpan(_t(0), _t(64)));
          
          win.nudgeMetric(-5);
          CHECK (win.visible().duration() == Duration{32 * FSecs(1280/Time::SCALE)});
          CHECK (win.px_per_sec()  == ZOOM_MAX_RESOLUTION / 64);
          CHECK (win.pxWidth()     == 1280);
          CHECK (win.overallSpan() == TimeSpan(_t(0), _t(64)));
          
          win.nudgeMetric(-12);
          CHECK (win.visible() == win.overallSpan());                            // zoom out stops at full canvas size
          CHECK (win.overallSpan() == TimeSpan(_t(0), _t(64)));
          CHECK (win.px_per_sec()  == 20);
          CHECK (win.pxWidth()     == 1280);
          
          // but canvas can be forcibly extended by »reverse zooming«
          win.expandVisibleRange (TimeSpan{_t(60), _t(62)});                     // zoom such as to bring current window at given relative position
          CHECK (win.px_per_sec()  == 20_r/64*2);                                // scale thus adjusted to reduce 64 sec to 2 sec (scale can be fractional!)
          CHECK (win.visible().duration() == _t(64 * 32));                       // zoom window has been inversely expanded by factor 64/2 == 32
          CHECK (win.visible() == win.overallSpan());                            // zoom fully covers the expanded canvas
          CHECK (win.overallSpan() == TimeSpan(_t(-1920), _t(128)));             // and overall canvas has been expanded to embed the previous window
          CHECK (win.overallSpan().duration() == _t(2048));                      // ... at indicated relative position (2sec ⟼ 64sec, one window size before end)
          
          // metric can be explicitly set (e.g. 5px per sound sample)
          win.setMetric (5 * 1_r/44100);
          CHECK (win.pxWidth() == 1280);
          CHECK (win.visible().duration() == Duration{FSecs(5,44100)*1280});
          CHECK (win.overallSpan().duration() == _t(2048));
        }
      
      
      /** @test position and extension of the visible window can be set explicitly */
      void
      verify_window()
        {
          ZoomWindow win{1280, TimeSpan{_t(0), FSecs(64)}};
          CHECK (win.visible() == win.overallSpan());
          CHECK (win.px_per_sec()  == 20);
          
          win.setVisibleDuration (Duration{FSecs(23,30)});
          CHECK (win.visible().duration() == _t(23,30));
          CHECK (win.visible().start() == Time(FSecs(64)/2 - FSecs(23/30)/2));   // when zooming down from full range, zoom anchor is window centre
          CHECK (win.px_per_sec()  == 1280_r/23*30);
          CHECK (win.pxWidth()     == 1280);
          
          win.setVisibleRange (TimeSpan{_t(12), FSecs(16)});
          CHECK (win.visible()     == TimeSpan(_t(12), _t(12+16)));
          CHECK (win.overallSpan() == TimeSpan(_t(0), _t(64)));
          CHECK (win.px_per_sec()  == 1280_r/16);
          CHECK (win.pxWidth()     == 1280);
          
          win.setVisiblePos(_t(12));                                             // bring a specific position into sight
          CHECK (win.visible().start() < _t(12));                                // window is placed such as to enclose this desired position
          CHECK (win.visible().duration() == _t(16));                            // window size and metric not changed
          CHECK (win.overallSpan() == TimeSpan(_t(0), _t(64)));
          CHECK (win.px_per_sec()  == 1280_r/16);
          CHECK (win.pxWidth()     == 1280);
          
          win.setVisiblePos(0.80);                                               // positioning relatively within overall canvas
          CHECK (win.visible().start() < Time{FSecs(64)*8/10});                  // window will enclose the desired anchor position
          CHECK (win.visible().end()   > Time{FSecs(64)*8/10});
          CHECK (win.px_per_sec()  == 1280_r/16);
          CHECK (win.pxWidth()     == 1280);
          
          // manipulate canvas extension explicitly
          win.setOverallDuration (Duration{FSecs(3600)});
          CHECK (win.overallSpan() == TimeSpan(_t(0), _t(3600)));
          CHECK (win.px_per_sec()  == 1280_r/16);
          CHECK (win.pxWidth()     == 1280);
          CHECK (win.visible().duration() == _t(16));                            // window position and size not affected
          CHECK (win.visible().start() < Time{FSecs(64)*8/10});
          CHECK (win.visible().end()   > Time{FSecs(64)*8/10});
          
          // reposition nominal canvas anchoring
          win.setOverallRange (TimeSpan{_t(-64), _t(-32)});
          CHECK (win.overallSpan() == TimeSpan(_t(-64), FSecs(32)));             // canvas nominally covers a completely different time range now
          CHECK (win.px_per_sec()  == 1280_r/16);                                // metric is retained
          CHECK (win.pxWidth()     == 1280);
          CHECK (win.visible()     == TimeSpan(_t(-32-16), FSecs(16)));          // window scrolled left to remain within canvas
          
          win.setOverallStart (_t(100));
          CHECK (win.overallSpan() == TimeSpan(_t(100), FSecs(32)));
          CHECK (win.visible()     == TimeSpan(_t(100), FSecs(16)));             // window scrolled right to remain within canvas
          CHECK (win.px_per_sec()  == 1280_r/16);                                // metric is retained
          
          win.setOverallRange (TimeSpan{_t(50), _t(52)});
          CHECK (win.overallSpan() == TimeSpan(_t(100), FSecs(2)));
          CHECK (win.visible()     == TimeSpan(_t(100), FSecs(16)));             // window truncated to fit into canvas
          CHECK (win.px_per_sec()  == 1280_r/2);                                 // metric need to be adjusted
          CHECK (win.pxWidth()     == 1280);
        }
      
      
      /** @test sliding the visible window, possibly expanding canvas */
      void
      verify_scroll()
        {
          ZoomWindow win{1280, TimeSpan{_t(0), FSecs(16)}};
          CHECK (win.visible() == win.overallSpan());
          CHECK (win.visible()     == TimeSpan(_t(0), FSecs(16)));
          CHECK (win.px_per_sec()  == 80);
          
          win.nudgeVisiblePos(+1);
          CHECK (win.visible()     == TimeSpan(_t(8), FSecs(16)));               // window shifted forward by half a page
          CHECK (win.overallSpan() == TimeSpan(_t(0), FSecs(16+8)));             // canvas expanded accordingly
          CHECK (win.px_per_sec()  == 80);                                       // metric is retained
          CHECK (win.pxWidth()     == 1280);
          
          win.nudgeVisiblePos(-3);
          CHECK (win.visible()     == TimeSpan(_t(-16), FSecs(16)));             // window shifted backwards by three times half window sizes
          CHECK (win.overallSpan() == TimeSpan(_t(-16), FSecs(16+8+32)));        // canvas is always expanded accordingly, never shrinked
          CHECK (win.px_per_sec()  == 80);                                       // metric is retained
          CHECK (win.pxWidth()     == 1280);
          
          win.setVisiblePos(0.50);
          CHECK (win.visible()     == TimeSpan(_t((56/2-16) -8), FSecs(16)));    // window positioned to centre of canvas
          
          win.setVisiblePos(-0.50);
          CHECK (win.visible()     == TimeSpan(_t(-16), FSecs(16)));             // relative positioning limited at lower bound
          win.setVisiblePos(2.34);
          CHECK (win.visible()     == TimeSpan(_t(56-16-16), FSecs(16)));        // relative positioning limited at upper bound
          win.setVisiblePos(_t(200));
          CHECK (win.visible()     == TimeSpan(_t(56-16-16), FSecs(16)));        // absolute positioning limited likewise
          win.setVisiblePos(_t(-200));
          CHECK (win.visible()     == TimeSpan(_t(-16), FSecs(16)));
          CHECK (win.px_per_sec()  == 80);                                       // metric retained
          CHECK (win.pxWidth()     == 1280);
          
          win.setVisibleRange(TimeSpan{_t(-200), FSecs(32)});                    // but explicit positioning outside of canvas is possible
          CHECK (win.overallSpan() == TimeSpan(_t(-200), _t(16+8)));             // ...and will expand canvas
          CHECK (win.visible()     == TimeSpan(_t(-200), FSecs(32)));
          CHECK (win.px_per_sec()  == 40);
          CHECK (win.pxWidth()     == 1280);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ZoomWindow_test, "unit gui");
  
  
}}} // namespace stage::model::test
