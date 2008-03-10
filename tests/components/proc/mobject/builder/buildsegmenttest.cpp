/*
  BuildSegment(Test)  -  building the render-tree for a segment of the EDL
 
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


#include "common/test/run.hpp"
//#include "common/factory.hpp"
//#include "common/util.hpp"

//#include <boost/format.hpp>
#include <iostream>

//using boost::format;
using std::string;
using std::cout;


namespace mobject
  {
  namespace builder
    {
    namespace test
      {
      
      
      
      
      /*******************************************************************
       * @test the builder core functionality: create a render pipeline
       *       for a given segment of the EDL.
       */
      class BuildSegment_test : public Test
        {
          virtual void run(Arg arg) 
            {
            } 
        };
      
      
      /** Register this test class... */
      LAUNCHER (BuildSegment_test, "function builder");
      
      
      
    } // namespace test
    
  } // namespace builder

} // namespace mobject
