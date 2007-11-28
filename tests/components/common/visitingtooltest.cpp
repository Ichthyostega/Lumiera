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

#include <boost/format.hpp>
#include <iostream>

using boost::format;
using std::string;
using std::cout;


namespace cinelerra
  {
  namespace visitor
    {
    namespace test
      {
      
      
      class HomoSapiens : public Visitable<>
        {
        public:
          DEFINE_PROCESSABLE_BY (Tool);
        };
      
      class Boss : public HomoSapiens
        {
        public:
          DEFINE_PROCESSABLE_BY (Tool);
        };
      
      class BigBoss : public Boss
        {
        public:
          DEFINE_PROCESSABLE_BY (Tool);
        };
      
      class Leader : public Boss
        {
        };
      
        
      class VerboseVisitor
        : public Tool,
          public Applicable<Boss>,
          public Applicable<BigBoss>
        {
          void talk_to (string guy)
            {
              cout << format ("Hello %s, nice to meet you...\n") % guy;
            }
        public:
          void treat (Boss&)    { talk_to("Boss"); }
          void treat (BigBoss&) { talk_to("big Boss"); }
        };
        
      
      
      
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
              known_visitor_known_class();
              TODO ("implement the more complicated visitor test cases");
              //visitor_not_visiting_some_class();
              //visitor_treating_new_subclass();
            } 
          
          void known_visitor_known_class()
            {
              HomoSapiens x1;
              Boss x2;
              BigBoss x3;
              
              HomoSapiens& xx2 (x2);
              HomoSapiens& xx3 (x3);
              
              VerboseVisitor wizzy;
              x1.apply (wizzy);
              xx2.apply (wizzy);
              xx3.apply (wizzy);
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
    
  } // namespace visitor

} // namespace cinelerra
