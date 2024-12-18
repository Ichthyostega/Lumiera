/*
  VariadicHelper(Test)  -  verify helpers for transforming variadics and tuple-like types

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file variadic-helper-test.cpp
 ** THe unit test \ref VariadicHelper_test demonstrates the usage of metaprogramming
 ** helpers to handle _tuple-like_ types and type sequences in a uniform way.
 ** @see variadic-helper.hpp
 ** @see tuple-helper.hpp
 ** @see feed-manifold.hpp real-world usage scenario
 **
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/meta/variadic-helper.hpp"
#include "lib/hetero-data.hpp"
#include "lib/test/diagnostic-output.hpp"////////////TODO

#include <tuple>
#include <array>

using lib::test::showType;
using std::array;
using std::tuple;


namespace lib  {
namespace meta {
namespace test {
  
  
  
  
  
  /*********************************************************************//**
   * @test Cover some advanced metaprogramming helpers to support working
   *       with _tuple like_ types together with simple types.
   */
  class VariadicHelper_test : public Test
    {
      virtual void
      run (Arg)
        {
          rebuild_variadic();
        }
      
      
      /** @test demonstrate uniform handling of...
       *      - simple types,
       *      - _tuple-like_ types (usable for structured binding)
       *      - _generic type sequences_
       */
      void
      rebuild_variadic()
        {
          // CASE-1 : a tuple....
          using T1 = tuple<int,double>;
          
          using S1 = ElmTypes<T1>;
          CHECK (2 == S1::SIZ);
          CHECK (showType< S1      >() ==           "ElmTypes<tuple<int, double>, void>"_expect);
          CHECK (showType< S1::Seq >() ==                           "TySeq<int, double>"_expect);
          CHECK (showType< S1::Tup >() ==                           "tuple<int, double>"_expect);
          CHECK (showType< S1::Idx >() ==            "integer_sequence<ulong, 0ul, 1ul>"_expect);
          
          using S1A = S1::Apply<std::is_pointer>;
          CHECK (showType< S1A     >() ==  "TySeq<is_pointer<int>, is_pointer<double> >"_expect);
          
          using S1AR = ElmTypes<S1A>::Rebind<std::__and_>;
          CHECK (showType< S1AR    >() == "__and_<is_pointer<int>, is_pointer<double> >"_expect);
          CHECK (false == S1AR::value);
          
          using S1AA = S1::AndAll<std::is_pointer>;
          CHECK (showType< S1AA    >() == "__and_<is_pointer<int>, is_pointer<double> >"_expect);
          CHECK (false == S1AA::value);
          
          using S1OA = S1::OrAll<std::is_pointer>;
          CHECK (showType< S1OA    >() ==  "__or_<is_pointer<int>, is_pointer<double> >"_expect);
          CHECK (false == S1OA::value);
          
          
          
          // CASE-0 : handling an unstructured simple type....
          using T0 = int*;
          
          using S0 = ElmTypes<T0>;
          CHECK (1 == S0::SIZ);
          CHECK (showType< S0      >() ==         "ElmTypes<int*, void>"_expect);
          CHECK (showType< S0::Seq >() ==                  "TySeq<int*>"_expect);
          CHECK (showType< S0::Tup >() ==                  "tuple<int*>"_expect);
          CHECK (showType< S0::Idx >() == "integer_sequence<ulong, 1ul>"_expect);
          
          using S0A = S0::Apply<std::is_pointer>;
          CHECK (showType< S0A     >() ==     "TySeq<is_pointer<int*> >"_expect);
          
          using S0AA = S0::AndAll<std::is_pointer>;
          CHECK (showType< S0AA    >() ==    "__and_<is_pointer<int*> >"_expect);
          CHECK (true == S0AA::value);
          
          using S0OA = S0::OrAll<std::is_pointer>;
          CHECK (showType< S0OA    >() ==     "__or_<is_pointer<int*> >"_expect);
          CHECK (true == S0OA::value);

          
          
          // CASE-2 : can also handle a std::array....
          using T2 = array<int*,3>;
          
          using S2 = ElmTypes<T2>;
          CHECK (3 == S2::SIZ);
          CHECK (showType< S2      >() ==         "ElmTypes<array<int*, 3ul>, void>"_expect);
          CHECK (showType< S2::Seq >() ==                  "TySeq<int*, int*, int*>"_expect);
          CHECK (showType< S2::Tup >() ==                  "tuple<int*, int*, int*>"_expect);
          CHECK (showType< S2::Idx >() ==   "integer_sequence<ulong, 0ul, 1ul, 2ul>"_expect);
          
          using S2A = S2::Apply<std::is_pointer>;
          CHECK (showType< S2A     >() ==  "TySeq<is_pointer<int*>, is_pointer<int*>, is_pointer<int*> >"_expect);
          
          using S2AA = S2::AndAll<std::is_pointer>;
          CHECK (showType< S2AA    >() == "__and_<is_pointer<int*>, is_pointer<int*>, is_pointer<int*> >"_expect);
          CHECK (true == S2AA::value);
          
          using S2OA = S2::OrAll<std::is_pointer>;
          CHECK (showType< S2OA    >() ==  "__or_<is_pointer<int*>, is_pointer<int*>, is_pointer<int*> >"_expect);
          CHECK (true == S2OA::value);

          
          
          // CASE-3 : a custom type which implements the C++ »tuple protocol«....
          using T3 = lib::HeteroData<int*,long,double*>;

          using S3 = ElmTypes<T3>;
          CHECK (3 == S3::SIZ);
          CHECK (showType< S3      >() ==                  "ElmTypes<HeteroData<int*, long, double*>, void>"_expect);
          CHECK (showType< S3::Seq >() ==                                       "TySeq<int*, long, double*>"_expect);
          CHECK (showType< S3::Idx >() ==                           "integer_sequence<ulong, 0ul, 1ul, 2ul>"_expect);
          
          using S3A = S3::Apply<std::is_pointer>;
          CHECK (showType< S3A     >() ==  "TySeq<is_pointer<int*>, is_pointer<long>, is_pointer<double*> >"_expect);
          
          using S3AA = S3::AndAll<std::is_pointer>;
          CHECK (showType< S3AA    >() == "__and_<is_pointer<int*>, is_pointer<long>, is_pointer<double*> >"_expect);
          CHECK (false == S3AA::value);
          
          using S3OA = S3::OrAll<std::is_pointer>;
          CHECK (showType< S3OA    >() ==  "__or_<is_pointer<int*>, is_pointer<long>, is_pointer<double*> >"_expect);
          CHECK (true == S3OA::value);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (VariadicHelper_test, "unit meta");
  
  
  
}}} // namespace lib::meta::test
