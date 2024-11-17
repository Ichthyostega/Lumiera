/*
  ZoomWindow(Test)  -  verify translation from model to screen coordinates

   Copyright (C)
     2018,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file zoom-window-test.cpp
 ** unit test \ref ZoomWindow_test
 ** The timeline uses the abstraction of an »Zoom Window«
 ** to define the scrolling and temporal scaling behaviour uniformly.
 ** This unit test verifies this abstracted behaviour against the spec.
 ** 
 ** # Fractional Seconds
 ** 
 ** A defining trait of the ZoomWindow implementation — as it stands 12/2022 — is the use
 ** of integer fractions for most scale and time interval calculations. The typical media
 ** handling operations often rely on denomination into a divisor defined scale — be it
 ** seconds divided by frame count (25fps), or be it audio samples like 1/96000 sec.
 ** And for presentation in the UI, these uneven fractions need to be broken down into
 ** a fixed pixel count, while the zoom factor can vary over several orders of magnitude.
 ** Integer fractions are a technically brilliant solution to cope with this challenge,
 ** without rounding discrepancies and accumulation of errors.
 ** 
 ** However, there is a catch: The way fractional arithmetics are handled leads to lots
 ** of multiplications, with the tendency to build up very large irreducible numbers, both in
 ** numerator and denominator. In worst case, numeric wrap-around can happen even at seemingly
 ** innocuous places. In an attempt to maintain the benefits of integer fraction arithmetics,
 ** for ZoomWindow a set of »coping strategies« was developed, to detect and control the cases
 ** when numbers „go south“. This approach is based on the observation that almost all
 ** everyday time calculations happen within a rather limited domain, while the extended
 ** time domain of years and centuries rather serves as a theoretical headroom. Thus it
 ** seems reasonable to benefit from integer fractions within this everyday range, under
 ** the condition that computations can be kept from derailing totally, when entering
 ** the extended domain.
 ** 
 ** To this end, we use the trick of introducing a minute numeric error, by re-quantising
 ** huge numbers into a scale with a smaller denominator. We introduce the notion of »toxic«
 ** numbers, which are defined by figures above 2^40 — irrespective if in numerator or in
 ** denominator. This rather arbitrary choice is based on the observation that most
 ** computation paths require to multiply with Time::SCALE (the µ-tick scale of 10^6),
 ** which together with 2^40 just fits into the value range of int64_t. Thus, into all
 ** crucial computation paths, a function `detox()` is wired, which remains inactive for
 ** regular values, but automatically _sanitises extreme values._ Together with the
 ** safety headroom built into the limits of the Lumiera lib::time::Time domain,
 ** this allows to handle all valid time points and represent even the largest
 ** possible lib::time::Duration::MAX.
 ** 
 ** A major part of this test is dedicated to covering those hypothetical corner cases
 ** and to ensure the defined behaviour can be maintained even under extreme conditions.
 ** 
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
   * @remark the `safeguard_*` tests focus on the boundary cases.
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
          
          safeguard_zero_init();
          safeguard_reversed_intervals();
          safeguard_toxic_zoomFactor();
          safeguard_poisonousMetric();
          safeguard_extremeZoomOut();
          safeguard_extremeTimePos();
          safeguard_extremeOffset();
          safeguard_verySmall();
          safeguard_veryDeep();
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
       *        - can be defined either with the canvas duration,
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
      
      
      /** @test verify defining and retaining the effective extension in pixels
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
          CHECK (TimeValue{640} == Time{Rat(1280)/ZOOM_MAX_RESOLUTION});
          
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
          CHECK (win.visible().start() ==      _t(-4));                          // (canvas was already positioned asymmetrically)
          
          win.setVisiblePos(-0.50);
          CHECK (win.visible()     == TimeSpan(_t(-16-40/2), FSecs(16)));        // relative positioning not limited at lower bound
          CHECK (win.visible().start() ==      _t(-36));                         //   (causing also further expansion of canvas) 
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
      
      
      /** @test verify safeguards against empty initialisation interval */
      void
      safeguard_zero_init()
        {
          ZoomWindow win{0, TimeSpan{_t(0), FSecs(0)}};
          CHECK (win.visible()     == TimeSpan(_t(0), _t(23)));                  // uses DEFAULT_CANVAS instead of empty TimeSpan
          CHECK (win.px_per_sec()  == 25);                                       // falls back on default initial zoom factor
          CHECK (win.pxWidth()     == 575);                                      // allocates pixels in accordance to default
          
          win.setOverallDuration(Duration{FSecs(50)});
          win.setVisibleDuration(Duration{FSecs(0)});
          CHECK (win.overallSpan() == TimeSpan(_t(0), _t(50)));
          CHECK (win.visible()     == TimeSpan(_t(0), _t(23)));                  // falls back to DEFAULT_CANVAS size
          CHECK (win.pxWidth()     == 575);                                      // allocates pixels in accordance to default
          
          win.calibrateExtension(0);
          CHECK (win.px_per_sec()  == 25);                                       // stays at default zoom factor
          CHECK (win.pxWidth()     == 1);                                        // retains 1px window size
          CHECK (win.visible().duration() == _t(1,25));                          // visible window has thus 1/25s duration
        }
      
      
      /** @test verify safeguards against reversed time intervals */
      void
      safeguard_reversed_intervals()
        {
          ZoomWindow win{1};
          win.setVisibleDuration(Duration{FSecs(1,25)});
          win.setOverallRange(TimeSpan(_t(10), _t(0)));                          // set an "reversed" overall time range
          CHECK (win.overallSpan() == TimeSpan(_t(0), _t(10)));                  // range has been re-oriented forward
          CHECK (win.visible().duration() == Time(40,0));
          CHECK (win.px_per_sec() == 25);
          CHECK (win.pxWidth() == 1);
          
          CHECK (TimeSpan(_t(10), _t(0)).duration() == Duration(FSecs(10)));     // TimeSpan is always properly oriented by construction
        }
      
      
      /** @test demonstrate sanitising of "poisonous" fractional zoom factor
       *        - construct an example factor of roughly 2/3, but using extremely large
       *          numerator and denominator close to total time axis dimensions.
       *        - even simple calculations with this poison value will fail
       *        - construct a new quantiser, based on the number to be sanitised
       *        - re-quantise the toxic number into this new quantiser
       *        - the sanitised number is almost identical to the toxic original
       *        - yet all the simple calculations can be carried out flawlessly
       *        - both toxic and sanitised number lead to the same zoom timespan
       */
      void
      safeguard_toxic_zoomFactor()
        {
          Rat poison{_raw(Time::MAX)-101010101010101010, _raw(Time::MAX)+23};
          CHECK (poison == 206435633551724850_r/307445734561825883);
          CHECK (2_r/3 < poison and poison < 1);                                 // looks innocuous...
          CHECK (poison + Time::SCALE < 0);                                      // simple calculations fail due to numeric overflow
          CHECK (poison * Time::SCALE < 0);
          CHECK (-6 == rational_cast<gavl_time_t>(poison * Time::SCALE));        // naive conversion to µ-ticks would lead to overflow
          CHECK (671453 == _raw(Time(FSecs(poison))));                           // however the actual conversion routine is safeguarded
          CHECK (671453.812f == rational_cast<float>(poison)*Time::SCALE);
          
          using util::ilog2;
          CHECK (40 == ilog2 (LIM_HAZARD));                                      // LIM_HAZARD is based on MAX_INT / Time::Scale
          CHECK (57 == ilog2 (poison.numerator()));                              // use the leading bit position as size indicator
          CHECK (58 == ilog2 (poison.denominator()));                            // use the maximum of numerator or denominator bit position
          CHECK (58-40 == 18);                                                   // everything beyond LIM_HAZARD counts as "toxic"
          
          int toxicity = toxicDegree (poison);
          CHECK (toxicity == 18);
          int64_t quant = poison.denominator() >> toxicity;                      // shift away the excess toxic LSB
          CHECK (quant == 1172812402961);
          CHECK (ilog2 (quant) == ilog2 (LIM_HAZARD));
          Rat detoxed = util::reQuant(poison, quant);                            // and use this "shortened" denominator for re-quantisation
          CHECK (detoxed == 787489446837_r/1172812402961);                       // the resulting fraction uses way smaller numbers
          CHECK (0.671453834f == rational_cast<float> (poison));                 // but yields approximately the same effective value
          CHECK (0.671453834f == rational_cast<float> (detoxed));
          
          CHECK (detoxed+Time::SCALE == 1172813190450446837_r/1172812402961);    // result: usual calculations without failure
          CHECK (Time(FSecs(detoxed)) > Time::ZERO);                             // can convert re-quantised number to µ-ticks
          CHECK (671453 == _raw(Time(FSecs(detoxed))));
                                                                                 // and resulting µ-ticks will be effectively the same
          CHECK (1906 == _raw(TimeValue(1280 / rational_cast<long double>(poison))));
          CHECK (1906 == _raw(TimeValue(1280 / rational_cast<long double>(detoxed))));
        }
      
      
      /** @test verify ZoomWindow code can handle "poisonous" fractional number parameters
       *        - toxic zoom factor passed through ZoomWindow::setMetric(Rat)
       *        - toxic proportion factor passed to ZoomWindow::setVisiblePos(Rat)
       *        - indirectly cause toxic posFactor in ZoomWindow::anchorWindowAtPosition(FSecs)
       *          by providing a target position very far off current window location
       */
      void
      safeguard_poisonousMetric()
        {
          ZoomWindow win{};
          CHECK (win.visible() == win.overallSpan());                            // by default window spans complete canvas
          CHECK (win.visible().duration() == _t(23));                            // ...and has just some handsome extension
          CHECK (win.px_per_sec() == 25);
          CHECK (win.pxWidth() == 575);

          Rat poison{_raw(Time::MAX)-101010101010101010, _raw(Time::MAX)+23};
          CHECK (0 < poison and poison < 1);
          
          /*--Test-1-----------*/
          win.setMetric (poison);                                                // inject an evil new value for the metric
          CHECK (win.visible() == win.overallSpan());                            // however, nothing happens
          CHECK (win.visible().duration() == _t(23));                            // since the window is confined to overall canvas size
          CHECK (win.visible() == TimeSpan(_t(0), _t(23)));                      // Note: this calculation is fail-safe
          CHECK (win.px_per_sec() == 25);
          CHECK (win.pxWidth() == 575);

          win.setOverallDuration(Duration(Time::MAX));                           // second test: expand canvas to allow for actual adjustment
          CHECK (win.overallSpan().duration() == TimeValue{307445734561825860}); // now canvas has ample size (half the possible maximum size)
          CHECK (win.overallSpan().duration() == Time::MAX);
          CHECK (win.visible().duration() == _t(23));                            // while the visible part remains unaltered

          /*--Test-2-----------*/
          win.setMetric (poison);                                                // Now attempt again to poison the zoom calculations...
          CHECK (win.overallSpan().duration() == Time::MAX);                     // overall canvas unchanged
          CHECK (win.visible().duration() == TimeValue{856350691});              // visible window expanded (a zoom-out, as required)
          CHECK (win.px_per_sec() == Rat{win.pxWidth()} / _FSecs(win.visible().duration()));
          float approxPoison = rational_cast<float> (poison);                    // the provided (poisonous) metric factor...
          CHECK (approxPoison == 0.671453834f);                                  // ...is approximately the same...
          float approxNewMetric = rational_cast<float> (win.px_per_sec());       // ...as the actual new metric factor we got
          CHECK (approxNewMetric == 0.671453893f);
          CHECK (win.px_per_sec() != poison);                                    // but it is not exactly the same
          CHECK (win.px_per_sec()  < poison);                                    // rather, it is biased towards slightly smaller values
          
          Rat poisonousDuration = win.pxWidth() / poison;                        // Now, to demonstrate this "poison" was actually dangerous
          CHECK (poisonousDuration == 7071251894921995309_r/8257425342068994);   // ...when we attempt to calculate the new duration directly....
          CHECK (poisonousDuration * Time::SCALE < 0);                           // ...then a conversion to TimeValue will cause integer wrap
          CHECK(856.350708f == rational_cast<float> (poisonousDuration));        // yet numerically the duration actually established is almost the same
          CHECK(856.350708f == rational_cast<float> (_FSecs(win.visible().duration())));
          CHECK (win.px_per_sec() == 575000000_r/856350691);                     // the new metric however is comprised of sanitised fractional numbers
          CHECK (win.pxWidth() == 575);                                          // and the existing pixel width was not changed
          
          CHECK (win.overallSpan().start()    == Time::ZERO);
          CHECK (win.overallSpan().duration() == TimeValue{307445734561825860});
          CHECK (win.visible().duration()     == TimeValue{856350691});

          /*--Test-3-----------*/
          win.setVisiblePos (poison);                                            // Yet another way to sneak in our toxic value...
          CHECK (win.overallSpan().start()    == Time::ZERO);
          CHECK (win.overallSpan().duration() == TimeValue{307445734561825860}); // However, all base values turn out unaffected
          CHECK (win.visible().duration()     == TimeValue{856350691});
          
          TimeValue targetPos{gavl_time_t(_raw(win.overallSpan().duration())     // based on the overall span...
                                          * rational_cast<double> (poison))};    // the given toxic factor would point at that target position
          
          CHECK (targetPos             == TimeValue{206435633551724864});
          CHECK (win.visible().start() == TimeValue{206435633106265625});        // the visible window has been moved to enclose this target
          CHECK (win.visible().end()   == TimeValue{206435633962616316});
          CHECK (win.visible().start() < targetPos);
          CHECK (win.visible().end()   > targetPos);
          
          CHECK (win.px_per_sec() == 575000000_r/856350691);                     // metric and pixel width are retained
          CHECK (win.pxWidth() == 575);
          
          
          win.setOverallRange(TimeSpan{Time::MAX, Offset{TimeValue(23)}});       // preparation for Test-4 : shift canvas to end of time
          CHECK (win.overallSpan() == win.visible());                            // consequence: window has been capped to canvas size
          CHECK (win.overallSpan().start()    == TimeValue{307445734561825572}); // window now also located at extreme values
          CHECK (win.overallSpan().end()      == TimeValue{307445734561825860});
          CHECK (win.overallSpan().duration() == TimeValue{288});                // window (and canvas) were expanded to comply to maximum zoom factor
          CHECK (win.px_per_sec() == 17968750_r/9);                              // zoom factor was then slightly reduced to match next pixel boundary
          CHECK (win.pxWidth() == 575);                                          // established pixel size was retained
          
          /*--Test-4-----------*/
          win.setVisiblePos(Time{Time::MIN + TimeValue(13)});                    // Test: implicitly provoke poisonous factor through extreme offset
          CHECK (win.visible().start() == Time::MIN + TimeValue(13));            // even while this position is far off, window start was aligned to it
          CHECK (win.visible().end()   == win.visible().start() + TimeValue{288});
          CHECK (win.visible().duration() == TimeValue{288});
          
          CHECK (win.overallSpan().start() == win.visible().start());            // canvas start at window start
          CHECK (win.overallSpan().end()   == TimeValue{307445734561825860});    // canvas end not changed
          CHECK (_raw(win.overallSpan().duration()) == 614891469123651707);      // canvas size was expanded to encompass changed window position
          CHECK (win.px_per_sec() == 17968750_r/9);                              // zoom factor not changed
          CHECK (win.pxWidth() == 575);                                          // established pixel size retained
        }
      
      
      /** @test verify ZoomWindow code can handle extreme zoom-out
       *        to reveal a timeline of epic dimensions....
       */
      void
      safeguard_extremeZoomOut()
        {
          /*--Test-1-----------*/
          ZoomWindow win{3, TimeSpan{_t(-1,2), _t(1,2)}};                        // setup ZoomWindow to very small pixel size (3px)
          CHECK (win.overallSpan().duration() == _t(1));
          CHECK (win.px_per_sec() == 3_r/1);
          CHECK (win.pxWidth() == 3);
          win.setOverallRange(TimeSpan{Time::MIN, Time::MAX});                   // ...and then also expand canvas to maximal size
          CHECK (_raw(win.overallSpan().duration()) == 614891469123651720);
          CHECK (_raw(win.visible().duration())     ==            1000000);
          CHECK (win.px_per_sec() == 3_r/1);
          CHECK (win.pxWidth() == 3);
          
          /*--Test-2-----------*/
          Rat bruteZoom{3_r/(int64_t{1}<<60)};
          win.setMetric (bruteZoom);                                             // zoom out beyond what is possible and to a toxic factor
          
          CHECK (_raw(win.overallSpan().duration()) == 614891469123651720);      // canvas size not changed
          CHECK (_raw(win.visible().duration())     ==   3298534883328000);      // window was expanded,
          CHECK (_raw(win.visible().duration())     <     int64_t{1}<<60 );      // ...but not as much as demanded
          CHECK (_raw(win.visible().duration())     == 3* LIM_HAZARD*1000);      // In fact it was capped at a built-in limit based on pixel size,
                                                                                 // to prevent formation of dangerous numbers within metric calculations
          CHECK (win.visible().start() == - win.visible().end());                // window has been expanded symmetrically to existing position
          CHECK (win.px_per_sec()  > bruteZoom);                                 // the actual zoom factor also reflects the applied limitation,
          CHECK (win.px_per_sec() == 125_r/137438953472);                        // to ensure the denominator does not exceed LIM_HAZARD
          CHECK (win.px_per_sec() == 1000_r/LIM_HAZARD);
          CHECK (win.px_per_sec() == 3 / _FSecs(win.visible().duration()));      // and this value also conforms with the pixel size and window duration
          CHECK (win.pxWidth() == 3);
          
          /*--Test-3-----------*/
          win.setMetric (5_r/std::numeric_limits<int64_t>::max());               // same limiting applies to even more nasty values
          CHECK (_raw(win.visible().duration()) == 3298534883328000);            // still unchanged at limit
          CHECK (win.px_per_sec() == 125_r/137438953472);
          CHECK (win.pxWidth() == 3);
          
          /*--Test-4-----------*/
          win.setMetric (1001_r/LIM_HAZARD);                                     // but zooming in more than that limit will be honored
          CHECK (_raw(win.visible().duration()) == 3295239643684316);            // ...window now slightly reduced in size
          CHECK (_raw(win.visible().duration())  < 3 * LIM_HAZARD*1000);
          CHECK (win.px_per_sec()  > 1000_r/LIM_HAZARD);
          CHECK (win.px_per_sec() == 1001_r/LIM_HAZARD);                         // (this is what was requested)
          CHECK (win.px_per_sec() == 1001_r/1099511627776);
          CHECK (win.pxWidth() == 3);

          /*--Test-5-----------*/
          win.setMetric (1000_r/LIM_HAZARD * 1024_r/1023);                       // likewise zooming back out slightly below limit is possible
          CHECK (_raw(win.visible().duration()) == 3295313657856000);            // ...window now again slightly increased, but not at maximum size
          CHECK (_raw(win.visible().duration())  < 3 * LIM_HAZARD*1000);
          CHECK (win.px_per_sec() > 1000_r/LIM_HAZARD);
          CHECK (win.px_per_sec() < 1001_r/LIM_HAZARD);
          CHECK (win.px_per_sec() == 1000_r/LIM_HAZARD * 1024_r/1023);           // zoom factor precisely reproduced in this case
          CHECK (win.px_per_sec() == 125_r/137304735744);
          CHECK (win.pxWidth() == 3);

          /*--Test-6-----------*/
          win.setMetric (1001_r/(LIM_HAZARD-3));                                 // however, setting »poisonous« factors close below the limit...
          CHECK (win.px_per_sec() > 1001_r/LIM_HAZARD);                          // results in a sanitised (simplified) zoom factor
          CHECK (win.px_per_sec() < 1002_r/LIM_HAZARD);
          CHECK (1001_r/(LIM_HAZARD-3) == 77_r/84577817521);                     // This case is especially interesting, since the initial factor isn't »toxic«,
                                                                                 // but the resulting duration is not µ-grid aligned, and after fixing that,
          CHECK (3_r/3295239643675325 * Time::SCALE == 120000_r/131809585747013);// the resulting zoom factor is comprised of very large numbers,
          CHECK (win.px_per_sec() == 2003_r/2199023255552);                      // ...which are then simplified and adjusted...
          CHECK (win.pxWidth() == 3);                                            // ... to match also the pixel size
         
          CHECK (_raw(Duration{3_r/(77_r/84577817521)}) == 3295239643675324);    // This is the duration we'd expect (truncated down)
          CHECK (_raw(win.visible().duration())         == 3295239643675325);    // ...this is the duration we actually get
          CHECK (_raw(Duration{3_r/win.px_per_sec()})   == 3293594491590614);    // Unfortunately, calculating back from the smoothed zoom-metric
                                                                                 // .. would yield a duration way off, with an relative error < 1‰
          CHECK (2003.0f/2002 - 1 == 0.000499486923f);                           // The reason for this relative error is the small numerator of 2002
                                                                                 // (2002 is increased to 2003 to get above 3px)
          
          /*--Test-7-----------*/
          win.calibrateExtension (1'000'000'000);                                // implicit drastic zoom-out by increasing the number of pixels
          CHECK (win.pxWidth()  < 1'000'000'000);                                // however: this number is capped at a fixed maximum
          CHECK (win.pxWidth() == MAX_PX_WIDTH);                                 // (which „should be enough“ for the time being...)
          CHECK (win.px_per_sec() == 89407_r/549755813888);                      // the zoom metric has been adapted, but to a sanitised value
          CHECK (win.px_per_sec() > Rat{MAX_PX_WIDTH}  /MAX_TIMESPAN);
          CHECK (win.px_per_sec() < Rat{MAX_PX_WIDTH+1}/MAX_TIMESPAN);
          
          CHECK (_raw(win.overallSpan().duration()) ==  614891469123651720);     // overall canvas duration not changed
          CHECK (_raw(win.visible().duration())     ==  614891469123651720);     // window duration now expanded to the maximum possible value
          CHECK (win.overallSpan().end()  == TimeValue{ 307445734561825860});    // window now spans the complete time domain
          CHECK (win.visible().end()      == TimeValue{ 307445734561825860});
          CHECK (win.visible().start()    == TimeValue{-307445734561825860});

                                                                                 // Note: these parameters build up to really »poisonous« values....
          CHECK (MAX_PX_WIDTH / _FSecs(win.visible().duration()) == 2500000000_r/15372286728091293);
          CHECK (MAX_PX_WIDTH * 1000000_r/614891469123651720     == 2500000000_r/15372286728091293);
          CHECK (win.px_per_sec() * _FSecs(win.visible().duration()) < 0);       // we can't even calculate the resulting pxWidth() naively
          CHECK (rational_cast<float>(win.px_per_sec())                          // ...while effectively these values are still correct
                * rational_cast<float>(_FSecs(win.visible().duration())) == 100000.031f);
          CHECK (rational_cast<float>(MAX_PX_WIDTH*1000000_r/614891469123651720) == 1.62630329e-07f);  // theoretical value
          CHECK (rational_cast<float>(win.px_per_sec())                          == 1.62630386e-07f);  // value actually chosen
          CHECK (win.px_per_sec() == 89407_r/549755813888);
          
          /*--Test-8-----------*/
          win.setMetric (bruteZoom);                                             // And now put one on top by requesting excessive zoom-out!
          CHECK (_raw(win.overallSpan().duration()) == 614891469123651720);      // overall canvas duration not changed
          CHECK (_raw(win.visible().duration())     == 614891469123651720);      // window duration was capped precisely at DURATION_MAX
          CHECK (win.px_per_sec() == 89407_r/549755813888);                      // zoom factor and now hitting again the minimum limit
          CHECK (MAX_PX_WIDTH /(614891469123651720_r/Time::SCALE) == 2500000000_r/15372286728091293);   // (this would be the exact factor)
          CHECK (2500000000_r/15372286728091293 < 89407_r/549755813888);         // zoom factor (again) numerically sanitised
          CHECK (win.pxWidth() == MAX_PX_WIDTH);                                 // pixel count unchanged at maximum
        }
      
      
      /** @test verify ZoomWindow code can navigate extremal time positions,
       *        thereby observing domain bounds without numeric wrap.
       */
      void
      safeguard_extremeTimePos()
        {
          /*--Test-1-----------*/
          ZoomWindow win{559, TimeSpan{Time::MAX, Duration{TimeValue(3)}}};      // setup a very small window clinging to Time::MAX
          CHECK (win.visible().duration() == TimeValue(280));                    // duration expanded due to MAX_ZOOM limit
          CHECK (win.visible().start() == TimeValue(307445734561825580));        // and properly oriented and aligned within domain
          CHECK (win.visible().end()   == TimeValue(307445734561825860));
          CHECK (win.visible().end()   == Time::MAX);
          CHECK (win.visible() == win.overallSpan());
          CHECK (win.px_per_sec() == 559_r/280*Time::SCALE);
          CHECK (win.px_per_sec() == 13975000_r/7);
          CHECK (win.pxWidth()    == 559);
          
          /*--Test-2-----------*/
          Time anchorPos{15_r/16 * Offset(Time::MIN)};
          win.setVisiblePos (anchorPos);                                         // scroll to a target position extremely far off
          CHECK (win.visible().duration() == TimeValue(280));                    // window dimensions retained
          CHECK (win.px_per_sec() == 13975000_r/7);
          CHECK (win.pxWidth()    == 559);
          CHECK (win.visible().start() >  Time::MIN);
          CHECK (win.visible().start() == anchorPos);                            // window now at desired position
          CHECK (win.visible().end()   >  anchorPos);
          CHECK (win.visible().start() == TimeValue(-288230376151711744));
          CHECK (win.visible().end()   == TimeValue(-288230376151711464));
          CHECK (win.overallSpan().start() == win.visible().start());            // canvas expanded accordingly
          CHECK (win.overallSpan().end()   == Time::MAX);
          
          /*--Test-3-----------*/
          win.calibrateExtension (560);
          CHECK (win.visible().duration() == TimeValue(280));                    // effective window dimensions unchanged
          CHECK (win.px_per_sec() == 2000000_r/1);                               // but zoom metric slightly adapted
          
          win.setOverallDuration (Duration::MAX);                                // now use maximally expanded canvas
          Duration targetDur{Duration::MAX - FSecs(23)};
          win.setVisibleDuration(targetDur);                                     // and demand the duration be expanded almost full size
          
          CHECK (win.visible().duration() == targetDur);                         // actual duration is the value requested
          CHECK (win.visible().duration() < Duration::MAX);
          CHECK (win.visible().start() == Time::MIN);                            // expansion was anchored at previous position
          CHECK (win.visible().start() <  Time::MAX);                            // and thus the window now clings to the lower end
          CHECK (win.visible().end()   == TimeValue(307445734538825860));
          CHECK (Time::MAX - win.visible().end() == TimeValue(23*Time::SCALE));
          CHECK (win.px_per_sec() == 2003_r/2199023255552);                      // effective zoom metric has been sanitised numerically
          CHECK (win.pxWidth()    == 560);                                       // but pixel count is matched precisely
          
          /*--Test-4-----------*/
          win.setVisiblePos (Rat{std::numeric_limits<int64_t>::max()-23});
          CHECK (win.visible().duration() == targetDur);                         // actual duration unchanged
          CHECK (win.px_per_sec() == 2003_r/2199023255552);
          CHECK (win.pxWidth()    == 560);
          CHECK (win.visible().end()   == Time::MAX);                            // but window now slinged to the right extreme
          CHECK (win.visible().start() >  Time::MIN);
          CHECK (win.visible().start() == TimeValue(-307445734538825860));
          
          /*--Test-5-----------*/
          win.calibrateExtension (561);                                          // expand by 1 pixel
          CHECK (win.visible().duration() >  targetDur);                         // actual duration indeed increased
          CHECK (win.visible().duration() == Duration::MAX);                     // and then capped at maximum
          CHECK (win.visible().end()   == Time::MAX);                            // but while initially the upper bound is increased...
          CHECK (win.visible().start() == Time::MIN);
          CHECK (win.px_per_sec() == 2007_r/2199023255552);                      // the smoothed nominal metric was also increased slightly
          CHECK (win.pxWidth()    == 561);
          
          /*--Test-6-----------*/
          win.setVisibleDuration (Duration::MAX - Duration(TimeValue(1)));       // request slightly different window duration
          CHECK (win.visible().end()   == Time::MAX);                            // by arbitrary choice, the single µ-tick was removed at start
          CHECK (win.visible().start() == Time::MIN + TimeValue(1));
          CHECK (win.px_per_sec() == 2007_r/2199023255552);                      // the smoothed nominal metric was also increased slightly
          CHECK (win.pxWidth()    == 561);
          
          win.setVisibleDuration (Duration(TimeValue(1)));                       // drastically zoom-in
          CHECK (win.visible().duration() == TimeValue(281));                    // ...but we get more than 1 µ-tick
          CHECK (561_r/_FSecs(TimeValue(1)) > ZOOM_MAX_RESOLUTION);              // because the requested window would exceed maximum zoom
          CHECK (win.px_per_sec() == 561000000_r/281);                           // and this conflict was resolved by increasing the window
          CHECK (win.visible().end()   == Time::MAX);                            // while keeping it aligned to the end of the timeline
          CHECK (win.pxWidth()    == 561);
        }
      
      
      /** @test verify ZoomWindow code is protected against excess scrolling.
       */
      void
      safeguard_extremeOffset()
        {
          ZoomWindow win{ 1, TimeSpan{Time::MAX, Duration{TimeValue(1)}}};       // use window of 1px size zoomed at 1 µ-tick
          CHECK (win.visible().start() == Time::MAX - TimeValue(1));             // which is aligned to the end of the time domain
          CHECK (win.visible().duration() == TimeValue(1));
          
          win.nudgeVisiblePos (-2);                                              // can be nudged by one window size to the left
          CHECK (win.visible().start() == Time::MAX - TimeValue(2));
          
          win.offsetVisiblePos (Offset{Duration::MAX});                          // but excess offset is just absorbed
          CHECK (win.visible().end()   == Time::MAX);                            // window again positioned at the limit
          CHECK (win.visible().start() == Time::MAX - TimeValue(1));
          CHECK (win.visible().duration() == TimeValue(1));
          CHECK (win.overallSpan().duration() == TimeValue(2));
          CHECK (win.px_per_sec() == 1000000);
          CHECK (win.pxWidth()    == 1);
          
          win.nudgeVisiblePos (std::numeric_limits<int64_t>::min());             // excess nudging likewise absorbed
          CHECK (win.overallSpan().duration() == Duration::MAX);
          CHECK (win.visible().duration() == TimeValue(1));
          CHECK (win.visible().start() == Time::MIN);                            // window now positioned at lower limit
          CHECK (win.visible().end()   == Time::MIN + TimeValue(1));
          CHECK (win.px_per_sec() == 1000000);
          CHECK (win.pxWidth()    == 1);
          
          win.calibrateExtension (460);
          win.setVisibleDuration (Duration{Time::MAX - TimeValue(1)});           // arrange window to be 1 µ-tick less than half
          CHECK (win.visible().duration() == Time::MAX - TimeValue(1));
          CHECK (win.visible().start() == Time::MIN);                            // ...so it spans [Time::MIN ... -1]
          CHECK (win.visible().end()   == TimeValue(-1));
          
          win.nudgeVisiblePos (+2);                                              // thus nudging two times by half-window size...
          CHECK (win.visible().end()   == Time::MAX - TimeValue(2));             // ...still fits into the time domain
          CHECK (win.visible().start() == TimeValue(-1));
          win.nudgeVisiblePos (-1);
          CHECK (win.visible().start() == TimeValue(-153722867280912930));       // navigation within domain works as expected
          CHECK (win.visible().end()   == TimeValue(+153722867280912929));
          
          win.nudgeVisiblePos (+1000);                                           // requesting an excessive nudge...
          CHECK (ilogb(500.0 * _raw(Time::MAX)) == 67);                          // which — naively calculated — would overflow 64-bit
          CHECK (win.visible().start() == TimeValue(+1));                        // but the window just stopped aligned to the upper limit
          CHECK (win.visible().end()   == Time::MAX);
          CHECK (win.pxWidth()         == 460);
        }
      
      
      /** @test verify ZoomWindow can handle excessively small windows.
       */
      void
      safeguard_verySmall()
        {                                                                        // for setup, request a window crossing time domain bounds
          ZoomWindow win{ 1, TimeSpan{Time::MAX - TimeValue(23), Duration::MAX}};
          CHECK (win.overallSpan().duration() == Duration::MAX);                 // we get a canvas with the requested extension Duration::MAX
          CHECK (win.overallSpan().end()      == Time::MAX);                     // but shifted into domain to fit
          CHECK (win.visible().duration() == LIM_HAZARD * 1000);                 // the visible window however is limited to be smaller
          CHECK (win.visible().start()+win.visible().end() == Time::ZERO);       // and (since this is a zoom-in) it is centred at origin
          CHECK (win.px_per_sec() == 1_r/(LIM_HAZARD*1000)*Time::SCALE);         // Zoom metric is likewise limited, to keep the numbers manageable
          CHECK (win.px_per_sec() == 125_r/137438953472);
          CHECK (win.pxWidth()    == 1);
          
          win.nudgeVisiblePos (+1);                                              // can work with this tiny window as expected
          CHECK (win.visible().start() == Time::ZERO);
          CHECK (win.visible().end()   == LIM_HAZARD*1000);
          CHECK (win.px_per_sec() == 125_r/137438953472);
          CHECK (win.pxWidth()    == 1);
          
          win.nudgeMetric (-1);                                                  // can not zoom out further
          CHECK (win.px_per_sec() == 125_r/137438953472);
          win.nudgeMetric (+1);                                                  // but can zoom in
          CHECK (win.px_per_sec() == 125_r/68719476736);
          CHECK (win.visible().start() == TimeValue(274877908523000));
          CHECK (win.visible().end()   == TimeValue(824633722411000));
          CHECK (win.visible().duration() == LIM_HAZARD * 1000 / 2);
          CHECK (win.pxWidth()    == 1);
          
          win.setVisiblePos (Time{Time::MAX - TimeValue(23)});
          CHECK (win.visible().end()   == Time::MAX);
          CHECK (win.visible().duration() == LIM_HAZARD * 1000 / 2);
          CHECK (win.px_per_sec() == 2_r/(LIM_HAZARD*1000)*Time::SCALE);
          CHECK (win.pxWidth()    == 1);
          
          win.setVisibleRange (TimeSpan{Time::MAX - TimeValue(23)                // request a window exceeding domain,
                                       ,FSecs{LIM_HAZARD, 1001}});               // but with a zoom slightly above minimal-zoom
          CHECK (win.visible().end()   == Time::MAX);                            // Resulting window is shifted into domain
          CHECK (win.visible().duration() == Duration(FSecs{LIM_HAZARD, 1001})); // and has the requested extension
          CHECK (win.visible().duration() == TimeValue(1098413214561438));
          CHECK ( FSecs(LIM_HAZARD, 1000) >  FSecs(LIM_HAZARD, 1001));           // which is indeed smaller than the maximum duration
          CHECK (win.px_per_sec() == 2003_r/2199023255552);
          CHECK (win.pxWidth()    == 1);
        }
      
      
      /** @test verify ZoomWindow and handle extreme zoom-in.
       *        - scrolling has always an effect
       *        - visible window is never void
       */
      void
      safeguard_veryDeep()
        {
          ZoomWindow win{TimeSpan{Time::MIN, Duration{TimeValue(1)}}};           // just request a window spanning the minimally possible value
          CHECK (win.overallSpan().duration() == win.visible().duration());
          CHECK (win.visible().duration() == TimeValue(1));                      // as requested we get a window sized 1 µ-tick
          CHECK (win.visible().start() == Time::MIN);                            // and aligned at the lower domain bound
          CHECK (win.visible().end()   == Time::MIN + TimeValue(1));
          CHECK (win.pxWidth() < ZOOM_MAX_RESOLUTION);                           // however, can't reach maximum zoom this way
          CHECK (win.px_per_sec() == 1000000);
          CHECK (win.pxWidth()    == 1);
          
          win.setOverallDuration (Duration{FSecs(1)});
          win.calibrateExtension (2);                                            // so... get more pixels to work with
          CHECK (win.visible().duration() == TimeValue(2));                      // ... they are used to expand the window
          CHECK (win.px_per_sec() == 1000000);                                   // .. resting at exiting zoom level
          
          win.setMetric (ZOOM_MAX_RESOLUTION);
          CHECK (win.px_per_sec() == ZOOM_MAX_RESOLUTION);                       // now able to reach the maximum zoom level
          CHECK (win.px_per_sec() == 2000000);                                   // (which is more or less an arbitrary choice)
          CHECK (win.visible().start() == Time::MIN);
          CHECK (win.visible().end()   == Time::MIN + TimeValue(1));             // while the actual window size is µ-grid aligned
          CHECK (win.pxWidth()    == 2);                                         // meaning we can not zoom in without limit
          
          win.nudgeVisiblePos (+1);                                              // scroll one »step« to the right
          CHECK (win.visible().start() == Time::MIN + TimeValue(1));             // yet this step has been increased to a full window size,
          CHECK (win.visible().end()   == Time::MIN + TimeValue(2));             // since a smaller scoll-step can not be represented in µ-ticks
          CHECK (win.visible().duration() == TimeValue(1));
          CHECK (win.px_per_sec() == ZOOM_MAX_RESOLUTION);
          
          win.calibrateExtension (3);                                            // add a third pixel
          CHECK (win.visible().duration() == TimeValue(2));                      // window extension increased to the next full µ-tick
          CHECK (win.px_per_sec() == 3_r/4 * ZOOM_MAX_RESOLUTION);               // and the rest was absorbed into the zoom scale
          CHECK (win.visible().start() == Time::MIN + TimeValue(1));
          CHECK (win.visible().end()   == Time::MIN + TimeValue(3));
          CHECK (win.pxWidth() == 3);
          
          win.setVisibleDuration (Duration{TimeValue(17)});
          CHECK (win.px_per_sec() == 3000000_r/17);
          win.setVisibleDuration (Duration{TimeValue(16)});
          CHECK (win.px_per_sec() == 187500);
          win.setVisibleDuration (Duration{TimeValue(15)});
          CHECK (win.px_per_sec() == 200000);
          CHECK (win.visible().start() == Time::MIN + TimeValue(1));
          CHECK (win.visible().end()   == Time::MIN + TimeValue(16));
          
          win.nudgeMetric (-1);
          CHECK (win.px_per_sec() == 100000);
          CHECK (win.visible().duration() == TimeValue(30));
          win.nudgeMetric (+2);
          CHECK (win.px_per_sec() == 375000);
          CHECK (win.visible().duration() == TimeValue(8));
          win.nudgeMetric (+1);
          CHECK (win.px_per_sec() == 750000);
          CHECK (win.visible().duration() == TimeValue(4));
          
          win.setMetric (2_r/3 * ZOOM_MAX_RESOLUTION);
          CHECK (win.px_per_sec() == 1_r/2 * ZOOM_MAX_RESOLUTION);               // can't do that, Dave
          CHECK (win.px_per_sec() == 1000000);
          CHECK (win.visible().duration() == TimeValue(3));
          CHECK (win.visible().start() == Time::MIN + TimeValue(1));
          CHECK (win.visible().end()   == Time::MIN + TimeValue(4));
          
          win.nudgeVisiblePos (-5);
          CHECK (win.visible().start() == Time::MIN + TimeValue(0));             // stopped at lower time domain limit
          CHECK (win.visible().end()   == Time::MIN + TimeValue(3));
          CHECK (win.visible().duration() == TimeValue(3));
          
          win.calibrateExtension (MAX_PX_WIDTH);                                 // similar logic applies when using much more pixels
          CHECK (win.pxWidth() == 100000);
          CHECK (win.visible().duration() == TimeValue(100000));
          CHECK (win.px_per_sec() == 1_r/2 * ZOOM_MAX_RESOLUTION);
          CHECK (win.visible().start() == Time::MIN + TimeValue(0));
          CHECK (win.visible().end()   == Time::MIN + TimeValue(100000));
          
          win.setMetric (3_r/2 * ZOOM_MAX_RESOLUTION);
          CHECK (win.px_per_sec() == ZOOM_MAX_RESOLUTION);                       // that's all we get
          CHECK (win.visible().duration() == TimeValue(50000));                  // (until someone comes up with a good use case for showing more)
          CHECK (win.visible().end()   == Time::MIN + TimeValue(50000));
          CHECK (win.pxWidth() == 100000);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ZoomWindow_test, "unit gui");
  
  
}}} // namespace stage::model::test
