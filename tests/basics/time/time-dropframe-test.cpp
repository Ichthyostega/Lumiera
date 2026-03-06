/*
  TimeDropframe(test)  -  document drop-frame calculation

   Copyright (C)
    2010                Stefan Kangas <skangas@skangas.se>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file time-dropframe-test.cpp
 ** unit test \ref TimeDropframe_test to document the
 ** algorithm to compute the components of drop-frame timecode.
 ** @see TimeFormats_test
 ** @see TimeValue_test
 ** @see timevalue.hpp
 */


#include "test/run.hpp"
#include "test/test-helper.hpp"
#include "lib/time/dropframe.hpp"
#include "lib/time/timevalue.hpp"


namespace lib {
namespace time{
namespace test{
  
  using time::Time;
  using time::TimeVar;
  using time::FSecs;
  using time::raw_time_64;
  
  namespace {// Test setup
    const int FRAMES = 15;
    const int MILLIS = 700;
    const int SECONDS = 20;
    const int MINUTES = 55;
    const int HOURS = 3;
    
    
    /* ====== conversion helpers to verify results ====== */
    
    const auto TIME_SCALE_sec{lib::time::TimeValue::SCALE        };
    const auto TIME_SCALE_ms {lib::time::TimeValue::SCALE / 1'000};
    
    int
    dropframe_frames (raw_time_64 timecode)
    {
      return calculate_ntsc_drop_frame_number(timecode) % 30;
    }
    
    int
    dropframe_seconds (raw_time_64 timecode)
    {
      return calculate_ntsc_drop_frame_number(timecode) / 30 % 60;
    }
    
    int
    dropframe_minutes (raw_time_64 timecode)
    {
      return calculate_ntsc_drop_frame_number(timecode) / 30 / 60 % 60;
    }
    
    int
    dropframe_hours (raw_time_64 timecode)
    {
      return calculate_ntsc_drop_frame_number(timecode) / 30 / 60 / 60 % 24;
    }
    
    int
    time_hours (raw_time_64 time)
    {
      return time / TIME_SCALE_sec / 60 / 60;
    }
    
    int
    time_minutes (raw_time_64 time)
    {
      return (time / TIME_SCALE_sec / 60) % 60;
    }
    
    int
    time_seconds (raw_time_64 time)
    {
      return (time / TIME_SCALE_sec) % 60;
    }
    
    int
    time_millis (raw_time_64 time)
    {
      return (time / TIME_SCALE_ms) % 1000;
    }
  }
  
  
  
  
  /******************************************************************//**
   * @test document the computation of NTSC drop frame timecode mapping.
   */
  class TimeDropframe_test : public Test
    {
      virtual void
      run (Arg)
        {
          verify_DropFrame_conv();
          verify_completeMapping();
        }
      
      
      /** @test perform a drop-frame timecode conversion
       *        and verify the complete round-trip is correct.
       */
      void
      verify_DropFrame_conv()
        {
            // Make sure frame 0 begins at 0
            raw_time_64 t = build_time_from_ntsc_drop_frame (0, 0, 0, 0);
            CHECK (t == 0);
            
            t = build_time_from_ntsc_drop_frame (FRAMES, SECONDS, MINUTES, HOURS);
            
            // Calculate manually what result to expect....
            int frames = FRAMES + 30*SECONDS + 30*60*MINUTES + 30*60*60*HOURS;   // sum up using nominal 30fps
            int minutes_to_drop_frames = (MINUTES - MINUTES/10) + (HOURS * 54);  // but every minute, with the exception of every 10 minutes...
            frames -= 2*minutes_to_drop_frames;                                  // ...drop 2 frames
            int64_t expectedMillis = 1000LL * frames  * 1001/30000;              // now convert frames to time, using the real framerate
            
            expectedMillis %= 1000;                                              // look at the remainder..
            CHECK (time_millis (t)  == expectedMillis);
            
            CHECK (time_seconds (t) == SECONDS);                                 // while all other components should come out equal as set
            CHECK (time_minutes (t) == MINUTES);
            CHECK (time_hours (t)   == HOURS);
            
            // Reverse calculate frames for NTSC drop
          //CHECK (lumiera_quantise_frames (t, 0, dropFrameDuration) == frames); // the total nominal frames
            CHECK (dropframe_frames (t) == FRAMES);                              // maximum one frame off due to rounding

          }
      
      
      
      /** @test Cover the whole value range of a day in drop-frame:
       *      - manually construct a drop-frame timecode
       *      - make sure our library function returns the same times.
       */
      void
      verify_completeMapping()
        {
          for (int hrs = 0; hrs <= 24; hrs += 6)
            for (int min = 0; min <= 59; min += 1)
              for (int sec = 0; sec <= 59; sec += 10)
                for (int frame = 0; frame <= 29; frame++)
                  {
                    // Skip dropped frames
                    if (min % 10 and sec == 0 and frame < 2)
                      continue;
                    
                    raw_time_64 t = build_time_from_ntsc_drop_frame(frame, sec, min, hrs);
                    /*
                    ECHO ("%02d:%02d:%02d;%02d"
                         , lumiera_time_ntsc_drop_hours (t)
                         , lumiera_time_ntsc_drop_minutes (t)
                         , lumiera_time_ntsc_drop_seconds (t)
                         , lumiera_time_ntsc_drop_frames (t)
                         );
                    */
                    CHECK (dropframe_frames (t)  == frame);
                    CHECK (dropframe_seconds (t) == sec);
                    CHECK (dropframe_minutes (t) == min);
                    CHECK (dropframe_hours (t)   == hrs % 24);
                  }
          
          // Make sure we do not get non-existent frames
          for (int i = 0; i < 59; i++)
            {
              int frame = (i % 10 == 0) ? 0 : 2;
              raw_time_64 t = build_time_from_ntsc_drop_frame (frame, 0, i, 0);
              CHECK (dropframe_frames (t) == frame);
            }
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimeDropframe_test, "unit common");
  
  
  
}}} // namespace lib::time::test
