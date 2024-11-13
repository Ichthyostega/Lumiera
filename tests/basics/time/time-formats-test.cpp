/*
  TimeFormats(Test)  -  timecode handling and formatting

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file time-formats-test.cpp
 ** unit test \ref TimeFormats_test
 */


#include "lib/test/run.hpp"
//#include "lib/test/test-helper.hpp"
#include "steam/asset/meta/time-grid.hpp"
#include "lib/time/timequant.hpp"
#include "lib/time/timecode.hpp"
#include "lib/time/mutation.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include <string>

using boost::lexical_cast;
using util::isnil;
using std::string;


namespace lib {
namespace time{
namespace test{
  
  using steam::asset::meta::TimeGrid;
  using util::toString;
  using format::Frames;
  using format::Smpte;
  
  namespace{
    const FrameCnt MAX_FRAME = 265*24*60*60*25;
    
    string
    generateRandomFrameNr()
    {
      FrameCnt frameNr(0);
      while (!frameNr)
        frameNr = rani(2*MAX_FRAME) - MAX_FRAME;
      
      return toString(frameNr)+"#";
    }
  }
  
  
  
  /****************************************************//**
   * @test verify handling of grid aligned timecode values.
   *       - full cycle from parsing to formatting
   *       - mutating the components of timecode
   *       - some formatting corner cases
   *       - formatting in various formats
   */
  class TimeFormats_test : public Test
    {
      virtual void
      run (Arg) 
        {
          seedRand();
          TimeGrid::build("pal0", FrameRate::PAL);
          
          checkTimecodeUsageCycle ();
//        checkFrames ();
//        checkSeconds ();
//        checkHms ();
          checkSmpte();
//        checkDropFrame();
//        checkCopyAssignments();
        } 
      
      
      /** @test demonstrate a full usage cycle of timecode and time values.
       *        Starting with a textual representation according to a specific timecode format,
       *        and based on the knowledge of the implicit underlying time grid (coordinate system,
       *        here with origin=0 and framerate=25fps), this timecode string may be parsed.
       *        This brings us (back) to the very origin, which is a raw TimeValue (internal time).
       *        Now, this time value might be manipulated, compared to other values etc.
       *        Anyway, at some point these time values need to be related to some time scale again,
       *        leading to quantised time values, which — finally — can be cast into a timecode format
       *        for external representation again, thus closing the circle.
       */
      void
      checkTimecodeUsageCycle ()
        {
          string srcCode  = generateRandomFrameNr();
          PQuant refScale = Quantiser::retrieve("pal0");
          
          // get internal (raw) time value
          TimeValue t1 = format::Frames::parse (srcCode, *refScale);
          ENSURE (0 != t1);
          
          // manipulating
          TimeVar v1(t1);
          v1 += Time(FSecs(6,5));
          CHECK (t1 < v1);
          
          // quantising into an external grid
          QuTime q1 (t1, "pal0");
          CHECK (q1 == t1);
          
          // further mutations (here nudge by +5 grid steps)
          QuTime q2 = q1;
          q2.accept (Mutation::nudge(+5));
          CHECK (q1 < q2);
          
          // converting (back) into a timecode format
          FrameNr frames1(q1);
          FrameNr frames2(q2);
          CHECK (5 == frames2 - frames1);
          
          q2.accept (Mutation::changeTime(v1));
          CHECK (30 == q2.formatAs<Frames>() - frames1);     // q2 == v1 == t1 + (6*5)/(5*5)sec
          
          CHECK (srcCode == string(frames1));
          CHECK (srcCode != string(frames2));
          
          showTimeCode (frames1);
          showTimeCode (frames2);
          showTimeCode (q2.formatAs<Smpte>());
        }
      
      
      template<class TC>
      void
      showTimeCode (TC timecode)
        {
          cout << timecode.describe()<<"=\""<<timecode<<"\" time = "<< timecode.getTime() << endl; 
        }
      
