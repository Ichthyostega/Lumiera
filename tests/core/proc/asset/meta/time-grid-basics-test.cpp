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

/** @file time-grid-basics-test.cpp
 ** unit test \ref TimeGridBasics_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"

#include "proc/asset/meta.hpp"
#include "proc/asset/meta/time-grid.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include <boost/rational.hpp>

using boost::rational_cast;
using lib::test::randStr;



namespace proc {
namespace asset{
namespace meta {
namespace test {
  
  using namespace lib::time;
  
  typedef Builder<TimeGrid> GridBuilder;
  
  namespace { // Test definitions...
    
    const Time testOrigin (12,34);
    const FrameRate testFps (5,6);
    
    const uint MAX_FRAMES = 1000;
    const uint DIRT_GRAIN = 50;
  }
  
  
  
  /***********************************************************************//**
   * @test build some simple time grids and verify their behaviour
   *       for quantising (grid aligning) time values.
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
          
          CHECK ( spec.fps    == 1);
          CHECK ( spec.origin == TimeValue(0));
          CHECK (!spec.predecessor);
          
          spec.fps    = testFps;
          spec.origin = testOrigin;
          
          PGrid myGrid = spec.commit();
          CHECK (myGrid);
          CHECK (myGrid->ident.name == myGrID.getSym());
          
           // now verify the grid
          //  by performing some conversions...
          int randomFrame = (rand() % MAX_FRAMES);
          
          Time point (myGrid->timeOf (randomFrame));
          CHECK (point == testOrigin + randomFrame * testFps.duration());
          
          uint fract = 2 + rand() % DIRT_GRAIN;
          FSecs dirt = rational_cast<FSecs> (1 / testFps / fract);
          ASSERT (Time(dirt) < testFps.duration());
          ASSERT (0 < dirt);
          
          Time dirty(point + Time(dirt));
          CHECK (point == testOrigin + myGrid->gridAlign(dirty));
        }
      
      
      void
      createGrid_simplified()
        {
          PGrid simplePALGrid = TimeGrid::build ("", FrameRate::PAL);
          CHECK (simplePALGrid);
          CHECK (!util::isnil (simplePALGrid->ident.name));   // note: name-ID filled in automatically
          cout << "simple PAL Grid: " << simplePALGrid->ident << endl;
          
          CHECK (Time(0,2) == simplePALGrid->timeOf(50));
          CHECK (Time(0,2) == simplePALGrid->timeOf(FSecs(2)));
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (TimeGridBasics_test, "unit asset");
  
  
}}}} // namespace proc::asset::meta::test
