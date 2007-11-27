/*
  BuilderTool(Test)  -  specialized form of the acyclic visitor
 
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
#include "proc/mobject/builder/buildertool.hpp"
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
       * @test the generic visitor pattern specialized for treating
       *       MObjects in the builder. Because the generic visitor 
       *       implementation is already covered by
       *       \link VisitingTool_test, it is sufficient to test
       *       the specialisation to the builder
       * @todo work out what this means haha.... 
       */
      class BuilderTool_test : public Test
        {
          virtual void run(Arg arg) 
            {
              UNIMPLEMENTED ("testing the visitor pattern for the builder");
            } 
        };
      
      
      /** Register this test class... */
      LAUNCHER (BuilderTool_test, "unit builder");
      
      
      
    } // namespace test
    
  } // namespace builder

} // namespace mobject
