/*
  CommonResult(Test)  -  verify reconciled function return types

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file common-result-test.cpp
 ** unit test \ref CommonResult_test
 */



#include "test/run.hpp"
#include "test/test-helper.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/random.hpp"

#include "lib/meta/common-result.hpp"

#include <string>


using test::showType;
using std::string;
using std::tuple;


namespace lib  {
namespace meta {
namespace test {
  
  namespace { // Dummy hierarchy
    
    struct II      { };
    struct S1 : II { };
    struct S2 : II { };
    
    template<typename X>
    X yield();
  }
  
  
  
  /*******************************************************************//**
   * @test demonstrate the metafunction to determine a common return type.
   * @see TupleIdxAdaptor_test
   * @see IterExplorer_test::verify_expandOperation
   * @see common-result.hpp
   * @see iter-explorer.hpp
   */
  class CommonResult_test : public Test
    {
      virtual void
      run (Arg)
        {
          simpleUsage();
          underlyingMechanism();
          demonstrate_TypeReconciliation();
          demonstrate_Metaprocessing();
        }
      
      
      
      /** @test how to use the metafunction and the concept */
      void
      simpleUsage()
        {
          using CT = CommonResult_t<int,char,long,short>;
          CHECK (showType<CT>() == "long"_expect );
          
          CHECK ((not has_CommonResult_v<int,string>));
          CHECK ((not reconcilable<int,string>));           // Concept
          
          CHECK ((reconcilable_ref<short&, short const&>)); // Concept
        }
      
      
      
