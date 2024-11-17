/*
  VisitingToolExtended(Test)  -  check obscure corner cases of visitor lib implementation

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file visitingtool-extended-test.cpp
 ** unit test \ref VisitingToolExtended_test
 */


#include "lib/test/run.hpp"
#include "lib/visitor.hpp"
#include "lib/format-string.hpp"

#include <iostream>

using util::_Fmt;
using std::string;
using std::cout;


namespace lib {
namespace visitor{
namespace test2  {
  
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

  
  template<class BASE>
  class VerboseVisitor
    : public BASE
    {
    protected:
      void talk_to (string guy)
        {
          cout << _Fmt{"Hello %s, nice to meet you...\n"} % guy;
        }
    };
    
  class Babbler
    : public Applicable< Babbler, 
                         Types<Boss,BigBoss>::List,    // treat this types
                         VerboseVisitor<Tool>         //  intermediary base class
                       >
    {
    public:
      void treat (Boss&)    { talk_to("Boss"); }
      void treat (BigBoss&) { talk_to("Big Boss"); }
    };

  // the classes above comprise the standard use case,
  // what follows covers rather exotic corner cases 
  
  
  
  /** defines an catch-all-function instead of the silent default error handler */
  template<class RET>
  struct Catched
    {
      RET onUnknown (HomoSapiens&) { cout << "we-do-everything-for-YOU!\n"; return RET(); }
    };
    
  /** defines another different visiting tool base */  
  typedef visitor::Tool<void, Catched> Hastalavista;
  typedef Visitable<Hastalavista> Chief;  ///< another special kind of visitable

#define DEFINE_HASTALAVISTA_PROCESSABLE         \
    virtual void  apply (Hastalavista& tool) \
    { return Chief::dispatchOp (*this, tool); }
  
  
  /** now mixing the two hierarchies... */
  class Leader : public Chief,
                 public Boss  ///< can act as HomoSapiens or as Chief
    {
    public:
      using HomoSapiens::apply;
      DEFINE_HASTALAVISTA_PROCESSABLE;
    };
  
  class Visionary : public Leader
    {
      DEFINE_HASTALAVISTA_PROCESSABLE;
    };
  
  /** Hastalavista-Visiting-Tool
   *  tailored for the Chief hierarchy
   */  
  class Blatherer
    : public Applicable< Blatherer, 
                         Types<Visionary>::List,         // get calls to Visionary dispatched 
                         VerboseVisitor<Hastalavista>   // note: different tool base class 
                       >
    {
    public:
      void treat (Leader&)  { talk_to("Mr.Future"); }
    };
  
  
        

  
  
  
  /*********************************************************************//**
   * @test more esoteric corner cases of our visitor lib implementation.
   *       Defines a hierarchy of test classes, which mix two different
   *       kinds of "visitable" by two disjoint tool base classes. One
   *       of these base classes uses an explicit error handling
   *       catch-all-function.
   */
  class VisitingToolExtended_test : public Test
    {
      virtual void run(Arg) 
        {
          known_visitor_known_class();
          visitor_not_visiting_some_class();
          visiting_mixed_hierarchy();
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
        
      void visiting_mixed_hierarchy()
        {
          Leader x1;
          Visionary x2;
          
          HomoSapiens& homo1 (x1);
          HomoSapiens& homo2 (x2);
          Chief& chief1 (x1);
          Chief& chief2 (x2);
          Leader& lead1 (x1);
          Leader& lead2 (x2);
          
          Blatherer bla;
          cout << "=== Blatherer meets Leader and Visionary masqueraded as Chief ===\n";
          chief1.apply (bla);  // catch-all, because Blatherer doesn't declare to be applicalbe to Leader
          chief2.apply (bla);  // treat(Visionary&) resolved to treat(Leader&) as expected
          
          Babbler bab;
          Tool& tool1 (bab);
          cout << "=== Babbler masqueraded as Tool meets Leader and Visionary masqueraded as HomoSapiens ===\n";
          homo1.apply (tool1); // because just going through the VTable, the dispatch works as expected 
          homo2.apply (tool1); // same here (in both cases, the call is resolved to treat(Boss&) as expected)
          
          cout << "=== Babbler masqueraded as Tool meets Leader and Visionary masqueraded as Leader ===\n";
          lead1.apply (tool1); // nothing happens, because Leader here is treated by his HomoSapiens base
          lead2.apply (tool1); // surprisingly the VTable mechanism is choosen here, resulting in an correct dispatch
          
          // note: the following doesn't compile (an this is a feature, not a bug):
          
          // "Chief chief"           : is abstract, because the Visitable-Template enforces implementing 
          //                           the "apply(TOOL&)" function, either directly or via the 
          //                           DEFINE_PROCESSABLE_BY macro

        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (VisitingToolExtended_test, "unit common");
  
  
  
}}} // namespace lib::visitor::test2
