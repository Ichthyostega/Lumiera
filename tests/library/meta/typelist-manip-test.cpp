/*
  TypeListManip(Test)  -  appending, mixing and filtering typelists

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file typelist-manip-test.cpp
 ** \par checking the correct working of simple list manipulation metafunctions
 **
 ** The semi-automatic assembly of processing node invocation code utilises some
 ** metaprogramming magic built upon simple list manipulation. As template metaprogramming
 ** is kind-of functional programming, most of this is done by recursion.
 ** To check the correct working, this test uses some constant-wrapper types and a debugging
 ** template which will print these constant numbers, thus allowing to verify in the output
 ** if various lists of such constant-wrapper types were manipulated as expected.
 **
 ** @see typelist-test.cpp
 ** @see typelist-util.hpp
 ** @see nodewiring-config.hpp real world usage example
 **
 */


#include "lib/test/run.hpp"
#include "lib/meta/generator.hpp"
#include "lib/meta/typelist-manip.hpp"
#include "meta/typelist-diagnostics.hpp"

#include <type_traits>
#include <iostream>

using ::test::Test;
using std::is_same;
using std::cout;
using std::endl;


namespace lib  {
namespace meta {
namespace test {
      
      
      namespace { // test data
        
        
        
        typedef TyOLD< Num<1>
                     , Num<2>
                     , Num<3>
                     >::List List1;
        typedef TyOLD< Num<5>
                     , Num<6>
                     , Num<7>
                     >::List List2;
        
        
        // see also the CountDown template in typelist-diagnostics.hpp...
        
      } // (End) test data
      
      
      
      
      
      
      /*********************************************************************//**
       * @test check utilities for manipulating lists-of-types.
       *       - build an list of constant-wrapper-types and
       *         print them for debugging purpose.
       *       - append lists, single elements and Nil-Type
       *         in various combinations
       *       - manipulations like splice, get end, dissect
       *       - filtering out some types from a typelist by
       *         using a "predicate template" (metafunction)
       *       - building combinations and permutations
       */
      class TypeListManip_test : public Test
        {
          virtual void
          run (Arg) 
            {
              check_diagnostics ();
              check_pick_elm ();
              check_apply  ();
              check_filter ();
              check_append ();
              check_splice ();
              check_s_last ();
              check_dissect();
              check_prefix ();
              check_distribute();
              check_combine();
            }
          
          
          void
          check_diagnostics ()
            {
              // Explanation: the DISPLAY macro results in the following definition....
              typedef InstantiateChained<List1::List, Printer, NullP >  Contents_List1;
              cout << "List1" << "\t:" << Contents_List1::print() << endl;
              
              // That is: we instantiate the "Printer" template for each of the types in List1,
              // forming an inheritance chain. I.e. the defined Type "Contents_List1" inherits
              // from each instantiation (single inheritance).
              // The print() function is defined to create a string showing each.
              
              DISPLAY (List2);
            }
          
          
          void
          check_pick_elm ()
            {
              Pick<List2,0>::Type e0;
              Pick<List2,1>::Type e1;
              Pick<List2,2>::Type e2;
              
              using E3         = Pick<List2,3>::Type;
              using NilE       = Pick<Nil, 23>::Type;
              using Irrelevant = Pick<void*,456>::Type;
              
              CHECK (5 == e0);
              CHECK (6 == e1);
              CHECK (7 == e2);
              
              CHECK ((is_same<Nil, E3>        ::value));
              CHECK ((is_same<Nil, NilE>      ::value));
              CHECK ((is_same<Nil, Irrelevant>::value));
            }
          
          
          void
          check_append ()
            {
              using Append1 = Append<Nil, Nil>;
              DISPLAY (Append1);
              
              using Append2 = Append<Num<11>,Num<22>>;
              DISPLAY (Append2);
              
              using Append3 = Append<Num<111>,Nil>;
              DISPLAY (Append3);
              
              using Append4 = Append<Nil,Num<222>>;
              DISPLAY (Append4);
              
              using Append5 = Append<List1,Nil>;
              DISPLAY (Append5);
              
              using Append6 = Append<Nil,List2>;
              DISPLAY (Append6);
              
              using Append7 = Append<Num<111>,List2>;
              DISPLAY (Append7);
              
              using Append8 = Append<List1,Num<222>>;
              DISPLAY (Append8);
              
              using Append9 = Append<List1,List2>;
              DISPLAY (Append9);
            }
          
          
          void
          check_splice ()
            {
              using OLi = TyOLD<Num<9>,Num<8>>::List;
              // will "paste" the list OLi "on top" of another Typelist...
              
              using Overl01 = Splice<Nil, Nil>;
              DISPLAY (Overl01);
              
              using Overl02 = Splice<Nil, OLi>;
              DISPLAY (Overl02);
              
              using Overl03 = Splice<Nil, OLi, 5>;
              DISPLAY (Overl03);
              
              using Overl04 = Splice<List1, OLi>;
              DISPLAY (Overl04);
              
              using Overl05 = Splice<List1, OLi, 1>;
              DISPLAY (Overl05);
              
              using Overl06 = Splice<List1, OLi, 2>;
              DISPLAY (Overl06);
              
              using Overl07 = Splice<List1, OLi, 3>;
              DISPLAY (Overl07);
              
              using Overl08 = Splice<List1, OLi, 5>;
              DISPLAY (Overl08);
              
              using Overl09 = Splice<List1, List1>;
              DISPLAY (Overl09);
              
              using Overl10 = Splice<List1, List1, 1>;
              DISPLAY (Overl10);
              
              using Overl11 = Splice<List1, Nil>;
              DISPLAY (Overl11);
              
              using Overl12 = Splice<List1, Nil, 1>;
              DISPLAY (Overl12);
              
              using Overl13 = Splice<List1, Nil, 5>;
              DISPLAY (Overl13);
              
              
              using OLi2   = TyOLD<Num<99>>::List;
              using Front1 = Splice<List1, OLi2, 0>::Front;
              using Front2 = Splice<List1, OLi2, 1>::Front;
              using Front3 = Splice<List1, OLi2, 5>::Front;
              DISPLAY (Front1);
              DISPLAY (Front2);
              DISPLAY (Front3);
              using Back1  = Splice<List1, OLi2, 0>::Back;
              using Back2  = Splice<List1, OLi2, 1>::Back;
              using Back3  = Splice<List1, OLi2, 5>::Back;
              DISPLAY (Back1);
              DISPLAY (Back2);
              DISPLAY (Back3);
              
              // Note: with a Null-Overlay, this can be used to extract arbitrary sublists:
              using Front4 = Splice<List1, Nil, 1>::Front;
              using Back4  = Splice<List1, Nil, 1>::Back;
              DISPLAY (Front4);
              DISPLAY (Back4);
            }
          
          
          void
          check_s_last()
            {
              using    Elm = SplitLast<List1>::Type;
              using Prefix = SplitLast<List1>::List;
              
              using   ElmL = TyOLD<Elm>::List;
              
              DISPLAY (Prefix);
              DISPLAY (ElmL);
              
              using    Elm1 = SplitLast<ElmL>::Type;
              using NPrefix = SplitLast<ElmL>::List;
              
              DISPLAY (NPrefix);
              DISPLAY (TyOLD<Elm1>);
              
              using  NilSplit = SplitLast<Nil>::Type;
              using  NList    = SplitLast<Nil>::List;
              
              DISPLAY (NList);
              DISPLAY (TyOLD<NilSplit>);
            }
          
          
          void
          check_dissect()
            {
              typedef Append<List1,List2>::List LL;
              DISPLAY (LL);
              
              using List   = Dissect<LL>::List;     DISPLAY(List);
              using First  = Dissect<LL>::First;    DISPLAY(First);
              using Tail   = Dissect<LL>::Tail;     DISPLAY(Tail);
              using Prefix = Dissect<LL>::Prefix;   DISPLAY(Prefix);
              using Last   = Dissect<LL>::Last;     DISPLAY(Last);
              
              using Head   = Dissect<LL>::Head;
              using End    = Dissect<LL>::End;
              
              using HeadEnd = TyOLD<Head,End>;      DISPLAY(HeadEnd);
            }
          
          
          
          
          template<class X> struct AddConst2         { typedef X        Type; };
          template<int I>   struct AddConst2<Num<I>> { typedef Num<I+2> Type; };
          
