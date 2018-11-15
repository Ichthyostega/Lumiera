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

/** @file time-parsing-test.cpp
 ** unit test \ref TimeParsing_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "steam/asset/meta/time-grid.hpp"
#include "lib/time/quantiser.hpp"
#include "lib/time/timecode.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

using lib::Symbol;
using util::cStr;


namespace lib {
namespace time{
namespace test{
  
  using proc::asset::meta::TimeGrid;
  using format::LERR_(INVALID_TIMECODE);
  
  
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
  
  
  
  
  /****************************************************//**
   * @test parse textual time specifications given
   *       in the supported timecode formats
   */
  class TimeParsing_test : public Test
    {
      virtual void
      run (Arg) 
        {
          defineTestTimeGrids();
          
          parseFrames();
          parseFractionalSeconds();
/////////////////////////////////////////////TODO
//        parseHms();
//        parseSmpte();
//        parseDropFrame();
        } 
      
      
      void
      defineTestTimeGrids()
        {
          TimeGrid::build(DEFAULT_GRID, FrameRate::PAL);
          TimeGrid::build(OFFSET_GRID,  FrameRate::PAL, Time(0,10));
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
          Parsing<format::Frames> ("12 25#  33#")     .should_yield (1);                // note pitfall: the first valid number is used
          Parsing<format::Frames> ("12\n 25# \n 33#") .should_yield (1);
          Parsing<format::Frames> ("12.25#")          .should_fail();                   // rejected because of leading dot (ambiguity)
        }
      
      
      void
      parseFractionalSeconds ()
        {
          Parsing<format::Seconds> ("0sec")           .should_yield (0);
          Parsing<format::Seconds> ("1sec")           .should_yield (1);
          Parsing<format::Seconds> ("10sec")          .should_yield (10);
          Parsing<format::Seconds> ("100sec")         .should_yield (100);
          Parsing<format::Seconds> ("-10sec")         .should_yield (-10);
          Parsing<format::Seconds> ("-0sec")          .should_yield (0);
          
          Parsing<format::Seconds> ("1/2sec")         .should_yield (Time(500,0)     );
          Parsing<format::Seconds> ("1/25sec")        .should_yield (Time( 40,0)     );
          Parsing<format::Seconds> ("1/250sec")       .should_yield (Time(  4,0)     ); // no quantisation involved in parsing 
          Parsing<format::Seconds> ("1/250sec", OFFSET_GRID).should_yield (Time(4,10)); // ...but the origin of the grid is used
          
          Parsing<format::Seconds> ("10/2sec")        .should_yield (5);
          Parsing<format::Seconds> ("1000/200sec")    .should_yield (5);
          Parsing<format::Seconds> ("-10/2sec")       .should_yield (-5);
          Parsing<format::Seconds> ("10/-2sec")       .should_fail();                   // only leading sign allowed (ambiguity)
          
          Parsing<format::Seconds> ("1+1/2sec")       .should_yield (Time(500,1)     );
          Parsing<format::Seconds> ("1-1/2sec")       .should_yield (Time(500,0)     );
          Parsing<format::Seconds> ("-1-1/2sec")      .should_yield (-Time(500,1)    );
          Parsing<format::Seconds> ("-1+1/2sec")      .should_yield (-Time(500,0)    );
          Parsing<format::Seconds> ("-1+1/-2sec")     .should_fail();
          
          Parsing<format::Seconds> ("-12+24690/12345sec", OFFSET_GRID).should_yield(0); // origin=+10sec -12sec + 2/1sec == 0
          
          Parsing<format::Seconds> ("1")              .should_fail();
          Parsing<format::Seconds> ("1 sec")          .should_fail();
          Parsing<format::Seconds> ("--1sec")         .should_fail();
          Parsing<format::Seconds> ("/-1sec")         .should_fail();
          Parsing<format::Seconds> ("1.2sec")         .should_fail();
          Parsing<format::Seconds> ("1/.2sec")        .should_fail();
          Parsing<format::Seconds> ("1 + 2 / 4 sec")  .should_fail();
          Parsing<format::Seconds> ("1 + 2 / 4sec")   .should_yield(4);                 // note pitfall: leading garbage not considered
          Parsing<format::Seconds> ("xxx4secxxxx")    .should_yield(4);
          Parsing<format::Seconds> ("x1# 8/2sec 2sec").should_yield(4);                 // note pitfall: first valid number used
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