      /** @test show the language mechanism used as foundation */
      void
      underlyingMechanism()
        {
          auto expr = defaultGen.i(2)? defaultGen.u64() : defaultGen.uni();
          CHECK (expr >= 0);
          CHECK (showType<decltype(expr)>() == "double"_expect );
          
          CHECK (showType<decltype( true? short(1) : long(1))>() == "long"_expect );
          CHECK (showType<decltype(false? short(1) : long(1))>() == "long"_expect );

          CHECK (showType<decltype( true? yield<const string>() : yield<string&>())>() == "const string"_expect );

          CHECK (showType<decltype( true? yield<II> () : yield<S1>())>() == "II"_expect );
          CHECK (showType<decltype( true? yield<II&>() : yield<S1>())>() == "II"_expect );
        }
      
      
      /** @test show how various combinations of types are handled
       *      - some types can not be reconciled
       *      - there is no common type for an empty sequence of types
       *      - result falls back to return by-value (or RRef),
       *        unless all involved types are given by LRef
       *      - types that must be converted are reconciled into a value
       *      - C++ allows to promote integral numbers to floating-point
       *      - superclass / subclass relationship is observed
       *      - yet no automatic upcast to superclass is performed
       */
      void
      demonstrate_TypeReconciliation()
        {
          CHECK ((not CommonResult<int,long*>{}));                   // can not be reconciled
          CHECK ((not CommonResult<int,long*>::value));
//        using X = CommonResult<int,long*>::Type;                   // does not (and should not) compile
//        using X = CommonResult_t<int,long*>;                       // does not compile
          CHECK ((not has_CommonResult_v<int,long*>));
          CHECK ((not reconcilable<int,long*>));                     // a concept
          
          CHECK ((not CommonResult<>::value));                       // ¬ ∃ T∈{} |…
//        using X = CommonResult_t<>;                                // does not compile
          
          CHECK ((showType<CommonResult_t<string>>())                == "string &&"_expect );
          CHECK ((showType<CommonResult_t<const string>>())          == "const string &&"_expect );
          CHECK ((showType<CommonResult_t<volatile string>>())       == "string &&"_expect );
          CHECK ((showType<CommonResult_t<const volatile string>>()) == "const string &&"_expect );
          CHECK ((showType<CommonResult_t<string & >>())             == "string&"_expect );
          CHECK ((showType<CommonResult_t<string &&>>())             == "string &&"_expect );
          CHECK ((showType<CommonResult_t<string const& >>())        == "string const&"_expect );
          CHECK ((showType<CommonResult_t<string *  >>())            == "string* &&"_expect );
          CHECK ((showType<CommonResult_t<string *& >>())            == "string*&"_expect );
          CHECK ((showType<CommonResult_t<string const *& >>())      == "string const*&"_expect );
          CHECK ((showType<CommonResult_t<string * const& >>())      == "string* const&"_expect );
          CHECK ((showType<CommonResult_t<string * const  >>())      == "const string* &&"_expect );
          
          CHECK ((showType<CommonResult_t<string,string>>())           == "string &&"_expect );
          CHECK ((showType<CommonResult_t<string,string&>>())          == "string"_expect );
          CHECK ((showType<CommonResult_t<string,string&,string>>())   == "string &&"_expect );
          CHECK ((showType<CommonResult_t<string,string,string&>>())   == "string"_expect );
          CHECK ((showType<CommonResult_t<const string, string&>>())   == "const string"_expect );
          CHECK ((showType<CommonResult_t<string&,  string&&>>())      == "string"_expect );
          CHECK ((showType<CommonResult_t<string&&, string&&>>())      == "string &&"_expect );
          CHECK ((showType<CommonResult_t<string&&,const string&&>>()) == "const string &&"_expect );
          
          CHECK ((showType<CommonResult_t<float*, float* const>>())        == "const float* &&"_expect );
          CHECK ((showType<CommonResult_t<float*, float const*>>())        == "const float *"_expect );
          CHECK ((showType<CommonResult_t<float*, void* >>())              == "void *"_expect );
          CHECK ((showType<CommonResult_t<float* &, void* &>>())           == "void *"_expect );
          CHECK ((showType<CommonResult_t<float* &, void* &>>())           == "void *"_expect );
          CHECK ((showType<CommonResult_t<short&, long&>>())               == "long"_expect );
          CHECK ((showType<CommonResult_t<const int&,uint16_t,int64_t>>()) == "long"_expect );
          
          CHECK ((showType<CommonResult_t<float,double>>())              == "double"_expect );
          CHECK ((showType<CommonResult_t<long, double>>())              == "double"_expect );
          CHECK ((showType<CommonResult_t<long, double, float>>())       == "double"_expect );
          CHECK ((showType<CommonResult_t<long const&,double const&>>()) == "double"_expect );
          
          CHECK ((not reconcilable<float,float*> ));
          CHECK ((not reconcilable<float*,float**> ));

          CHECK ((    reconcilable<short,long> ));
          CHECK ((    reconcilable<short,long&> ));
          CHECK ((not reconcilable_ref<short,long> ));
          CHECK ((not reconcilable_ref<short,long&> ));
          CHECK ((    reconcilable_ref<long&,long&> ));
          CHECK ((not reconcilable_ref<short&,long&> ));

          // II ◁— S1 ∧ II ◁— S2
          CHECK ((    reconcilable<S1,II> ));
          CHECK ((    reconcilable<II,S1> ));
          CHECK ((    reconcilable<II,S2> ));
          CHECK ((not reconcilable<S1,S2> ));
          CHECK ((not reconcilable<S1,S2,II>    ));                    // NOTE: this is surprising
          CHECK ((    reconcilable<II,S1,S2>    ));                    //       the reason is processing from left-to-right
          CHECK ((    reconcilable<II&,S1&,S2&> ));

          CHECK ((showType<CommonResult_t<II,S1>>()) == "II &&"_expect );
          CHECK ((showType<CommonResult_t<S1,II>>()) == "II &&"_expect );
          CHECK ((showType<CommonResult_t<S1,II,S2>>()) == "II &&"_expect );
          CHECK ((showType<CommonResult_t<S1&,II,S2>>()) == "II &&"_expect );
          CHECK ((showType<CommonResult_t<II&,S1&,S2&>>()) == "II&"_expect );
        }
      
      
      /** @test show use in Metaprogramming with variadics */
      void
      demonstrate_Metaprocessing()
        {
          auto tup = std::make_tuple (1, 2.3, '4');
          CHECK (tup == "«tuple<int, double, char>»──(1,2.3,4)"_expect );

          using Tup = decltype(tup);
          CHECK (showType<Tup>() == "tuple<int, double, char>"_expect );
          
          using ValSeq   = ElmTypes<Tup>::Seq;
          using ConstSeq = ElmTypes<ValSeq>::template Apply<std::add_lvalue_reference_t>;
          CHECK (showType<ConstSeq>() == "Types<int&, double&, char&>"_expect );

          using CommRes = CommonResult_t<ConstSeq>;
          CHECK (showType<CommRes>() == "double"_expect );
          
          // Pick function overload based on reconcilability
          auto trp = std::tuple_cat (tup, tuple("😈"));
          CHECK (doIt (tup) == "Yes we can"_expect );
          CHECK (doIt (trp) == "SO SAD!↯↯!"_expect );
        }
      
      template<typename...TYPES>
      requires reconcilable<TYPES...>
      static string doIt (tuple<TYPES...>) { return "Yes we can"; }
      
      template<typename...TYPES>
      static string doIt (tuple<TYPES...>) { return "SO SAD!↯↯!"; }
    };
  
  LAUNCHER (CommonResult_test, "unit common");
  
  
}}} // namespace lib::meta::test