      void
      checkFrames ()
        {
          UNIMPLEMENTED ("verify frame count time format");
        }
      
      
      void
      checkSeconds ()
        {
          UNIMPLEMENTED ("verify fractional seconds as timecode format");
        }
      
      
      void
      checkHms ()
        {
          UNIMPLEMENTED ("verify hour-minutes-seconds-millis timecode");
        }
      
      
      /** @test detailed coverage of SMPTE timecode representation.
       *        Using a scale grid with PAL framerate; this test doesn't
       *        cover the handling of drop-frame timecode.
       *        - creating a timecode representation causes frame quantisation
       *        - the components of SMPTE timecode can be accessed and manipulated
       *        - timecode can be incremented/decremented as a whole
       *        - we allow extension of the scale towards negative values
       *        - for these, the representation is flipped and the negative
       *          orientation only indicated through the sign field.
       */
      void
      checkSmpte ()
        {
          Time raw(555,23,42,5);
          QuTime t1 (raw, "pal0");
          SmpteTC smpte(t1);
          
          cout << "----SMPTE-----" << endl;
          showTimeCode(smpte);
          CHECK ("  5:42:23:13" == string(smpte));
          CHECK (raw - Time(35,0) == smpte.getTime());    // timecode value got quantised towards next lower frame
          CHECK (13 == smpte.frames);
          CHECK (23 == smpte.secs);
          CHECK (42 == smpte.mins);
          CHECK ( 5 == smpte.hours);
          CHECK ( 1 == smpte.sgn);
          CHECK ("SMPTE" == smpte.describe());
          
          ++smpte;
          CHECK ("  5:42:23:14" == string(smpte));
          smpte.frames += 12;
          CHECK ("  5:42:24:01" == string(smpte));
          smpte.secs = -120;
          CHECK ("  5:40:00:01" == string(smpte));
          CHECK (smpte.mins-- == 40);
          CHECK (--smpte.mins == 38);
          CHECK ("  5:38:00:01" == string(smpte));
          
          TimeVar tx = smpte.getTime();
          CHECK (tx == Time(0,0,38,5) + Time(FSecs(1,25)));
          
          // Extended SMPTE: extension of the axis beyond origin towards negative values
          smpte.hours -= 6;
          CHECK ("- 0:21:59:24"== string(smpte));         // representation is symmetrical to origin
          CHECK (tx - Time(0,0,0,6) == smpte.getTime());  // Continuous time axis
          
          CHECK (-1 == smpte.sgn);                        // Note: for these negative (extended) SMPTE...
          CHECK (smpte.mins > 0);                         // ...the representation is really flipped around zero
          CHECK (smpte.secs > 0);
          CHECK (smpte.frames > 0);
          tx = smpte.getTime();
          ++smpte.frames;                                 // now *increasing* the frame value
          CHECK ("- 0:22:00:00"== string(smpte));         // means decreasing the resulting time
          CHECK (smpte.getTime() == tx - Time(1000/25,0,0,0));
          ++smpte;                                        // but the orientation of the increment on the *whole* TC values is unaltered
          CHECK ("- 0:21:59:24"== string(smpte));         // so this actually *advanced* time by one frame 
          CHECK (tx == smpte.getTime());
          CHECK (tx <  TimeValue(0));
          
          smpte.mins -= 2*60;                             // now lets flip the representation again...
          CHECK ("  1:38:00:01"== string(smpte));
          CHECK (+1 == smpte.sgn);
          CHECK (smpte.getTime() > 0);
          CHECK (tx + Time(0,0,0,2) == smpte.getTime());
          smpte.secs -= 2*60*60;                          // and again...
          CHECK (tx == smpte.getTime());
          CHECK ("- 0:21:59:24"== string(smpte));
          
          smpte.sgn += 123;                               // just flip the sign
          CHECK ("  0:21:59:24"== string(smpte));
          CHECK (tx == -smpte.getTime());
          CHECK (+1 == smpte.sgn);                        // sign value is limited to +1 / -1
          
          smpte.secs.setValueRaw(61);                     // set "wrong" value, bypassing normalisation
          CHECK (smpte.secs == 61);
          CHECK (smpte.getTime() == Time(1000*24/25, 01, 22));
          CHECK (smpte.secs == 61);                       // calculated value is correct, but doesn't change state
          CHECK ("  0:21:61:24"== string(smpte));
          smpte.rebuild();                                // but rebuilding the value includes normalisation
          CHECK (smpte.secs ==  1);
          CHECK (smpte.mins == 22);
          CHECK ("  0:22:01:24"== string(smpte));
          
          smpte.frames.setValueRaw (25);
          CHECK ("  0:22:01:25"== string(smpte));
          smpte.hours = -1;                               // flipped representation handles denormalised values properly
          CHECK ("- 0:37:58:00"== string(smpte));
          
          smpte.mins.setValueRaw (59);
          smpte.secs.setValueRaw (61);
          smpte.frames.setValueRaw(-26);                  // provoke multiple over/underflows...
          smpte.hours.setValueRaw (-2);
          CHECK ("--2:59:61:-26"==string(smpte));
          tx = smpte.getTime();
          CHECK (tx == -1*(Time(0,61,59) - Time(0,0,0,2) - Time(FSecs(26,25))));
          smpte.invertOrientation();
          CHECK ("  1:00:00:01"== string(smpte));
          CHECK (tx == smpte.getTime());                  // applying invertOrientation() doesn't change the value
          
          smpte.frames.setValueRaw(-1);
          tx -= Time(FSecs(2,25));
          CHECK (tx == smpte.getTime());
          CHECK ("  1:00:00:-1"== string(smpte));
          smpte.invertOrientation();                      // invoking on positive should create double negated representation
          CHECK ("--1:00:00:01"== string(smpte));         // and here especially also causes a series of overflows
          CHECK (tx == smpte.getTime());                  // but without affecting the overall value
        }
      
      
      void
      checkDropFrame ()
        {
          UNIMPLEMENTED ("verify especially SMPTE-drop-frame timecode");
        }
      
      
      void
      checkCopyAssignments ()
        {
          UNIMPLEMENTED ("verify Timecode values can be copied and assigned properly");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimeFormats_test, "unit common");
  
  
  
}}} // namespace lib::time::test
