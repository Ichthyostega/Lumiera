/*
  TypeSeqManip(Test)  -  simple manipulations on type sequences

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file typeseq-manip-test.cpp
 ** verify the proper working of simple type sequence manipulations.
 ** Here, "type sequence" stands for an instance of the template Types<T1,T2,...> from
 ** typelist.hpp . While this template is the entry point to type list metaprogramming,
 ** in many cases it is useful on its own for specifying a fixed collection of types, e.g.
 ** for building a tuple type. Thus, while more complicated manipulations typically rely
 ** on typelists, sometimes we need simple manipulations working directly on type sequences.
 ** These are covered here in a similar fashion as the typelist manipulators.
 **
 ** @see typeseq-util.hpp
 ** @see typelist-util.hpp
 ** @see typelist-manip-test.cpp
 **
 */


#include "test/run.hpp"
#include "lib/meta/typeseq-util.hpp"
#include "lib/meta/typelist-manip.hpp"
#include "lib/meta/typelist-diagnostics.hpp"

using std::string;


namespace lib  {
namespace meta {
namespace test {
  
  
  namespace { // type-sequences to test with
    
    using Types1 = Types< Num<1>
                        , Num<2>
                        , Num<3>
                        >;
    using Types2 = Types< Num<7>
                        , Num<8>
                        , Num<9>
                        >;
    
  } // (End) test data
  
  
  
  
  
  
  /**********************************************************************//**
   * @test check the basic utilities for manipulating (fixed) type sequences.
   *       - re-build an sequence from a type list
   *       - prepend a type to a given type sequence
   *       - create shifted sequences
   *       - dissect a sequence to extract head, tail, prefix, last element
   */
  class TypeSeqManip_test : public Test
    {
      virtual void
      run (Arg)
        {
          check_indexOf ();
          check_buildSeq();
          check_prepend ();
          check_shift   ();
          check_split   ();
        }
      
      
      void
      check_indexOf()
        {
          CHECK ((0 == indexOfType<int,    int, string, string>()));
          CHECK ((1 == indexOfType<string, int, string, string>()));
          CHECK ((2 == indexOfType<int,    string, string, int>()));
//        indexOfType<int>();
//        indexOfType<int,long,long>();                           // does not compile...
        }
      
      
      void
      check_buildSeq ()
        {
          using LL = Append<Types1::List, Types2::List>::List;
          EXPECT (LL, "-<1>-<2>-<3>-<7>-<8>-<9>-");
          
          using Seq     = Types<LL>::Seq;
          using SeqList = Seq::List;
          EXPECT (Seq,     "-<1>-<2>-<3>-<7>-<8>-<9>-");
          EXPECT (SeqList, "-<1>-<2>-<3>-<7>-<8>-<9>-");
          
          using NulS = Types<NilNode>::Seq;
          EXPECT (NulS, "-");
        }
      
      
      void
      check_prepend ()
        {
          using Prepend1 = Prepend<Num<5>, Types1 >;   EXPECT (Prepend1, "-<5>-<1>-<2>-<3>-");
          using Prepend2 = Prepend<Nil,    Types1 >;   EXPECT (Prepend2, "-<·>-<1>-<2>-<3>-");
          using Prepend3 = Prepend<Num<5>, Types<>>;   EXPECT (Prepend3, "-<5>-");
          using Prepend4 = Prepend<Nil,    Types<>>;   EXPECT (Prepend4, "-");
        }
      
      
      void
      check_shift ()
        {
          using LL  = Append<Types2::List, Types1::List>::List;
          using Seq = Types<LL>::Seq;
          
          using  Seq_0 =       Shifted<Seq,0>::Type;   EXPECT (Seq_0,  "-<7>-<8>-<9>-<1>-<2>-<3>-");
          using  Seq_1 =       Shifted<Seq,1>::Type;   EXPECT (Seq_1,  "-<8>-<9>-<1>-<2>-<3>-");
          using  Seq_2 =       Shifted<Seq,2>::Type;   EXPECT (Seq_2,  "-<9>-<1>-<2>-<3>-");
          using  Seq_3 =       Shifted<Seq,3>::Type;   EXPECT (Seq_3,  "-<1>-<2>-<3>-");
          using  Seq_4 =       Shifted<Seq,4>::Type;   EXPECT (Seq_4,  "-<2>-<3>-");
          using  Seq_5 =       Shifted<Seq,5>::Type;   EXPECT (Seq_5,  "-<3>-");
          using  Seq_6 =       Shifted<Seq,6>::Type;   EXPECT (Seq_6,  "-");
          
          using Head_0 = Types<Shifted<Seq,0>::Head>;  EXPECT (Head_0, "-<7>-");
          using Head_1 = Types<Shifted<Seq,1>::Head>;  EXPECT (Head_1, "-<8>-");
          using Head_2 = Types<Shifted<Seq,2>::Head>;  EXPECT (Head_2, "-<9>-");
          using Head_3 = Types<Shifted<Seq,3>::Head>;  EXPECT (Head_3, "-<1>-");
          using Head_4 = Types<Shifted<Seq,4>::Head>;  EXPECT (Head_4, "-<2>-");
          using Head_5 = Types<Shifted<Seq,5>::Head>;  EXPECT (Head_5, "-<3>-");
          using Head_6 = Types<Shifted<Seq,6>::Head>;  EXPECT (Head_6, "-"    );
          using Head_7 = Types<Shifted<Seq,7>::Head>;  EXPECT (Head_7, "-"    );
        }
      
      
      void
      check_split ()
        {
          using LL  = Append<Types1::List, Types2::List>::List;
          using Seq = Types<LL>::Seq;           EXPECT (Seq   , "-<1>-<2>-<3>-<7>-<8>-<9>-");
          
          using List   = Split<Seq>::List;      EXPECT (List  , "-<1>-<2>-<3>-<7>-<8>-<9>-");
          using First  = Split<Seq>::First;     EXPECT (First , "-<1>-"                    );
          using Tail   = Split<Seq>::Tail;      EXPECT (Tail  , "-<2>-<3>-<7>-<8>-<9>-"    );
          using Prefix = Split<Seq>::Prefix;    EXPECT (Prefix, "-<1>-<2>-<3>-<7>-<8>-"    );
          using Last   = Split<Seq>::Last;      EXPECT (Last  , "-<9>-"                    );
          
          using Head   = Split<Seq>::Head;
          using End    = Split<Seq>::End;
          
          using Ends   = Types<Head,End>;       EXPECT (Ends  , "-<1>-<9>-");
          
          using NoList = Split<Types<>>::List;  EXPECT (NoList, "-");
          using NoHead = Split<Types<>>::Head;  EXPECT (NoHead, "-");
        }
      
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (TypeSeqManip_test, "unit common");
  
  
  
}}} // namespace lib::meta::test