          void
          check_apply ()
            {
              using Added2 = Apply<List1, AddConst2>;
              DISPLAY (Added2);
            }
          
          
          template<class X> struct IsEven         { enum {value = false };        };
          template<int I>   struct IsEven<Num<I>> { enum {value = (0 == I % 2) }; };
          
          void
          check_filter ()
            {
              using FilterEven = Filter<Append<List1,List2>::List, IsEven >; 
              DISPLAY (FilterEven);
            }
          
          
          void
          check_prefix ()
            {
              using Prefix1 = PrefixAll<Num<11>,Num<22>>;
              DISPLAY (Prefix1);
              
              using Prefix2 = PrefixAll<Num<101>,List1>;
              DISPLAY (Prefix2);
              
              using Prefix3 = PrefixAll<Nil,List1>;
              DISPLAY (Prefix3);
              
              using List_of_Lists = TyOLD<List1::List,Num<0>,List2::List>::List;
              using Prefix4       = PrefixAll<Num<111>, List_of_Lists>;
              DISPLAY (Prefix4);
              
              using Prefix5 = PrefixAll<List1,List2>;
              DISPLAY (Prefix5);
              
              using Prefix6 = PrefixAll<List1,List_of_Lists>;
              DISPLAY (Prefix6);
            }
          
          
          void
          check_distribute()
            {
              using Dist1 = Distribute<Num<11>, List1>;
              DISPLAY (Dist1);
              
              using Prefixes = TyOLD<Num<11>,Num<22>,Num<33>>::List;
              using  Dist2   = Distribute<Prefixes, Num<0>>;
              DISPLAY (Dist2);
              
              using Dist3 = Distribute<Prefixes, List1>;
              DISPLAY (Dist3);
              
              using Dist4 = Distribute<Prefixes, TyOLD<List1::List,List2::List>::List>;
              DISPLAY (Dist4);
            }
          
          
          void
          check_combine()
            {
              using Down = CountDown<Num<11>>;
              DISPLAY (Down);
              
              using Combi = Combine<List1::List, CountDown>;
              DISPLAY (Combi);
              
              using OnOff = CombineFlags<List1::List>;
              DISPLAY (OnOff);
            }
          
        };
      
      
      /** Register this test class... */
      LAUNCHER (TypeListManip_test, "unit common");
      
      
      
}}} // namespace lib::meta::test
