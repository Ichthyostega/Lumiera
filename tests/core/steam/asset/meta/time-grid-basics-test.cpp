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

#include "steam/asset/meta.hpp"
#include "steam/asset/meta/time-grid.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include <boost/rational.hpp>

using boost::rational_cast;
using lib::test::randStr;



namespace steam {
namespace asset{
namespace meta {
namespace test {
  
  using namespace lib::time;
  
  typedef Builder<TimeGrid> GridBuilder;
  
  namespace { // Test definitions...
    
    const Time TEST_ORIGIN (12,34);
    const FrameRate TEST_FPS (5,6);
    
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
          
          spec.fps    = TEST_FPS;
          spec.origin = TEST_ORIGIN;
          
          PGrid myGrid = spec.commit();
          CHECK (myGrid);
          CHECK (myGrid->ident.name == myGrID.getSym());
          
           // now verify the grid
          //  by performing some conversions...
          int randomFrame = (rand() % MAX_FRAMES);
          
          Time point (myGrid->timeOf (randomFrame));
          CHECK (point == TEST_ORIGIN + randomFrame * TEST_FPS.duration());
          
          int fract = 2 + rand() % DIRT_GRAIN;
          FSecs dirt = (1/TEST_FPS) / fract;
          ASSERT (Time(dirt) < TEST_FPS.duration());
          ASSERT (0 < dirt);
          
          Time dirty(point + Time(dirt));
          CHECK (point == TEST_ORIGIN + myGrid->gridLocal(dirty));
        }
      
      
      void
      createGrid_simplified()
        {
          PGrid simplePALGrid = TimeGrid::build (FrameRate::PAL);
          CHECK (simplePALGrid);
          CHECK (!util::isnil (simplePALGrid->ident.name));   // note: name-ID filled in automatically
          cout << "simple PAL Grid: " << simplePALGrid->ident << endl;
          
          CHECK (Time(0,2) == simplePALGrid->timeOf(50));
          CHECK (Time(0,2) == simplePALGrid->timeOf(FSecs(2)));
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (TimeGridBasics_test, "unit asset");
  
  
}}}} // namespace steam::asset::meta::test
