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
#include "lib/time/timecode.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>
//#include <iostream>
//#include <cstdlib>

using boost::lexical_cast;
using util::isnil;
//using std::rand;
//using std::cout;
//using std::endl;


namespace lib {
namespace time{
namespace test{
  
  
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
      run (Arg arg) 
        {
          long refval= isnil(arg)?  1 : lexical_cast<long> (arg[1]);
          
          TimeValue ref (refval);
          
          checkTimecodeUsageCycle (ref);
          checkFrames ();
          checkSeconds ();
          checkHms ();
          checkSmpte();
          checkDropFrame();
        } 
      
      
      void
      checkTimecodeUsageCycle (TimeValue ref)
        {
          UNIMPLEMENTED ("full usage cycle for a timecode value");
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
          UNIMPLEMENTED ("verify SMPTE timecode format");
        }
      
      
      void
      checkDropFrame ()
        {
          UNIMPLEMENTED ("verify especially SMPTE-drop-frame timecode");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimeFormats_test, "unit common");
  
  
  
}}} // namespace lib::time::test
