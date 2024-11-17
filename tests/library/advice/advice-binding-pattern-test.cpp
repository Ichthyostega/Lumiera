/*
  AdviceBindingPattern(Test)  -  cover pattern matching used to dispatch Advice

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file advice-binding-pattern-test.cpp
 ** unit test \ref AdviceBindingPattern_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/format-cout.hpp"

#include "lib/time/timevalue.hpp"
#include "common/advice.hpp"

using lib::time::Time;



namespace lumiera {
namespace advice {
namespace test  {
  
  namespace {
    class DummyAdvice { };
  }
  
  
  
  /***************************************************************************//**
   * @test the pattern matching machinery used to find an Advice solution.
   *       Each advice::Provision and advice::Request specifies a binding, used
   *       to discern various pieces of advice. Whenever patterns on the two sides
   *       match, an Advice channel is created, causing the advice provision to
   *       get visible to the advised entity.
   *       
   *       This test creates various patterns and verifies matching behaves
   *       as specified and documented.
   * 
   * @todo partially unimplemented and thus commented out ////////////////////TICKET #605
   * 
   * @see advice.hpp
   * @see AdviceBasics_test
   * @see AdviceMultiplicity_test
   * @see AdviceIndex_test implementation test
   */
  class AdviceBindingPattern_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          verifyPatternSyntax();
          verifyPatternNormalisation();
          verifyStaticMatch();
          verifyPreparedMatch();
          verifyDynamicMatch();
        }
      
      
      void
      verifyPatternSyntax()
        {
#define _PARSE_AND_SHOW(_STR_) \
          cout << _STR_ << "\t--->" << Binding(_STR_) << endl;
          
          _PARSE_AND_SHOW ("");
          _PARSE_AND_SHOW ("aSymbol");
          _PARSE_AND_SHOW ("a.compound_Symbol-with-various.parts");
          _PARSE_AND_SHOW ("trailing Garbage allowed. ☢☢ eat ☠☠☠ atomic ☠☠☠ waste ☢☢");
          _PARSE_AND_SHOW ("a, list , of ,symbols.");
          _PARSE_AND_SHOW ("nullary().");
          _PARSE_AND_SHOW ("nullary( )");
          _PARSE_AND_SHOW ("nullary  .");
          _PARSE_AND_SHOW ("predicate( with-argument )");
          
          VERIFY_ERROR (BINDING_PATTERN_SYNTAX, Binding("no (valid definition here)"));
          VERIFY_ERROR (BINDING_PATTERN_SYNTAX, Binding("predicate(with ☠☠☠ Garbage ☠☠☠"));
          VERIFY_ERROR (BINDING_PATTERN_SYNTAX, Binding("§&Ω%€GΩ%€ar☠☠☠baäääääge"));
          
          Binding testBinding;
          testBinding.addTypeGuard<DummyAdvice>();
          testBinding.addPredicate("one two(), three( four ).");
          
          cout << testBinding << endl;
        }
      
      
      void
      verifyPatternNormalisation()
        {
          Binding b0, b00;
          Binding b1 ("cat1(), cat2().");
          Binding b2 (" cat2 cat1 ....");
          
          cout << "b0==" << b0 << endl;
          cout << "b1==" << b1 << endl;
          cout << "b2==" << b2 << endl;
          
          
          CHECK (b0 == b00); CHECK (b00 == b0);
          CHECK (b1 == b2);  CHECK (b2 == b1);
          
          CHECK (b0 != b1);  CHECK (b1 != b0);
          CHECK (b0 != b2);  CHECK (b2 != b0);
          
          b2.addPredicate("cat1()");          // adding the same predicate multiple times has no effect 
          b2.addPredicate(" cat1 "); 
          CHECK (b1 == b2);
          b2.addPredicate("cat3(zzz)");
          CHECK (b1 != b2);
          
          b1.addTypeGuard<Time>();
          CHECK (b1 != b2);
          b1.addPredicate(" cat3(  zzz   )  ");
          CHECK (b1 != b2);
          b2.addTypeGuard<Time>();
          CHECK (b1 == b2);
          
          cout << "b2==" << b2 << endl;
        }
      
      
      void
      verifyStaticMatch()
        {
          CHECK ( matches (Binding(), Binding()));
          CHECK ( matches (Binding("pred()"),  Binding("pred(  ) ")));
          
          CHECK ( matches (Binding("pred(x)"), Binding("pred(x)")));
          CHECK (!matches (Binding("pred()"),  Binding("pred(x)")));
          CHECK (!matches (Binding("pred(x)"), Binding("pred(y)")));
          
          CHECK ( matches (Binding("pred(x), pred(y)"), Binding("pred(y), pred(x)")));
          CHECK (!matches (Binding("pred(x), pred(y)"), Binding("pred(y), pred(y)")));
        }
      
      
      void
      verifyPreparedMatch()
        {
          Binding b1 ("pred()");
          Binding b2 ("pred");
          Binding b3 ("pred, pred(x)");
          Binding b4 ("pred( x ) , pred().");
          CHECK ( matches (b1,b2));
          CHECK ( matches (b3,b4));
          
          Binding::Matcher bm1 (b1.buildMatcher());
          Binding::Matcher bm2 (b2.buildMatcher());
          Binding::Matcher bm3 (b3.buildMatcher());
          Binding::Matcher bm4 (b4.buildMatcher());
          
          CHECK (hash_value(b1) == hash_value(bm1));
          CHECK (hash_value(b2) == hash_value(bm2));
          CHECK (hash_value(b3) == hash_value(bm3));
          CHECK (hash_value(b4) == hash_value(bm4));
          
          CHECK (hash_value(b1) != hash_value(b3));
          
          CHECK ( matches (bm1,bm2));
          CHECK ( matches (bm3,bm4));
          CHECK (!matches (bm1,bm3));
          CHECK (!matches (bm2,bm4));
        }
      
      
      /** @test match against patterns containing variables,
       *        verify the created solution arguments
       *  @todo this is a future extension and its not clear
       *        if we need it and what the exact semantics
       *        could be  ///////////////////////////////TICKET #615
       */
      void
      verifyDynamicMatch()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #615
          CHECK ( matches (Binding("pred(u)"), Binding("pred(X)")));
          CHECK ( matches (Binding("pred(f(u))"), Binding("pred(f(X))")));
          CHECK ( matches (Binding("pred(f(u,Y))"), Binding("pred(f(X,v))")));
          CHECK ( matches (Binding("pred(f(u,X))"), Binding("pred(f(X,v))")));   // the so called "standardisation apart"
          
          CHECK (!matches (Binding("pred(u,v)"), Binding("pred(X)")));
          CHECK (!matches (Binding("pred(f(u))"), Binding("pred(f(v))")));
          CHECK (!matches (Binding("pred(f(u))"), Binding("pred(g(X))")));
          CHECK (!matches (Binding("pred(f(u,v))"), Binding("pred(f(X,X))")));
          
          //////TODO should also cover the difference between equality and match, which gets tangible only in conjunction with variables
          
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #615
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (AdviceBindingPattern_test, "unit common");
  
  
}}} // namespace lumiera::advice::test
