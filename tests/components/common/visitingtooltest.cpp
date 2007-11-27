/*
  VisitingTool(Test)  -  check the acyclic visitor lib implementation
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
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
#include "common/visitor.hpp"
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
      
      
      
      
      /*************************************************************************
       * @test our lib implementation of the acyclic visitor pattern.
       *       Defines a hierarchy of test classes to check the following cases
       *       <ul><li>calling the correct visiting tool specialized function
       *               for given concrete hierarchy classes</li>
       *           <li>visiting tool not declaring to visit some class</li>
       *           <li>newly added class causes the catch-all to be invoked
       *               when visited by known visitor</li>
       *       </ul>
       */
      class VisitingTool_test : public Test
        {
          virtual void run(Arg arg) 
            {
              UNIMPLEMENTED ("testing the generic visitor pattern");
              known_visitor_known_class();
              visitor_not_visiting_some_class();
              visitor_treating_new_subclass();
            } 
          
          void known_visitor_known_class()
            {
              UNIMPLEMENTED ("testing the generic visitor pattern");
            }
          
          void visitor_not_visiting_some_class()
            {
              UNIMPLEMENTED ("testing the generic visitor pattern");
            }
            
          void visitor_treating_new_subclass()
            {
              UNIMPLEMENTED ("testing the generic visitor pattern");
            }
        };
      
      
      /** Register this test class... */
      LAUNCHER (VisitingTool_test, "unit common");
      
      
      
    } // namespace test
    
  } // namespace builder

} // namespace mobject
