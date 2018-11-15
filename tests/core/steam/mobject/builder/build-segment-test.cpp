/*
  BuildSegment(Test)  -  building the render-tree for a segment of the Timeline

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file build-segment-test.cpp
 ** unit test \ref BuildSegment_test
 */


#include "lib/test/run.hpp"
//#include "lib/util.hpp"


//using std::string;


namespace steam {
namespace mobject {
namespace builder {
namespace test {
  
  
  
  
  /***************************************************************//**
   * @test the builder core functionality: create a render pipeline
   *       for a given segment of the Session/Timeline.
   */
  class BuildSegment_test : public Test
    {
      virtual void run(Arg) 
        {
          UNIMPLEMENTED ("oh my");
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (BuildSegment_test, "function builder");
  
  
  
}}}} // namespace steam::mobject::builder::test
