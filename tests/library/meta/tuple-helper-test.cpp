/*
  TupleHelper(Test)  -  verify helpers for working with tuples and type sequences

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file tuple-helper-test.cpp
 ** Interplay of typelists, type tuples and std::tuple.
 ** 
 ** @see tuple-helper.hpp
 ** @see function-closure.hpp
 ** @see control::CmdClosure real world usage example
 **
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "meta/typelist-diagnostics.hpp"
#include "meta/tuple-diagnostics.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/test/diagnostic-output.hpp"////////////////TODO

#include <string>

using lib::test::showSizeof;
using util::toString;
using util::_Fmt;
using std::is_same_v;
using std::make_tuple;
using std::get;

namespace lib  {
namespace meta {
namespace test {
  
  
  
  namespace { // test data
    
    
    typedef Types< Num<1>
                 , Num<3>
                 , Num<5>
                 >         Types1;
    typedef Types< Num<2>
                 , Num<4>
                 >         Types2;
    typedef Types< Num<7>> Types3;
    
    
    
  } // (End) test data
  
  
  
  /*********************************************************************//**
   * @test Cover various aspects of the integration of our type sequences
   *       with the tuple type from the standard library
   *       - verify our generic tuple access decorator
   *       - verify generating tuple types from type list processing
   *       - TODO more helpers to come
   */
  class TupleHelper_test : public Test
    {
      virtual void
      run (Arg)
        {
          check_diagnostics();
          check_tuple_from_Typelist();
          demonstrate_generic_iteration();
        }
      
      
      /** verify the test input data
       *  @see TypeListManip_test::check_diagnostics()
       *       for an explanation of the DISPLAY macro
       */
      void
      check_diagnostics ()
        {
          typedef Types1::List L1;
          typedef Types2::List L2;
          typedef Types3::List L3;
          
          DISPLAY (L1);
          DISPLAY (L2);
          DISPLAY (L3);
          
          typedef Tuple<Types1> Tup1;
          Tup1 tup1x (Num<1>(11), Num<3>(), 55);
          
          DISPLAY (Tup1);     // prints the type
          DUMPVAL (Tup1());   // prints the contents
          DUMPVAL (tup1x);
          
          EXPECT (Types1,    "-<1>-<3>-<5>-");
          EXPECT (Tup1, "TUPLE-<1>-<3>-<5>-");
          CHECK  (get<2>(tup1x) == Num<5>{55});
          CHECK  (toString(tup1x) == "«tuple<Num<1>, Num<3>, Num<5> >»──({11},(3),{55})"_expect);
        }
      
      
      /** @test verify the ability to generate tuple types from typelist metaprogramming
       *        - the resulting types are plain flat `std::tuple` instantiations
       *        - memory layout is identical to a POD, as expected
       *        - our generic string conversion is extended to work with tuples
       */
      void
      check_tuple_from_Typelist()
        {
          using L1 = Types1::List;          // ... start from existing Typelist...
          
          using T_L1 = Tuple<L1>;           // derive a tuple type from this typelist
          using Seq1 = RebindTupleTypes<T_L1>::Seq;
                                            // extract the underlying type sequence
          EXPECT (T_L1, "TUPLE-<1>-<3>-<5>-");
          EXPECT (Seq1, "-<1>-<3>-<5>-");
          
          T_L1 tup1;                        // can be instantiated at runtime (and is just a std:tuple)
          CHECK (toString(tup1) == "«tuple<Num<1>, Num<3>, Num<5> >»──((1),(3),(5))"_expect);
          
          using Prepend = Tuple<Node<int, L1>>;
                                            // another ListType based tuple created by prepending
          EXPECT (Prepend, "TUPLE-<i>-<1>-<3>-<5>-");
          
          Prepend prep (22, 11,33,Num<5>());
          CHECK (toString(prep) == "«tuple<int, Num<1>, Num<3>, Num<5> >»──(22,{11},{33},(5))"_expect);
          
          using NulT = Tuple<Types<> >;     // plain-flat empty Tuple
          using NulL = Tuple<Nil>;          // list-style empty Tuple
          
          NulT nulT;                        // and these, too, can be instantiated
          NulL nulL;
          CHECK (toString(nulT) == "«tuple<>»──()"_expect);
          CHECK (toString(nulL) == "«tuple<>»──()"_expect);
          CHECK ((is_same<NulT, NulL>()));
          
          using S4 = struct{int a,b,c,d;};  // expect this to have the same memory layout
          CHECK (sizeof(S4) == sizeof(prep));
          CHECK (1 == sizeof(nulL));        // ...minimal storage, as expected
          
          
          CHECK (is_Tuple<T_L1>());
          CHECK (is_Tuple<Prepend>());
          CHECK (is_Tuple<NulT>());
          CHECK (not is_Tuple<Seq1>());
          
          cout << tup1 <<endl               // these automatically use our generic string conversion
               << prep <<endl
               << nulL <<endl;
          
          cout << showSizeof(tup1) <<endl
               << showSizeof(prep) <<endl
               << showSizeof(nulT) <<endl
               << showSizeof(nulL) <<endl;
          
          CHECK (sizeof(prep) == sizeof(int)+sizeof(Num<1>)+sizeof(Num<3>)+sizeof(Num<5>));
        }
      
      
      /** @test demonstrate generic tuple iteration mechanisms
       *      - apply a generic lambda to each element of a tuple
       *      - iterate over all index numbers of a tuple (or tuple-like)
       * @remark the _iteration_ happens at compile-time, i.e. the given
       *      lambda-template is instantiated for each type of the tuple;
       *      however, at runtime, each of these instantiations is invoked
       *      in sequence, performing the body of the λ-function, which may
       *      access the actual data value for each of the tuple elements.
       */
      void
      demonstrate_generic_iteration()
        {
          auto tup = make_tuple (1, 2.3, '4');
          using Tup = decltype(tup);
          CHECK (toString(tup) == "«tuple<int, double, char>»──(1,2.3,4)"_expect);
          
          string dump{};
          CHECK (dump == "");
          // apply λ-generic to each element...
          forEach (tup, [&](auto elm){ dump += "|"+toString(elm); });
          CHECK (dump == "|1|2.3|4"_expect);

          // apply λ-generic to each element and build new tuple from results
          auto tupStr = mapEach (tup, [](auto const& elm){ return toString(elm); });
          CHECK (tupStr == "«tuple<string, string, string>»──(1,2.3,4)"_expect);
          
          _Fmt showElm{"|%d|▷%s◁"};
          dump = "";
          // apply λ-generic with (constexpr) index
          forEachIDX<Tup> ([&](auto idx)
                            {
                              using Idx = decltype(idx);
                              using Iii = std::integral_constant<size_t, idx.value>;
                              CHECK ((is_same_v<Idx, Iii>));
                              dump += showElm % uint(idx) % get<idx>(tup);
                            });
          CHECK (dump == "|0|▷1◁|1|▷2.3◁|2|▷4◁"_expect);
          
          
          // apply λ-generic and combine results
          auto boolFun = [&](auto idx){ return bool(get<idx>(tup)); };
          
          CHECK (    andAllIDX<Tup> (boolFun));
          get<0>(tup) = 0;                     // ◁————————————————————— demonstrate that a run-time evaluation happens
          CHECK (not andAllIDX<Tup> (boolFun));
          CHECK (    orAnyIDX<Tup> (boolFun));
          
          // can use this mechanism also for sole compile-time computation
          auto integralCheckFun = [](auto idx){ return std::is_integral_v<std::tuple_element_t<idx,Tup>>;};
          
          CHECK (not andAllIDX<Tup> (integralCheckFun));
          CHECK (    orAnyIDX<Tup> (integralCheckFun));
          
          // Note however, the same can also be achieved simpler,
          // using any meta-function which takes a single type argument...
          CHECK (not ElmTypes<Tup>::AndAll<std::is_integral>());  //   ... __and_<is_integral<int>,is_integral<double>,is_integral<char>>
          CHECK (    ElmTypes<Tup>::OrAll<std::is_integral>());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TupleHelper_test, "unit meta");
  
  
  
}}} // namespace lib::meta::test
