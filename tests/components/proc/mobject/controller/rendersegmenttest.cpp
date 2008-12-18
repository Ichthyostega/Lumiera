/*
  RenderSegment(Test)  -  Proc-Layer Integrationtest
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/


#include "lib/test/run.hpp"
//#include "lib/factory.hpp"
//#include "lib/util.hpp"

//#include <boost/format.hpp>
#include <iostream>

//using boost::format;
using std::string;
using std::cout;


namespace mobject
  {
  namespace controller
    {
    namespace test
      {
      
      
      
      
      /*******************************************************************
       * @test create a render process from a given segment of the EDL.
       *       Basically this includes cooperation of all parts of the
       *       Lumiera Proc Layer. For a prepared test-EDL we invoke the
       *       controller to create a render process. This includes building
       *       the render pipeline. Finally, we analyze all the created 
       *       Structures. 
       * @note this test doesn't cover the actual rendering.
       * @see  proc_interface::ControllerFacade
       */
      class RenderSegment_test : public Test
        {
          virtual void run(Arg arg) 
            {
              UNIMPLEMENTED ("complete render process for a given test segment of the EDL");
            } 
        };
      
      
      /** Register this test class... */
      LAUNCHER (RenderSegment_test, "function operate");
      
      
      
    } // namespace test
    
  } // namespace controller

} // namespace mobject
