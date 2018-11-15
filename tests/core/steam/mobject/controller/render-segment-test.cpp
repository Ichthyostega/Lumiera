/*
  RenderSegment(Test)  -  Steam-Layer integration test

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

/** @file render-segment-test.cpp
 ** unit test \ref RenderSegment_test
 */


#include "lib/test/run.hpp"
//#include "lib/util.hpp"

//using std::string;


namespace steam {
namespace mobject {
namespace controller {
namespace test {
  
  
  
  
  /****************************************************************//**
   * @test create a render process from a given segment of the Session.
   *       Basically this includes cooperation of all parts of the
   *       Lumiera Steam Layer. For a prepared test-Session we invoke the
   *       controller to create a render process. This includes building
   *       the render pipeline. Finally, we analyse all the created 
   *       Structures. 
   * @note this test doesn't cover the actual rendering.
   * @see  proc_interface::ControllerFacade
   */
  class RenderSegment_test : public Test
    {
      virtual void run(Arg arg) 
        {
          UNIMPLEMENTED ("complete render process for a given test segment of the Session");
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (RenderSegment_test, "function operate");
  
  
  
}}}} // namespace steam::mobject::controller::test
