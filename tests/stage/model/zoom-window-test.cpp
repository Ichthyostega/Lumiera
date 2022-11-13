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
 ** The timeline uses the abstraction of an »Zoom Window«
 ** to define the scrolling and temporal scaling behaviour uniformly.
 ** This unit test verifies this abstracted behaviour against the spec.
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "stage/model/zoom-window.hpp"
#include "lib/format-cout.hpp"//////////////TODO
#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;
#define SHOW_EXPR(_XX_) \
    cout << "#Probe# " << STRINGIFY(_XX_) << " ? = " << _XX_ <<endl;
//////////////////////////////////////////////////////////////////////////////TODO


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
          CHECK (Rat(10,3) == 10_r/3);
          CHECK (Rat(10,3) == boost::rational<int64_t>(10,3));                   // using Rat = boost::rational<int64_t>
          CHECK (rational_cast<float> (10_r/3) == 3.3333333f);                   // rational_cast calculates division after type conversion
          
          verify_simpleUsage();
          verify_setup();
          verify_calibration();
          verify_metric();
          verify_window();
          verify_scroll();
          
          verify_changeNotification();
          
          toxic_corner_cases();
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
       *        - can be defined either the canvas duration,
       *          or an explicit extension given in pixels, or both
       *        - after construction, visible window always covers whole canvas
       *        - window extension, when given, defines the initial metric
       *        - otherwise pixel extension is derived from default metric
       */
      void
      verify_setup()
        {
          ZoomWindow win1;
          CHECK (win1.overallSpan() == TimeSpan(_t(0), FSecs(23)));
          CHECK (win1.visible()     == win1.overallSpan());
          CHECK (win1.pxWidth()     == 25*23);
          CHECK (win1.px_per_sec()  == 25);
          
          ZoomWindow win2{TimeSpan{_t(-1), _t(+1)}};
          CHECK (win2.overallSpan() == TimeSpan(_t(-1), FSecs(2)));
          CHECK (win2.visible()     == win2.overallSpan());
          CHECK (win2.pxWidth()     == 25*2);
          CHECK (win2.px_per_sec()  == 25);
          
          ZoomWindow win3{555};
          CHECK (win3.overallSpan() == TimeSpan(_t(0), FSecs(23)));
          CHECK (win3.visible()     == win3.overallSpan());
          CHECK (win3.pxWidth()     == 555);
          CHECK (win3.px_per_sec()  == 555_r/23);
          
          ZoomWindow win4{555, TimeSpan{_t(-10), _t(-5)}};
          CHECK (win4.overallSpan() == TimeSpan(-Time(0,10), FSecs(5)));
          CHECK (win4.visible()     == win4.overallSpan());
          CHECK (win4.pxWidth()     == 555);
          CHECK (win4.px_per_sec()  == 111);
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
          CHECK (win.visible()     == TimeSpan(_t(32,2), FSecs(32)));
          CHECK (win.px_per_sec()  == 40);
          CHECK (win.pxWidth()     == 1280);
          
          win.setVisiblePos(0.0);
          CHECK (win.visible()     == TimeSpan(_t(0), FSecs(32)));               // zoom window moved to left side of overall range
          
          win.nudgeMetric(+15);
          CHECK (win.overallSpan() == TimeSpan(_t(0), _t(64)));
          CHECK (win.visible()     == TimeSpan(_t(0), FSecs(32,32768) +MICRO_TICK));
          CHECK (win.visible().start() == _t(0));                                // now anchor position is at left bound
          CHECK (win.visible().end()              == TimeValue(977));            // length was rounded up to the next grid position
          CHECK (Time{FSecs(32,32768)+MICRO_TICK} == TimeValue(977));            // (preferring slightly larger window unless perfect fit)
          CHECK (Time{FSecs(32,32768)           } == TimeValue(976));
          // scale factor calculated back from actual window width
          CHECK (win.px_per_sec()  == 1280_r/977 * Time::SCALE);
          CHECK (win.pxWidth()     == 1280);
          // Note: already getting close to the time grid...
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
          CHECK (win.visible().duration() == Duration{1280*MICRO_TICK});
          CHECK (win.px_per_sec()  == ZOOM_MAX_RESOLUTION / 2);
          CHECK (win.pxWidth()     == 1280);
          CHECK (win.overallSpan() == TimeSpan(_t(0), _t(64)));
          
          win.nudgeMetric(-5);
          CHECK (win.visible().duration() == Duration{32 * 1280*MICRO_TICK});
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
          win.setMetric (5 / (1_r/44100));
          CHECK (win.pxWidth() == 1280);
          CHECK (win.px_per_sec() <= 5*44100);                                   // zoom scale was slightly reduced to match exact pixel width
          CHECK (win.px_per_sec() >= 5*44100 - 1);
          CHECK (win.visible().duration() == Duration{1280_r/(5*44100) +MICRO_TICK});
          CHECK (win.visible().duration() == Duration{1280_r/win.px_per_sec()});
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
          CHECK (win.visible().start() == _t(64,2) - _t(23,30*2));               // when zooming down from full range, zoom anchor is window centre
          CHECK (win.px_per_sec()  == 1280_r/_FSecs(_t(23,30)));                 // scale factor slightly adjusted to match exact pixel width
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
          CHECK (win.overallSpan() == TimeSpan(_t(50), FSecs(2)));
          CHECK (win.visible()     == TimeSpan(_t(50), FSecs(2)));               // window truncated to fit into canvas
          CHECK (win.px_per_sec()  == 1280_r/2);                                 // metric need to be adjusted
          CHECK (win.pxWidth()     == 1280);
        }
      
      
      /** @test sliding the visible window, possibly expanding canvas */
      void
      verify_scroll()
        {
          ZoomWindow win{1280, TimeSpan{_t(0), FSecs(16)}};
          CHECK (win.visible()     == win.overallSpan());
          CHECK (win.visible()     == TimeSpan(_t(0), FSecs(16)));
          CHECK (win.px_per_sec()  == 80);
          
          win.nudgeVisiblePos(+1);
          CHECK (win.visible()     == TimeSpan(_t(8), FSecs(16)));               // window shifted forward by half a page
          CHECK (win.overallSpan() == TimeSpan(_t(0), FSecs(16+8)));             // canvas expanded accordingly
          CHECK (win.px_per_sec()  == 80);                                       // metric is retained
          CHECK (win.pxWidth()     == 1280);
          
          win.nudgeVisiblePos(-3);
          CHECK (win.visible()     == TimeSpan(_t(-16), FSecs(16)));             // window shifted backwards by three times half window size
          CHECK (win.overallSpan() == TimeSpan(_t(-16), FSecs(16+8+16)));        // canvas is always expanded accordingly, never shrinked
          CHECK (win.px_per_sec()  == 80);                                       // metric is retained
          CHECK (win.pxWidth()     == 1280);
          
          win.setVisiblePos(0.50);
          CHECK (win.visible()     == TimeSpan(_t((40/2-16) -8), FSecs(16)));    // window positioned to centre of canvas
          
          win.setVisiblePos(-0.50);
          CHECK (win.visible()     == TimeSpan(_t(-16-40/2), FSecs(16)));        // relative positioning not limited at lower bound
          win.setVisiblePos(_t(200));                                            // absolute positioning likewise not limited
          CHECK (win.visible()     == TimeSpan(_t(200-16), FSecs(16)));          // but anchored according to relative anchor pos
          CHECK (win.px_per_sec()  == 80);                                       // metric retained
          CHECK (win.pxWidth()     == 1280);
          
          win.setVisibleRange(TimeSpan{_t(-200), FSecs(32)});                    // but explicit positioning outside of canvas is possible
          CHECK (win.overallSpan() == TimeSpan(_t(-200), _t(200)));              // ...and will expand canvas
          CHECK (win.visible()     == TimeSpan(_t(-200), FSecs(32)));
          CHECK (win.px_per_sec()  == 40);
          CHECK (win.pxWidth()     == 1280);
        }
      
      
      /** @test a notification-λ can be attached and will be triggered on each change */
      void
      verify_changeNotification()
        {
          ZoomWindow win{100, TimeSpan{_t(0), FSecs(4)}};
          CHECK (win.overallSpan() == TimeSpan(_t(0), _t(4)));
          CHECK (win.visible()     == TimeSpan(_t(0), _t(4)));
          CHECK (win.px_per_sec()  == 25);
          CHECK (win.pxWidth()     == 100);
          
          bool notified{false};
          win.nudgeMetric(+1);
          CHECK (not notified);
          CHECK (win.px_per_sec()  == 50);
          CHECK (win.visible().duration() == _t(2));
          
          win.attachChangeNotification([&](){ notified = true; });
          CHECK (not notified);
          CHECK (win.px_per_sec()  == 50);
          win.nudgeMetric(+1);
          CHECK (win.px_per_sec()  == 100);
          CHECK (notified);
          
          notified = false;
          CHECK (win.visible().start() == _t(3,2));
          win.nudgeVisiblePos(+1);
          CHECK (win.visible().start() == _t(2));
          CHECK (notified);

          notified = false;
          CHECK (win.overallSpan() == TimeSpan(_t(0), _t(4)));
          win.setOverallRange(TimeSpan(_t(-4), _t(4)));
          CHECK (win.overallSpan() == TimeSpan(_t(-4), _t(4)));
          CHECK (notified);
          
          notified = false;
          CHECK (win.pxWidth()     == 100);
          win.calibrateExtension(200);
          CHECK (win.pxWidth()     == 200);
          CHECK (win.px_per_sec()  == 100);
          CHECK (notified);
          
          notified = false;
          bool otherTriger{false};
          ZoomWindow wuz{10, TimeSpan{_t(0), FSecs(1)}};
          wuz.attachChangeNotification([&](){ otherTriger = true; });
          CHECK (wuz.visible().start() == _t(0));
          CHECK (not notified);
          CHECK (not otherTriger);
          wuz.nudgeVisiblePos(-1);
          CHECK (not notified);
          CHECK (otherTriger);
          CHECK (wuz.visible().start() == _t(-1,2));
          
          otherTriger = false;
          CHECK (not notified);
          win.nudgeMetric(+1);
          CHECK (not otherTriger);
          CHECK (notified);
          CHECK (win.px_per_sec()  == 200);
          CHECK (wuz.px_per_sec()  == 10);
          
          notified = false;
          otherTriger = false;
          win.detachChangeNotification();
          win.nudgeMetric(+1);
          CHECK (not notified);
          CHECK (win.px_per_sec()  == 400);
          
          wuz.nudgeMetric(+1);
          CHECK (not notified);
          CHECK (otherTriger);
          CHECK (win.px_per_sec()  == 400);
          CHECK (wuz.px_per_sec()  == 20);
        }
      
      
      /** @test verify safeguards when used in extreme corner cases */
      void
      toxic_corner_cases()
        {
          {
            ZoomWindow win{0, TimeSpan{_t(0), FSecs(0)}};
            CHECK (win.visible()     == TimeSpan(_t(0), _t(23)));                // uses DEFAULT_CANVAS instead of empty TimeSpan
            CHECK (win.px_per_sec()  == 25);                                     // falls back on default initial zoom factor
            CHECK (win.pxWidth()     == 575);                                    // allocates pixels in accordance to default
            
            win.setOverallDuration(Duration{FSecs(50)});
            win.setVisibleDuration(Duration{FSecs(0)});
            CHECK (win.overallSpan() == TimeSpan(_t(0), _t(50)));
            CHECK (win.visible()     == TimeSpan(_t(0), _t(23)));                // falls back to DEFAULT_CANVAS size
            CHECK (win.pxWidth()     == 575);                                    // allocates pixels in accordance to default
            
            win.calibrateExtension(0);
            CHECK (win.px_per_sec()  == 25);                                     // stays at default zoom factor
            CHECK (win.pxWidth()     == 1);                                      // retains 1px window size
            CHECK (win.visible().duration() == _t(1,25));                        // visible window has thus 1/25s duration
            
            win.setOverallRange(TimeSpan(_t(10), _t(0)));
            SHOW_EXPR (win.overallSpan());
            SHOW_EXPR (win.visible());
            SHOW_EXPR (_raw(win.visible().duration()));
            SHOW_EXPR (win.px_per_sec());
            SHOW_EXPR (win.pxWidth());
            
            CHECK (TimeSpan(_t(10), _t(0)).duration() == Duration(FSecs(10)));    // TimeSpan is always properly oriented by construction
            Duration evilReversed = static_cast<Duration> (_t(-10));
            SHOW_EXPR (evilReversed);
            SHOW_EXPR (TimeSpan(_t(20), evilReversed));

          }
          {
            Rat poison{_raw(Time::MAX)-101010101010101010, _raw(Time::MAX)+23};
            SHOW_EXPR(poison);
            Rat sane = util::reQuant (poison, _raw(Time::MAX) / Time::SCALE);
            SHOW_EXPR(sane);
            SHOW_EXPR(rational_cast<double>(poison));
            SHOW_EXPR(rational_cast<double>(sane));
            ZoomWindow win{};
//            SHOW_EXPR(win.overallSpan());
//            SHOW_EXPR(_raw(win.visible().duration()));
//            SHOW_EXPR(win.px_per_sec());
//            SHOW_EXPR(win.pxWidth());
//            CHECK (win.visible()     == TimeSpan(_t(0), _t(23)));
//            CHECK (win.px_per_sec()  == 25);
//            CHECK (win.pxWidth()     == 575);
          }
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ZoomWindow_test, "unit gui");
  
  
}}} // namespace stage::model::test
