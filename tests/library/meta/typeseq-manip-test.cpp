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

using std::string;
using std::cout;
using std::endl;


namespace lib  {
namespace meta {
namespace test {
  
  
  namespace { // test data
    
    
    
    typedef Types< Num<1>
                 , Num<2>
                 , Num<3>
                 >         Types1;
    typedef Types< Num<7>
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
          check_buildSeq();
          check_prepend ();
          check_shift   ();
          check_split   ();
        }
      
      
      void
      check_buildSeq ()
        {
          typedef Append<Types1::List, Types2::List>::List LL;
          DISPLAY (LL);
          
          typedef Types<LL>::Seq Seq;
          typedef Seq::List SeqList;
          DISPLAY (Seq);
          DISPLAY (SeqList);
          
          typedef Types<NodeNull>::Seq NulS;
          DISPLAY (NulS);
        }
      
      
      void
      check_prepend ()
        {
          typedef Prepend<Num<5>, Types1> Prepend1;
          DISPLAY(Prepend1);
          
          typedef Prepend<NullType, Types1> Prepend2;
          DISPLAY(Prepend2);
          
          typedef Prepend<Num<5>,  Types<> > Prepend3;
          DISPLAY(Prepend3);
          
          typedef Prepend<NullType,  Types<> > Prepend4;
          DISPLAY(Prepend4);
        }
      
      
      void
      check_shift ()
        {
          typedef Append<Types2::List, Types1::List>::List LL;
          typedef Types<LL>::Seq Seq;
          
          typedef Shifted<Seq,0>::Type Seq_0;  DISPLAY (Seq_0);
          typedef Shifted<Seq,1>::Type Seq_1;  DISPLAY (Seq_1);
          typedef Shifted<Seq,2>::Type Seq_2;  DISPLAY (Seq_2);
          typedef Shifted<Seq,3>::Type Seq_3;  DISPLAY (Seq_3);
          typedef Shifted<Seq,4>::Type Seq_4;  DISPLAY (Seq_4);
          typedef Shifted<Seq,5>::Type Seq_5;  DISPLAY (Seq_5);
          typedef Shifted<Seq,6>::Type Seq_6;  DISPLAY (Seq_6);
          
          typedef Types<Shifted<Seq,0>::Head> Head_0; DISPLAY (Head_0);
          typedef Types<Shifted<Seq,1>::Head> Head_1; DISPLAY (Head_1);
          typedef Types<Shifted<Seq,2>::Head> Head_2; DISPLAY (Head_2);
          typedef Types<Shifted<Seq,3>::Head> Head_3; DISPLAY (Head_3);
          typedef Types<Shifted<Seq,4>::Head> Head_4; DISPLAY (Head_4);
          typedef Types<Shifted<Seq,5>::Head> Head_5; DISPLAY (Head_5);
          typedef Types<Shifted<Seq,6>::Head> Head_6; DISPLAY (Head_6);
        }
      
      
      void
      check_split ()
        {
          typedef Append<Types1::List, Types2::List>::List LL;
          typedef Types<LL>::Seq Seq;
          DISPLAY (Seq);
          
          typedef Split<Seq>::List   List;     DISPLAY(List); 
          typedef Split<Seq>::First  First;    DISPLAY(First); 
          typedef Split<Seq>::Tail   Tail;     DISPLAY(Tail);
          typedef Split<Seq>::Prefix Prefix;   DISPLAY(Prefix);
          typedef Split<Seq>::Last   Last;     DISPLAY(Last);
          
          typedef Split<Seq>::Head   Head;
          typedef Split<Seq>::End    End;
          
          typedef Types<Head,End>  HeadEnd;    DISPLAY(HeadEnd);
        }
      
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (TypeSeqManipl_test, "unit common");
  
  
  
}}} // namespace lib::meta::test
