/*
  VisitingToolExtended(Test)  -  check obscure corner cases of visitor lib implementation
 
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
      typedef visitor::Tool<> Tool;
      
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

        
      class VerboseVisitor
        : public Tool
        {
        protected:
          void talk_to (string guy)
            {
              cout << format ("Hello %s, nice to meet you...\n") % guy;
            }
        };
        
      class Babbler
        : public Applicable<Boss,Babbler>,
          public Applicable<BigBoss,Babbler>,
          public ToolType<Babbler, VerboseVisitor>
        {
        public:
          void treat (Boss&)    { talk_to("Boss"); }
          void treat (BigBoss&) { talk_to("Big Boss"); }
        };

      // the classes above comprise the standard use case,
      // what follows are rather exotic corner cases 
        
      class Blatherer
        : public Applicable<BigBoss,Blatherer>,
          public ToolType<Blatherer, VerboseVisitor>
        {
        public:
          void treat (BigBoss&)     { talk_to("big Boss"); }
          void treat (HomoSapiens&) { talk_to("we-do-everything-for-YOU"); } ///< catch-all function
          void catchy(HomoSapiens&) {}
        };
        
      
      typedef Visitable<void,Blatherer,InvokeCatchAllFunction> Vista2;
      
      class Chief : public Vista2 ///< abstract intermeidary node 
        {
        };
      
      class Leader : public Chief,
                     public Boss  ///< can act as HomoSapiens or as Chief
        {
        public:
          using HomoSapiens::apply;
          virtual void apply (Blatherer& tool)  { return Vista2::dispatchOp (*this, tool); }
        };
        
      class Visionary : public Leader
        {
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
      class VisitingToolExtended_test : public Test
        {
          virtual void run(Arg arg) 
            {
              //known_visitor_known_class();
              //visitor_not_visiting_some_class();
              //visitor_treating_new_subclass();
            } 
          
          void known_visitor_known_class()
            {
              Boss x1;
              BigBoss x2;
              
              // masquerade as HomoSapiens...
              HomoSapiens& homo1 (x1);
              HomoSapiens& homo2 (x2);
              
              cout << "=== Babbler meets Boss and BigBoss ===\n";
              Babbler bab;
              homo1.apply (bab);
              homo2.apply (bab);
            }
          
          void visitor_not_visiting_some_class()
            {
              HomoSapiens x1;
              Leader x2;
              
              HomoSapiens& homo1 (x1);
              HomoSapiens& homo2 (x2);
              
              cout << "=== Babbler meets HomoSapiens and Leader ===\n";
              Babbler bab;
              homo1.apply (bab);  // doesn't visit HomoSapiens
              homo2.apply (bab); //  treats Leader as Boss
            }
            
          void visitor_treating_new_subclass()
            {
              Leader x1;
              Visionary x2;
              HomoSapiens x3;
              
              HomoSapiens& homo1 (x1);
              HomoSapiens& homo2 (x2);
              HomoSapiens& homo3 (x3);
              Chief& chief1 (x1);
              Chief& chief2 (x2);
              Leader& lead1 (x1);
              Leader& lead2 (x2);
              
              Blatherer bla;
              VerboseVisitor vista;
              Tool& tool1 (vista);
              Tool& tool2 (bla);
              cout << "=== Blatherer meets Leader, Visionary and HomoSapiens masqueraded as HomoSapiens ===\n";
              homo1.apply (bla);  // nothing happens, because Blatherer doesn't declare to do anything as Tool 
              homo2.apply (bla);
              homo3.apply (bla);
              cout << "=== Blatherer meets Leader and Visionary masqueraded as Chief ===\n";
              chief1.apply (bla);  // but now, acting in the Chief hierarchy, the catch-all is called
              chief2.apply (bla);
              cout << "=== VerboseVistr masqueraded as Tool meets Leader and Visionary masqueraded as HomoSapiens ===\n";
              homo1.apply (tool1); // because acting in the HomoSapiens hierarch, no visiting happens and no catch-all 
              homo2.apply (tool1);
              cout << "=== Blatherer masqueraded as Tool meets Leader and Visionary masqueraded as Leader ===\n";
              lead1.apply (tool2); // nothing happens, because Leader here is treated by his HomoSapiens base
              lead2.apply (tool2);

              // note: the following doesn't compile (an this is a feature, not a bug):
              
              // "chief1.apply (tool2)"  : because the "Chief"-hierarchy enforces the catch-all function
              //                           and the compiler doesn't know Blatherer actually implements this 
              //                           catch-all-function, because of the masqueradeing as Tool. Note 
              //                           further: the catch-all function can have a more general type
              //                           (in this case HomoSapiens instead of Chief)
              
              // "Chief chief"           : is abstract, because the Visitable-Template enforces implementing 
              //                           the "apply(TOOL&)" function, either directly or via the 
              //                           DEFINE_PROCESSABLE_BY macro

            }
        };
      
      
      /** Register this test class... */
//      LAUNCHER (VisitingToolExtended_test, "unit common");
      
      
      
    } // namespace test
    
  } // namespace visitor

} // namespace cinelerra
