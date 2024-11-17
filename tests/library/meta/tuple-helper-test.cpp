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
#include "lib/format-cout.hpp"


using ::test::Test;
using lib::test::showSizeof;


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
          DISPLAY (T_L1);
          DISPLAY (Seq1);
          
          T_L1 tup1;                        // can be instantiated at runtime (and is just a std:tuple)
          DUMPVAL (tup1);
          
          using Prepend = Tuple<Node<int, L1>>;
          DISPLAY (Prepend);                // another ListType based tuple created by prepending
          
          Prepend prep (22, 11,33,Num<5>());
          DUMPVAL (prep);
          
          typedef Tuple<Types<> > NulT;     // plain-flat empty Tuple
          typedef Tuple<NullType> NulL;     // list-style empty Tuple
          
          NulT nulT;                        // and these, too, can be instantiated
          NulL nulL;
          
          using S4 = struct{int a,b,c,d;};  // expect this to have the same memory layout
          CHECK (sizeof(S4) == sizeof(prep));
          CHECK (1 == sizeof(nulL));        // ...minimal storage, as expected
          
          
          CHECK (is_Tuple<T_L1>());
          CHECK (is_Tuple<Prepend>());
          CHECK (is_Tuple<NulT>());
          CHECK (!is_Tuple<Seq1>());
          
          cout << tup1 <<endl               // these automatically use our generic string conversion
               << prep <<endl
               << nulL <<endl;
          
          cout << showSizeof(tup1) <<endl
               << showSizeof(prep) <<endl
               << showSizeof(nulT) <<endl
               << showSizeof(nulL) <<endl;
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TupleHelper_test, "unit meta");
  
  
  
}}} // namespace lib::meta::test
