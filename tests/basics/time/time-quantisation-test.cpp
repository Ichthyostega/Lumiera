/*
  TimeQuantisation(Test)  -  handling of virtually grid aligned time values

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

/** @file time-quantisation-test.cpp
 ** unit test \ref TimeQuantisation_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "steam/asset/meta/time-grid.hpp"
#include "lib/time/timequant.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>

using boost::lexical_cast;
using util::isnil;
using util::contains;


namespace lib {
namespace time{
namespace test{
  
  using steam::asset::meta::TimeGrid;
  
  
  /****************************************************//**
   * @test verify handling of quantised time values.
   *       - the simple usage, just referring to an 
   *         predefined grid by name
   *       - explicitly defining an quantiser
   *       - converting these quantised values into
   *         various timecode formats
   *       - error detection
   */
  class TimeQuantisation_test : public Test
    {
      int
      random_or_get (Arg arg)
        {
          if (isnil(arg))
            {// use random time value for all tests
              seedRand();
              return 1 + rani(10000);
            }
          else
            return lexical_cast<int> (arg[1]);
        }
      
      
      
      virtual void
      run (Arg arg) 
        {
          Time ref (0,random_or_get(arg),0,0);
          CHECK (TimeValue(0) < ref);
          
          checkSimpleUsage (ref);
          check_theFullStory (ref);
          checkMultipleGrids (ref);
          checkGridBinding (ref);
        } 
      
      
      void
      checkSimpleUsage (TimeValue org)
        {
          TimeGrid::build("my_simple_grid", 25);    // "someone" has defined a time grid
          
          QuTime qVal (org, "my_simple_grid");      // create time quantised to this grid
          
          FrameNr count(qVal);                      // materialise this quantised time into..
          int n = count;                            // frame count, accessible as plain number
          
          CHECK (Time(FSecs(n-1, 25)) <= org);      // verify quantisation: the original time
          CHECK (org < Time(FSecs(n+1, 25)));       // is properly bracketed by (n-1, n+1)
        }
      
      
      void
      check_theFullStory (TimeValue org)
        {
          PQuant fixQ (new FixedFrameQuantiser(25));
          QuTime qVal (org, fixQ);
          
          CHECK ( qVal.supports<format::Frames>());
          CHECK ( qVal.supports<format::Smpte>());
          
          SmpteTC smpteTCode = qVal.formatAs<format::Smpte>();
          showTimeCode (smpteTCode);
          
          HmsTC pureTimeCode = qVal.formatAs<format::Hms>();
          showTimeCode (pureTimeCode);
          
          FrameNr frameTCode = qVal.formatAs<format::Frames>();
          showTimeCode (frameTCode);
          
          Secs seconds  = qVal.formatAs<format::Seconds>();
          showTimeCode (seconds);
        }
      
      template<class TC>
      void
      showTimeCode (TC timecodeValue)
        {
          cout << timecodeValue.describe() 
               << " time = "<< timecodeValue.getTime() 
               << " code = "<< timecodeValue
               << endl;
        }
      
      
      void
      checkMultipleGrids (TimeValue org)
        {
          TimeGrid::build("my_alternate_grid", FrameRate::NTSC);
          
          QuTime palVal (org, "my_simple_grid");
          QuTime ntscVal (org, "my_alternate_grid");
          
          CHECK (org == palVal);
          CHECK (org == ntscVal);
          
          FrameNr palNr (palVal);
          FrameNr ntscNr(ntscVal);
          CHECK (palNr <= ntscNr);
        }
      
      
      void
      checkGridBinding (TimeValue org)
        {
          // refer to a grid not yet defined
          VERIFY_ERROR (UNKNOWN_GRID, QuTime weird(org, "special_funny_grid"));
          
          TimeGrid::build("special_funny_grid", 1);      // provide the grid's definition (1 frame per second)
          
          QuTime funny (org, "special_funny_grid");      // now OK, grid is known
          int cnt = funny.formatAs<format::Frames>();
                                                         // and now performing quantisation is OK 
          SmpteTC smpte (funny);                         // also converting into SMPTE (which implies frame quantisation)
          CHECK (0 == smpte.frames);                     // we have 1fps, thus the frame part is always zero!
          CHECK (cnt % 60 == smpte.secs);                // and the seconds part will be in sync with the frame count
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimeQuantisation_test, "unit common");
  
  
  
}}} // namespace lib::time::test
