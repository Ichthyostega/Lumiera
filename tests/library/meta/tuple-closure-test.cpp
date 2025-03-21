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
#include "lib/format-util.hpp"


namespace lib  {
namespace meta {
namespace test {
  
  using std::string;
  using std::tuple;
  using std::array;
  using std::make_tuple;
  using lib::meta::_Fun;
  using lib::test::showType;
  
  
  
  
  /*********************************************************************//**
   * @test wrap the constructors for »tuple-like« records as functor
   *       and pre-bind some arguments immediately.
   *     - verify binding flavours for a tuple with mixed types
   *     - verify binding also works seamlessly with std::array
   */
  class TupleClosure_test : public Test
    {
      virtual void
      run (Arg)
        {
          tuple_bindFront();
          tuple_bindBack();
          tuple_bindArg();
          array_bindFront();
          array_bindArg();
          verify_AdaptArray();
        }
      
      
      /** @test use a regular tuple and pre-fix the first elements */
      void
      tuple_bindFront()
        {
          using Tup = tuple<int,double,string>;
          using Builder = TupleClosureBuilder<Tup>;
          
          auto cons = Builder::closeFront (1,2.3);
          using FunType = _Fun<decltype(cons)>;
          CHECK (FunType() == true);       // indeed a function
          CHECK (showType<FunType::Sig>() == "tuple<int, double, string> (tuple<string>)"_expect);
          
          Tup tup = cons("five");
          CHECK (tup == "«tuple<int, double, string>»──(1,2.3,five)"_expect);
        }
      
      
      /** @test fix elements starting from the end of the tuple */
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
      
      
      /** @test fix specific argument within tuple */
      void
      tuple_bindArg()
        {
          using Tup = tuple<int,double,string>;
          using Builder = TupleClosureBuilder<Tup>;
          
          auto c1 = Builder::close<1>(3.1415927);
          CHECK (showType<_Fun<decltype(c1)>::Sig>() == "tuple<int, double, string> (tuple<int, string>)"_expect);
          
          Tup t1 = c1({2,"π"});
          CHECK (t1 == "«tuple<int, double, string>»──(2,3.1415927,π)"_expect);
          
          auto c2 = Builder::close<3>("fantastic");
          // Binding to out-of-scope arguments is ignored: the result is the identity-function
          CHECK (showType<_Fun<decltype(c2)>::Sig>() == "tuple<int, double, string> (tuple<int, double, string>)"_expect);
        }
      
      
      /** @test use a std::array and handle it like a tuple to pre-fix some elements */
      void
      array_bindFront()
        {
          using Arr = array<int,5>;
          using Builder = TupleClosureBuilder<Arr>;
          
          auto cons = Builder::closeFront (1u,2.3);
          CHECK (showType<_Fun<decltype(cons)>::Sig>() == "ArrayAdapt<int, int, int, int, int> (ArrayAdapt<int, int, int>)"_expect);
          
          Arr arr = cons({3,4,5});
          CHECK (arr == "[1, 2, 3, 4, 5]"_expect);
        }
      
      
      /** @test can also use the binding for arbitrary elements in a std::array */
      void
      array_bindArg()
        {
          using Arr = array<int,5>;
          using Builder = TupleClosureBuilder<Arr>;
          
          auto cons = Builder::close<3>(55);
          CHECK (showType<_Fun<decltype(cons)>::Sig>() == "ArrayAdapt<int, int, int, int, int> (ArrayAdapt<int, int, int, int>)"_expect);
          
          Arr arr = cons(array{1,2,3,4});
          CHECK (arr == "[1, 2, 3, 55, 4]"_expect);
        }
      
      
      
      /** @test verify properties of the metaprogramming-adapter,
       *        used as seamless overlay to handle std::array
       *        in the TUpleClosureBuilder.
       */
      void
      verify_AdaptArray()
        {
          // can be constructed from aggregate
          ArrayAdapt arr{1,2,3,4,5};
          CHECK (arr.size() == 5);
          
          // picks up a tuple-loke type signature
          using AA = decltype(arr);
          CHECK (showType<AA>() == "ArrayAdapt<int, int, int, int, int>"_expect );
          CHECK (showType<AA::value_type>() == "int"_expect );
          
          // can use subscript operator from underlying array
          CHECK (arr[0] == 1);
          CHECK (arr[2] == 3);
          CHECK (arr[4] == 5);
          // can use the tuple-like binding defined for array
          CHECK (std::get<0>(arr) == 1);
          CHECK (std::get<2>(arr) == 3);
          CHECK (std::get<4>(arr) == 5);
          
          // supports structured bindings
          auto& [v1,v2,v3,v4,v5] = arr;
          CHECK (v3 == 3);
          v3 = 33;
          CHECK (arr[2] == 33);
          
          // can copy-assign from std::array
          arr = array{5,4,3,2,1};
          CHECK (arr[0] == 5);
          CHECK (arr[4] == 1);
          
          // can copy/move-construct from std::array
          AA axx{array{-1,-2,-3,-4,-5}};
          CHECK (axx[0] == -1);
          CHECK (axx[2] == -3);
          CHECK (axx[4] == -5);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TupleClosure_test, "unit common");
  
  
  
}}} // namespace lib::meta::test
