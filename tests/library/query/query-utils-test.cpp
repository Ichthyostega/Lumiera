/*
  QueryUtils(Test)  -  checking various utils provided for dealing with config queries

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file query-utils-test.cpp
 ** unit test \ref QueryUtils_test
 */


#include "lib/test/run.hpp"
#include "lib/util.hpp"
#include "lib/util-foreach.hpp"
#include "lib/query-util.hpp"
#include "lib/cmdline.hpp"
#include "lib/query-diagnostics.hpp"
#include "lib/format-cout.hpp"

#include <functional>

using lib::Cmdline;
using util::isnil;
using util::contains;
using util::for_each;

using std::placeholders::_1;
using std::bind;
using std::string;



namespace lib {
namespace query {
namespace test{
  
  
  struct Thing
    {
      virtual ~Thing() {} // add RTTI for Query.asKey();
    };
  
  
  
  /********************************************************************//**
   * @test check the various small helpers and utilities we utilise
   *       for dealing with ConfigQuery 
   */
  class QueryUtils_test : public Test
    {
      
      virtual void
      run (Arg arg) 
        {
          if (isnil(arg))  arg = Cmdline ("normaliseID extractID removeTerm countPred");
          
          if (contains (arg, "normaliseID"))   check_normaliseID();
          if (contains (arg, "extractID"  ))   check_extractID  ();
          if (contains (arg, "removeTerm" ))   check_removeTerm ();
          if (contains (arg, "countPred"  ))   check_countPred ();
        }
      
      
      
      /** @test sanitising and normalising various tokens */
      void
      check_normaliseID ()
        {
          Cmdline tokens ("a A AA dufte 1a _1 A_A BÄH");
          tokens.push_back ("");
          tokens.push_back ("  White \t space ");
          tokens.push_back ("§&Ω%€GΩ%€ar  ☠☠☠  baäääääge!!!!! ");
          
          cout << "..original : "<<tokens<<" :"<<endl;
          
          for_each (tokens, normaliseID, _1 );
          
          cout << "normalised : "<<tokens<<" :"<<endl;
        }
      
      
      
      /** @test the simple regexp extracting a parameter token */
      void
      check_extractID ()
        {
          CHECK ("tok" == extractID ("pred", "pred(tok)." ));
          CHECK ("tok" == extractID ("pred", "    pred( tok )" ));
          CHECK ("tok" == extractID ("pred", "pred(tok), pred(tux)." ));
          CHECK ("tok" == extractID ("pred", "other(xyz) pred(tok) pred(tux)" ));
          CHECK ("tok" == extractID ("pred", "some( pred(tok)" ));
          
          CHECK (isnil (extractID ("pred", "pred (tok)")));
          CHECK (isnil (extractID ("pred", "pred tok)" )));
          CHECK (isnil (extractID ("pred", "pred(tok " )));
        }
      
      
      
      /** @test the regexp based cutting of a term with given symbol */
      void
      check_removeTerm ()
        {
          // successful------Symbol---input-string----------------------extracted------remaining-------------
          CHECK_removeTerm ("pred", "pred(tok).",                     "pred(tok)",   "."                    );
          CHECK_removeTerm ("pred", "    pred( tok )",                "pred(tok)",   "    "                 );
          CHECK_removeTerm ("pred", "pred(tok), pred(tux).",          "pred(tok)",   "pred(tux)."           );
          CHECK_removeTerm ("pred", "other(xyz) pred(tok) pred(tux)", "pred(tok)",   "other(xyz) pred(tux)" );
          CHECK_removeTerm ("pred", "some( pred(tok)",                "pred(tok)",   "some( "               );
          
          // not successful
          CHECK_removeTerm ("pred", "pred (tok",                      "",            "pred (tok" );
          CHECK_removeTerm ("pred", "pred tok)",                      "",            "pred tok)" );
          CHECK_removeTerm ("pred", "pred(tok",                       "",            "pred(tok"  );
        }
      
      void
      CHECK_removeTerm (Symbol sym, string input, string extracted, string modified)
        {
          CHECK (extracted == removeTerm (sym, input));
          CHECK (modified  == input);
        }
      
      
      
      /** @test counting of predicates in a query
       *  (currently 4/08 regexp based...)
       */
      void
      check_countPred ()
        {
          for (uint i=1; i <= 30; ++i)
              CHECK ( i == countPred (garbage_query (i)));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (QueryUtils_test, "unit query");
  
  
  
}}} // namespace lib::query::test
