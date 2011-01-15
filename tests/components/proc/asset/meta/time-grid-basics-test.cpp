/*
  TimeGridBasics(Test)  -  verify a simple reference scale for time quantisation

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
#include "lib/test/test-helper.hpp"

#include "proc/asset/meta.hpp"
#include "proc/asset/meta/time-grid.hpp"
//#include "proc/asset/entry-id.hpp"
//#include "lib/p.hpp"
//#include "proc/assetmanager.hpp"
//#include "proc/asset/inventory.hpp"
//#include "proc/mobject/session/clip.hpp"
//#include "proc/mobject/session/track.hpp"
//#include "lib/meta/trait-special.hpp"
//#include "lib/util-foreach.hpp"
//#include "lib/symbol.hpp"

//#include <tr1/unordered_map>
//#include <iostream>
//#include <string>

//using lib::test::showSizeof;
//using lib::test::randStr;
//using util::isSameObject;
//using util::and_all;
//using util::for_each;
//using util::isnil;
using lib::test::randStr;
//using lib::Literal;
//using lib::Symbol;
//using lumiera::P;
//using std::string;
//using std::cout;
//using std::endl;



namespace asset {
namespace meta {
namespace test {
  
  typedef Builder<TimeGrid> GridBuilder;
  typedef EntryID<TimeGrid> GridID;
  
  namespace { // Test definitions...
    
    Time testOrigin (12,23);
    FrameRate testFps (5,4);
    
  }
  
  
  
  /***************************************************************************
   * @test build some simple time grids and verify their behaviour
   *       for quantising (grid aligning) time values.
   * 
   * @todo WIP-WIP-WIP                                       ////////////////////TICKET #736
   * 
   * @see asset::meta::TimeGrid
   * @see time-quantisation-test.cpp usage context
   */
  class TimeGridBasics_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          createGrid_fullProcedure();
          createGrid_simplified();
        }
      
      
      void
      createGrid_fullProcedure()
        {
          GridID myGrID (randStr(8));
          GridBuilder spec = asset::Meta::create (myGrID);
          
          CHECK (!spec.fps_);
          CHECK (spec.origin_ == Time(0));
          CHECK (!spec.predecessor_);
          
          spec.fps_ = testFps;
          spec.origin_ = testOrigin;
          
          PGrid myGrid = spec.commit();
          CHECK (myGrid);
          
          UNIMPLEMENTED ("the basic Grid API, so we can actually check anything");
        }
      
      
      void
      createGrid_simplified()
        {
        }
      
      
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (TimeGridBasics_test, "unit asset");
  
  
}}} // namespace asset::meta::test
