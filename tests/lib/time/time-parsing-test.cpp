/*
  TimeParsing(Test)  -  handling textual time(code) specifications

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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
#include "lib/test/test-helper.hpp"
#include "proc/asset/meta/time-grid.hpp"
#include "lib/time/quantiser.hpp"
//#include "lib/time/timequant.hpp"
#include "lib/time/timecode.hpp"
//#include "lib/time/display.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

//#include <boost/lexical_cast.hpp>
//#include <iostream>
//#include <cstdlib>

//using boost::lexical_cast;
//using util::isnil;
using lib::Symbol;
using util::cStr;
//using std::rand;
//using std::cout;
//using std::endl;


namespace lib {
namespace time{
namespace test{
  
  using asset::meta::TimeGrid;
  using format::LUMIERA_ERROR_INVALID_TIMECODE;
  
  namespace { // Helper for writing test cases
    
    Symbol DEFAULT_GRID = "pal0";
    Symbol OFFSET_GRID  = "pal10";
    
    /** 
     * Expression builder
     * for writing time value parsing tests
     */
    template<class FMT>
    class Parsing
      {
        string const& timeSpec_;
        PQuant grid_;
        
      public:
        Parsing (string const& toParse, Symbol gridID =DEFAULT_GRID)
          : timeSpec_(toParse)
          , grid_(Quantiser::retrieve(gridID))
          { }
        
        
        void
        should_yield (TimeValue const& expected)
          {
            TimeValue parsed = FMT::parse (timeSpec_, *grid_);
            CHECK (parsed == expected, "parsing '%s' resulted in %s instead of %s"
                                     , cStr(timeSpec_)
                                     , cStr(Time(parsed))
                                     , cStr(Time(expected)));
          }
        
        void
        should_yield (FSecs const& expectedSecs)
          {
            should_yield (Time (expectedSecs));
          }
        
        void
        should_fail ()
          {
            VERIFY_ERROR (INVALID_TIMECODE, FMT::parse (timeSpec_, *grid_));
          }
        
      };
    
  }//(End)Test case helper
  
  
  
  
  /********************************************************
   * @test parse textual time specifications given
   *       in the supported timecode formats
   */
  class TimeParsing_test : public Test
    {
      virtual void
      run (Arg) 
        {
          TimeGrid::build("pal0",  FrameRate::PAL);
          TimeGrid::build("pal10", FrameRate::PAL, Time(0,10));
          
          parseFrames();
          parseFractionalSeconds();
//        parseHms();
//        parseSmpte();
//        parseDropFrame();
        } 
      
      
      void
      parseFrames ()
        {
          Parsing<format::Frames> ("0#")              .should_yield (0);
          Parsing<format::Frames> ("1#")              .should_yield (FSecs(1,25)     );
          Parsing<format::Frames> ("-1#")             .should_yield (FSecs(-1,25)    );
          Parsing<format::Frames> ("-0#")             .should_yield (0);
          Parsing<format::Frames> ("25#")             .should_yield (1               );
          Parsing<format::Frames> ("26#")             .should_yield (Time(40,1)      );
          Parsing<format::Frames> ("25#", OFFSET_GRID).should_yield (1+10            );
          Parsing<format::Frames> ("-1#", OFFSET_GRID).should_yield (10 - FSecs(1,25));
          
          Parsing<format::Frames> ("23")              .should_fail();
          Parsing<format::Frames> ("23 #")            .should_fail();
          Parsing<format::Frames> ("23.#")            .should_fail();
          Parsing<format::Frames> ("23x#")            .should_fail();
          
          Parsing<format::Frames> ("xxx25#xxx")       .should_yield (1);
          Parsing<format::Frames> ("12 25#")          .should_yield (1);
          Parsing<format::Frames> ("12 25#  33#")     .should_yield (1);
          Parsing<format::Frames> ("12\n 25# \n 33#") .should_yield (1);
          Parsing<format::Frames> ("12.25#")          .should_yield (1);
        }
      
      
      void
      parseFractionalSeconds ()
        {
          UNIMPLEMENTED ("verify reading fractional seconds as timecode format");
        }
      
      
      void
      parseHms ()
        {
          UNIMPLEMENTED ("verify reading hour-minutes-seconds-millis time specs");
        }
      
      
      void
      parseSmpte ()
        {
        }
      
      
      void
      parseDropFrame ()
        {
          UNIMPLEMENTED ("verify especially SMPTE-drop-frame timecode");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimeParsing_test, "unit common");
  
  
  
}}} // namespace lib::time::test
