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


#include "test/run.hpp"
#include "test/test-helper.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "meta/typelist-diagnostics.hpp"
#include "meta/tuple-diagnostics.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/hetero-data.hpp"

#include <string>

using test::showSizeof;
using test::showType;
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
          verify_tuple_like_concept();
        }
      
      
      /** verify the test input data
       *  @see TypeListManip_test::check_diagnostics()
       *       for an explanation of the DISPLAY macro
       */
      void
      check_diagnostics ()
        {
          using L1 = Types1::List;
          using L2 = Types2::List;
          using L3 = Types3::List;
          
          DISPLAY (L1);
          DISPLAY (L2);
          DISPLAY (L3);
          
          using Tup1 = Tuple<Types1>;
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
      
      
      
      template<typename X>
      string
      render()
        {
          return showType<X>();
        }
      
      template<tuple_like X>
      string
      render()
        {
          string res{"Tup"};
          res +="("+toString(std::tuple_size_v<X>)+") : "+ showType<X>();
          lib::meta::forEachIDX<X> ([&](auto i)
                                      {
                                        using Elm = std::tuple_element_t<i, X>;
                                        res += " ▷"+ toString(uint(i)) + ": " + showType<Elm>();
                                      });
          return res;
        }

      /** @test verify construction of a concept to detect _tuple-like_ classes,
       *        which conform to the »tuple protocol« and can thus be used
       *        in structural bindings
       *      - the size of such an entity can be detected at compile time
       *      - essentially, these can be considered as _product types_ — which implies
       *        that there are N element types
       *      - access to the corresponding member data is possible either through a
       *        `get` member function or free function detected by ADL
       */
      void
      verify_tuple_like_concept()
        {
          using Tup = std::tuple<long,short>;
          using Arr = std::array<int,3>;
          using Het = lib::HeteroData<int,string>::Chain<short>::ChainExtent<bool,Nil>::ChainType;
          
          CHECK (    tuple_sized<Tup> );
          CHECK (    tuple_sized<Arr> );
          CHECK (    tuple_sized<Het> );
          CHECK (not tuple_sized<int> );
          
          CHECK (   (tuple_element_accessible<Tup,0>));
      //  CHECK (   (tuple_element_accessible<Tup,2>));
          CHECK (   (tuple_element_accessible<Het,0>));
          CHECK (    tuple_accessible<Tup> );
          CHECK (    tuple_accessible<Arr> );
          CHECK (    tuple_accessible<Het> );
          CHECK (not tuple_accessible<int> );
          
          // verify the concept detects various tuple-like types
          CHECK (    tuple_like<Tup> );
          CHECK (    tuple_like<Arr> );
          CHECK (    tuple_like<Het> );
          CHECK ((   tuple_like<Tuple<Types<int,float>::List>>));
          CHECK ((   tuple_like<std::tuple<int>              >));
          CHECK ((   tuple_like<std::tuple<int,char,long>    >));
          CHECK ((   tuple_like<std::tuple<>                 >));
          CHECK ((   tuple_like<std::pair<short,long>        >));
          CHECK ((   tuple_like<std::array<short,5>          >));
          CHECK ((   tuple_like<std::array<long,0>           >));
          CHECK ((   tuple_like<HeteroData<size_t>           >));
          CHECK ((   tuple_like<HeteroData<int,char>         >));
          CHECK ((   tuple_like<HeteroData<>                 >));
          
          // verify arbitrary non-structured types
          CHECK (not tuple_like<int            >);
          CHECK (not tuple_like<void           >);
          CHECK (not tuple_like<void*          >);
          CHECK (not tuple_like<const void*    >);
          CHECK (not tuple_like<const int      >);
          CHECK (not tuple_like<int            >);
          CHECK (not tuple_like<int &          >);
          CHECK (not tuple_like<int const &    >);
          CHECK (not tuple_like<int const *    >);
          CHECK (not tuple_like<int *          >);
          CHECK (not tuple_like<int * const    >);
          CHECK (not tuple_like<int * const &  >);
          CHECK (not tuple_like<int * &        >);
          CHECK (not tuple_like<int * &&       >);
          CHECK (not tuple_like<int &&         >);
          CHECK (not tuple_like<int const &&   >);
          CHECK (not tuple_like<double         >);
          CHECK (not tuple_like<string         >);
          CHECK((not tuple_like<Nil            >));
          CHECK((not tuple_like<Node<short,Nil>>));
          
          // the tuple, the array and the HeteroData are tuple-like,
          // and will be handled by a special overload, exploiting the additional features
          CHECK (render<Tup>() == "Tup(2) : tuple<long, short> ▷0: long ▷1: short"_expect);
          CHECK (render<Arr>() == "Tup(3) : array<int, 3ul> ▷0: int ▷1: int ▷2: int"_expect);
          CHECK (render<Het>() == "Tup(5) : HeteroData<Node<StorageFrame<0ul, int, string>, "
                                           "Node<StorageFrame<1ul, short>, "
                                           "Node<StorageFrame<2ul, bool, Nil>, Nil> > > > "
                                  "▷0: int ▷1: string ▷2: short ▷3: bool ▷4: Nil"_expect);
          // the plain type `int` is not tuple-like and thus the fallback case is picked
          CHECK (render<int>() == "int"_expect);
          
          CHECK (std::tuple_size_v<const Tup> == 2 );
          
          using Elm1 = std::tuple_element_t<1, const Tup>;
          CHECK (showType<Elm1>() == "const short"_expect);
          
          // note: a const tuple will add const qualification to each element type
          using TupConstSeq = lib::meta::ElmTypes<const Tup>::Seq;
          CHECK (showType<TupConstSeq>() == "Types<long const, short const>"_expect);
          
          
          // a unified access function `getElm`
          // which works both with access by member or free function
          
          using T1 = decltype(lib::meta::getElm<0> (std::declval<Tup>()));
          CHECK (showType<T1>() == "long &&"_expect);
          
          using T2 = decltype(lib::meta::getElm<0> (std::declval<Tup&>()));
          CHECK (showType<T2>() == "long&"_expect);
          
          using T3 = decltype(lib::meta::getElm<0> (std::declval<Tup const&>()));
          CHECK (showType<T3>() == "long const&"_expect);
          
          using H1 = decltype(lib::meta::getElm<4> (std::declval<Het>()));
          CHECK (showType<H1>() == "Nil"_expect);
          
          using H2 = decltype(lib::meta::getElm<4> (std::declval<Het&>()));
          CHECK (showType<H2>() == "Nil&"_expect);
          
          using H3 = decltype(lib::meta::getElm<4> (std::declval<Het const&>()));
          CHECK (showType<H3>() == "Nil const&"_expect);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TupleHelper_test, "unit meta");
  
  
  
}}} // namespace lib::meta::test
