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
 ** Here, "type sequence" denotes an instance of the template Types<T1,T2,...> from
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


#include "lib/test/run.hpp"
#include "lib/meta/typeseq-util.hpp"
#include "lib/meta/typelist-manip.hpp"
#include "meta/typelist-diagnostics.hpp"
#include "lib/format-cout.hpp"

using std::string;


namespace lib  {
namespace meta {
namespace test {
  
  
  namespace { // test data
    
    typedef TyOLD< Num<1>
                 , Num<2>
                 , Num<3>
                 >         Types1;
    typedef TyOLD< Num<7>
                 , Num<8>
                 , Num<9>
                 >         Types2;
    
    // see also the CountDown template in typelist-diagnostics.hpp...
    
  } // (End) test data
  
  
  
  
  
  
  /**********************************************************************//**
   * @test check the basic utilities for manipulating (fixed) type sequences.
   *       - re-build an sequence from a type list
   *       - prepend a type to a given type sequence
   *       - create shifted sequences
   *       - dissect a sequence to extract head, tail, prefix, last element
   */
  class TypeSeqManipl_test : public Test
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
          DISPLAY (LL);
          
          using Seq     = TyOLD<LL>::Seq;
          using SeqList = Seq::List;
          DISPLAY (Seq);
          DISPLAY (SeqList);
          
          using NulS = TyOLD<NilNode>::Seq;
          DISPLAY (NulS);
        }
      
      
      void
      check_prepend ()
        {
          using Prepend1 = Prepend<Num<5>, Types1 >;
          DISPLAY(Prepend1);
          
          using Prepend2 = Prepend<Nil,    Types1 >;
          DISPLAY(Prepend2);
          
          using Prepend3 = Prepend<Num<5>, TyOLD<>>;
          DISPLAY(Prepend3);
          
          using Prepend4 = Prepend<Nil,    TyOLD<>>;
          DISPLAY(Prepend4);
        }
      
      
      void
      check_shift ()
        {
          using LL  = Append<Types2::List, Types1::List>::List;
          using Seq = TyOLD<LL>::Seq;
          
          using  Seq_0 =       Shifted<Seq,0>::Type;   DISPLAY (Seq_0);
          using  Seq_1 =       Shifted<Seq,1>::Type;   DISPLAY (Seq_1);
          using  Seq_2 =       Shifted<Seq,2>::Type;   DISPLAY (Seq_2);
          using  Seq_3 =       Shifted<Seq,3>::Type;   DISPLAY (Seq_3);
          using  Seq_4 =       Shifted<Seq,4>::Type;   DISPLAY (Seq_4);
          using  Seq_5 =       Shifted<Seq,5>::Type;   DISPLAY (Seq_5);
          using  Seq_6 =       Shifted<Seq,6>::Type;   DISPLAY (Seq_6);
          
          using Head_0 = TyOLD<Shifted<Seq,0>::Head>;  DISPLAY (Head_0);
          using Head_1 = TyOLD<Shifted<Seq,1>::Head>;  DISPLAY (Head_1);
          using Head_2 = TyOLD<Shifted<Seq,2>::Head>;  DISPLAY (Head_2);
          using Head_3 = TyOLD<Shifted<Seq,3>::Head>;  DISPLAY (Head_3);
          using Head_4 = TyOLD<Shifted<Seq,4>::Head>;  DISPLAY (Head_4);
          using Head_5 = TyOLD<Shifted<Seq,5>::Head>;  DISPLAY (Head_5);
          using Head_6 = TyOLD<Shifted<Seq,6>::Head>;  DISPLAY (Head_6);
        }
      
      
      void
      check_split ()
        {
          using LL  = Append<Types1::List, Types2::List>::List;
          using Seq = TyOLD<LL>::Seq;
          DISPLAY (Seq);
          
          using List   = Split<Seq>::List;      DISPLAY(List); 
          using First  = Split<Seq>::First;     DISPLAY(First); 
          using Tail   = Split<Seq>::Tail;      DISPLAY(Tail);
          using Prefix = Split<Seq>::Prefix;    DISPLAY(Prefix);
          using Last   = Split<Seq>::Last;      DISPLAY(Last);
          
          using Head   = Split<Seq>::Head;
          using End    = Split<Seq>::End;    
          
          using HeadEnd = TyOLD<Head,End>;      DISPLAY(HeadEnd);
        }
      
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (TypeSeqManipl_test, "unit common");
  
  
  
}}} // namespace lib::meta::test
