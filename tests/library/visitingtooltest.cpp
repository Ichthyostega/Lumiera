/*
  VisitingTool(Test)  -  check the standard visitor use case

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


#include "lib/test/run.hpp"
#include "lib/visitor.hpp"

#include <boost/format.hpp>
#include <iostream>

using boost::format;
using std::string;
using std::cout;


namespace lib {
namespace visitor {
namespace test1 {
  
  typedef visitor::Tool<> VisitingTool;
  
  class HomoSapiens : public Visitable<>
    {
    public:
      DEFINE_PROCESSABLE_BY (VisitingTool);
    };
  
  class Boss : public HomoSapiens
    {
    public:
      DEFINE_PROCESSABLE_BY (VisitingTool);
    };
  
  class BigBoss : public Boss
    {
    public:
      DEFINE_PROCESSABLE_BY (VisitingTool);
    };
  
  class Visionary : public Boss
    {
      DEFINE_PROCESSABLE_BY (VisitingTool);
    };
  
  class Leader : public Visionary
    {
    };
  
  
  
  class VerboseVisitor
    : public VisitingTool
    {
    protected:
      void talk_to (string guy)
        {
          cout << format ("Hello %s, nice to meet you...\n") % guy;
        }
    };
  
  class Babbler
    : public Applicable< Babbler
                       , Types<Boss,BigBoss,Visionary>::List   // dispatch calls to this types
                       , VerboseVisitor
                       >
    {
    public:
      void treat (Boss&)    { talk_to("Boss"); }
      void treat (BigBoss&) { talk_to("Big Boss"); }
    };
  
  // note the following details:
  // - Babbler "forgot" to declare being applicable to HomoSapiens
  // - we have new derived class Leader without separate "apply()"-implementation 
  
  
  
  
  /*********************************************************************//**
   * @test our lib implementation of the visitor pattern.
   *       Defines a hierarchy of test classes to check the following cases
   *       - calling the correct visiting tool specialised function
   *         for given concrete hierarchy classes
   *       - visiting tool not declaring to visit some class 
   *         is silently ignored by default
   *       - newly added classes will be handled by existing
   *         functions according to inheritance relations
   */
  class VisitingTool_test : public Test
    {
      virtual void
      run (Arg) 
        {
          known_visitor_known_class();
          visiting_extended_hierarchy();
        }
      
      void
      known_visitor_known_class()
        {
          Boss x1;
          BigBoss x2;
          
          // masquerade as HomoSapiens...
          HomoSapiens& homo1 (x1);
          HomoSapiens& homo2 (x2);
          
          cout << "=== Babbler meets Boss and BigBoss ===\n";
          Babbler bab;
          VisitingTool& vista (bab);
          homo1.apply (vista);
          homo2.apply (vista);
        }
      
      void
      visiting_extended_hierarchy()
        {
          HomoSapiens x1;
          Leader x2;
          
          HomoSapiens& homo1 (x1);
          HomoSapiens& homo2 (x2);
          
          cout << "=== Babbler meets HomoSapiens and Leader ===\n";
          Babbler bab;
          VisitingTool& vista (bab);
          homo1.apply (vista);  // silent error handler (not Applicable to HomoSapiens)
          homo2.apply (vista); //  Leader handled as Visionary and treated as Boss
        }
        
    };
  
  
  /** Register this test class... */
  LAUNCHER (VisitingTool_test, "unit common");
  
  
  
}}} // namespace lib::visitor::test1
