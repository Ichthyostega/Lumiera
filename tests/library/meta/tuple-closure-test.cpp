/*
  TupleClosure(Test)  -  appending, mixing and filtering typelists

   Copyright (C)
     2025,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file function-closure-test.cpp
 ** Unit test \ref TupleClosure_test demonstrates how to pre-bind
 ** some values for construction of _tuple-like_ objects.
 ** @see function-closure.hpp
 ** @see NodeBuilder_test::build_Node_closedParam() "usage example"
 **
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/meta/tuple-closure.hpp"
#include "lib/test/diagnostic-output.hpp"////////////////TODO




namespace lib  {
namespace meta {
namespace test {
  
  using std::string;
  using std::tuple;
  using std::make_tuple;
  using lib::meta::_Fun;
  using lib::test::showType;
  
  
  
  
  /*********************************************************************//**
   * @test wrap the constructors for »tuple-like« records as functor
   *       and pre-bind some arguments immediately.
   */
  class TupleClosure_test : public Test
    {
      virtual void
      run (Arg)
        {
          tuple_bindFront();
          tuple_bindBack();
        }
      
      
      /** @test use a regular tuple and pre-fix the first elements
       */
      void
      tuple_bindFront()
        {
          using Tup = tuple<int,double,string>;
          using Builder = TupleClosureBuilder<Tup>;
          
          auto cons = Builder::closeFront (1,2.3);
          using FunType = _Fun<decltype(cons)>;
          CHECK (FunType());
SHOW_EXPR(showType<FunType::Sig>())
          CHECK (showType<FunType::Sig>() == "tuple<int, double, string> (tuple<string>)"_expect);
          
          Tup tup = cons("five");
          CHECK (tup == "«tuple<int, double, string>»──(1,2.3,five)"_expect);
        }
      
      
      /** @test fix elements starting from the end of the tuple
       */
      void
      tuple_bindBack()
        {
          using Tup = tuple<int,double,string>;
          using Builder = TupleClosureBuilder<Tup>;
          
          auto c1 = Builder::closeBack("π");
          CHECK (showType<_Fun<decltype(c1)>::Sig>() == "tuple<int, double, string> (tuple<int, double>)"_expect);
          
          Tup t1 = c1(make_tuple (2,3.1415));
          CHECK (t1 == "«tuple<int, double, string>»──(2,3.1415,π)"_expect);
          auto c2 = Builder::closeBack(3.14159265,"pi");
          CHECK (showType<_Fun<decltype(c2)>::Sig>() == "tuple<int, double, string> (tuple<int>)"_expect);
          
          Tup t2 = c2(make_tuple (-1));
          CHECK (t2 == "«tuple<int, double, string>»──(-1,3.1415927,pi)"_expect);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TupleClosure_test, "unit common");
  
  
  
}}} // namespace lib::meta::test
