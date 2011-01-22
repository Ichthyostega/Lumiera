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


#include "lib/test/run.hpp"
//#include "lib/test/test-helper.hpp"
#include "proc/asset/meta/time-grid.hpp"
#include "lib/time/timequant.hpp"
#include "lib/time/timecode.hpp"
#include "lib/time/display.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>
//#include <cstdlib>

using boost::lexical_cast;
using util::isnil;
//using std::rand;
using std::cout;
using std::endl;


namespace lib {
namespace time{
namespace test{
  
  using asset::meta::TimeGrid;
  
  /********************************************************
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
          TimeGrid::build("pal0", FrameRate::PAL);
          
//        checkTimecodeUsageCycle ();
//        checkFrames ();
//        checkSeconds ();
//        checkHms ();
          checkSmpte();
//        checkDropFrame();
//        checkCopyAssignments();
        } 
      
      
      void
      checkTimecodeUsageCycle ()
        {
          UNIMPLEMENTED ("full usage cycle for a timecode value");
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
          UNIMPLEMENTED ("verify seconds as timecode format");
        }
      
      
      void
      checkHms ()
        {
          UNIMPLEMENTED ("verify hour-minutes-seconds-millis timecode");
        }
      
      
      void
      checkSmpte ()
        {
          Time raw(555,23,42,5);
          QuTime t1 (raw, "pal0");
          SmpteTC smpte(t1);
          
          showTimeCode(smpte);
          CHECK ("  5:42:23:13" == string(smpte));
          CHECK (raw - Time(35,0) == smpte.getTime());    // quantisation to next lower frame
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
          
          // extension beyond origin to negative values
          Time tx = smpte.getTime();
          smpte.hours -= 6;
          CHECK ("- 0:21:59:24"== string(smpte));         // representation is symmetrical to origin
          CHECK (tx - Time(6*60*60) == smpte.getTime());  // Continuous time axis
          
          CHECK (-1 == smpte.sgn);                        // Note: for these negative (extended) SMPTE...
          CHECK (smpte.mins > 0);                         // ...the representation is really flipped around zero
          CHECK (smpte.secs > 0);
          CHECK (smpte.frames > 0);
          tx = smpte.getTime();
          ++smpte.frames;                                 // now *increasing* the frame value
          CHECK ("- 0:22:00:00"== string(smpte));         // means decreasing the resulting time
          CHECK (tx - Time(1000/25,0,0,0) == smpte.getTime());
          ++smpte;                                        // but the orientation of the increment on the *whole* TC values is unaltered
          CHECK ("- 0:21:59:24"== string(smpte));         // so this actually *advanced* time by one frame 
          CHECK (tx == smpte.getTime());
          CHECK (tx <  Time(0));
          
          smpte.mins -= 2*60;                             // now lets flip it again...
          CHECK ("  1:38:00:01"== string(smpte));
          CHECK (+1 == smpte.sgn);
          CHECK (smpte.getTime() > 0);
          CHECK (tx + Time(2*60*60) == smpte.getTime());
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
