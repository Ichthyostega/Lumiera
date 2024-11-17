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
        
        
        
        typedef Types< Num<1>
                     , Num<2>
                     , Num<3>
                     >::List List1;
        typedef Types< Num<5>
                     , Num<6>
                     , Num<7>
                     >::List List2;
        
        
        // see also the CountDown template in typelist-diagnostics.hpp...
        
      } // (End) test data
      
      
      
      
      
      
      /*********************************************************************//**
       * @test check utilities for manipulating lists-of-types.
       *       - build an list of constant-wrapper-types and
       *         print them for debugging purpose.
       *       - append lists, single elements and NullType
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
              
              typedef Pick<List2,3>::Type     E3;
              typedef Pick<NullType,23>::Type Nil;
              typedef Pick<void*,456>::Type   Irrelevant;
              
              CHECK (5 == e0);
              CHECK (6 == e1);
              CHECK (7 == e2);
              
              CHECK ((is_same<NullType, E3>        ::value));
              CHECK ((is_same<NullType, Nil>       ::value));
              CHECK ((is_same<NullType, Irrelevant>::value));
            }
          
          
          void
          check_append ()
            {
              typedef Append<NullType, NullType> Append1;
              DISPLAY (Append1);
              
              typedef Append<Num<11>,Num<22>>    Append2;
              DISPLAY (Append2);
              
              typedef Append<Num<111>,NullType>  Append3;
              DISPLAY (Append3);
              
              typedef Append<NullType,Num<222>>  Append4;
              DISPLAY (Append4);
              
              typedef Append<List1,NullType>     Append5;
              DISPLAY (Append5);
              
              typedef Append<NullType,List2>     Append6;
              DISPLAY (Append6);
              
              typedef Append<Num<111>,List2>     Append7;
              DISPLAY (Append7);
              
              typedef Append<List1,Num<222>>     Append8;
              DISPLAY (Append8);
              
              typedef Append<List1,List2>        Append9;
              DISPLAY (Append9);
            }
          
          
          void
          check_splice ()
            {
              typedef Types<Num<9>,Num<8>>::List OLi;
              // will "paste" the list OLi "on top" of another Typelist...
              
              typedef Splice<NullType, NullType> Overl01;
              DISPLAY (Overl01);
              
              typedef Splice<NullType, OLi>      Overl02;
              DISPLAY (Overl02);
              
              typedef Splice<NullType, OLi, 5>   Overl03;
              DISPLAY (Overl03);
              
              typedef Splice<List1, OLi>         Overl04;
              DISPLAY (Overl04);
              
              typedef Splice<List1, OLi, 1>      Overl05;
              DISPLAY (Overl05);
              
              typedef Splice<List1, OLi, 2>      Overl06;
              DISPLAY (Overl06);
              
              typedef Splice<List1, OLi, 3>      Overl07;
              DISPLAY (Overl07);
              
              typedef Splice<List1, OLi, 5>      Overl08;
              DISPLAY (Overl08);
              
              typedef Splice<List1, List1>       Overl09;
              DISPLAY (Overl09);
              
              typedef Splice<List1, List1, 1>    Overl10;
              DISPLAY (Overl10);
              
              typedef Splice<List1, NullType>    Overl11;
              DISPLAY (Overl11);
              
              typedef Splice<List1, NullType, 1> Overl12;
              DISPLAY (Overl12);
              
              typedef Splice<List1, NullType, 5> Overl13;
              DISPLAY (Overl13);
              
              
              typedef Types<Num<99>>::List OLi2;
              typedef Splice<List1, OLi2, 0>::Front Front1;
              typedef Splice<List1, OLi2, 1>::Front Front2;
              typedef Splice<List1, OLi2, 5>::Front Front3;
              DISPLAY (Front1);
              DISPLAY (Front2);
              DISPLAY (Front3);
              typedef Splice<List1, OLi2, 0>::Back  Back1;
              typedef Splice<List1, OLi2, 1>::Back  Back2;
              typedef Splice<List1, OLi2, 5>::Back  Back3;
              DISPLAY (Back1);
              DISPLAY (Back2);
              DISPLAY (Back3);
              
              // Note: with a Null-Overlay, this can be used to extract arbitrary sublists:
              typedef Splice<List1, NullType, 1>::Front Front4;
              typedef Splice<List1, NullType, 1>::Back  Back4;
              DISPLAY (Front4);
              DISPLAY (Back4);
            }
          
          
          void
          check_s_last()
            {
              typedef SplitLast<List1>::Type Elm;
              typedef SplitLast<List1>::List Prefix;
              
              typedef Types<Elm>::List ElmL;
              
              DISPLAY (Prefix);
              DISPLAY (ElmL);
              
              typedef SplitLast<ElmL>::Type Elm1;
              typedef SplitLast<ElmL>::List NPrefix;
              
              DISPLAY (NPrefix);
              DISPLAY (Types<Elm1>);
              
              typedef SplitLast<NullType>::Type Nil;
              typedef SplitLast<NullType>::List NList;
              
              DISPLAY (NList);
              DISPLAY (Types<Nil>);
            }
          
          
          void
          check_dissect()
            {
              typedef Append<List1,List2>::List LL;
              DISPLAY (LL);
              
              typedef Dissect<LL>::List   List;     DISPLAY(List); 
              typedef Dissect<LL>::First  First;    DISPLAY(First); 
              typedef Dissect<LL>::Tail   Tail;     DISPLAY(Tail);
              typedef Dissect<LL>::Prefix Prefix;   DISPLAY(Prefix);
              typedef Dissect<LL>::Last   Last;     DISPLAY(Last);
              
              typedef Dissect<LL>::Head   Head;
              typedef Dissect<LL>::End    End;
              
              typedef Types<Head,End>  HeadEnd;     DISPLAY(HeadEnd);
            }
          
          
          
          
          template<class X> struct AddConst2         { typedef X        Type; };
          template<int I>   struct AddConst2<Num<I>> { typedef Num<I+2> Type; };
          
          void
          check_apply ()
            {
              typedef Apply<List1, AddConst2> Added2;
              DISPLAY (Added2);
            }
          
          
          template<class X> struct IsEven         { enum {value = false };        };
          template<int I>   struct IsEven<Num<I>> { enum {value = (0 == I % 2) }; };
          
          void
          check_filter ()
            {
              typedef Filter<Append<List1,List2>::List, IsEven > FilterEven; 
              DISPLAY (FilterEven);
            }
          
          
          void
          check_prefix ()
            {
              typedef PrefixAll<Num<11>,Num<22>> Prefix1;
              DISPLAY (Prefix1);
              
              typedef PrefixAll<Num<101>,List1>  Prefix2;
              DISPLAY (Prefix2);
              
              typedef PrefixAll<NullType,List1>  Prefix3;
              DISPLAY (Prefix3);
              
              typedef Types<List1::List,Num<0>,List2::List>::List  List_of_Lists;
              typedef PrefixAll<Num<111>,List_of_Lists> Prefix4;
              DISPLAY (Prefix4);
              
              typedef PrefixAll<List1,List2>   Prefix5;
              DISPLAY (Prefix5);
              
              typedef PrefixAll<List1,List_of_Lists> Prefix6;
              DISPLAY (Prefix6);
            }
          
          
          void
          check_distribute()
            {
              typedef Distribute<Num<11>, List1> Dist1;
              DISPLAY (Dist1);
              
              typedef Types<Num<11>,Num<22>,Num<33>>::List Prefixes;
              typedef Distribute<Prefixes, Num<0>> Dist2;
              DISPLAY (Dist2);
              
              typedef Distribute<Prefixes, List1> Dist3;
              DISPLAY (Dist3);
              
              typedef Distribute<Prefixes, Types<List1::List,List2::List>::List> Dist4;
              DISPLAY (Dist4);
            }
          
          
          void
          check_combine()
            {
              typedef CountDown<Num<11>> Down;
              DISPLAY (Down);
              
              typedef Combine<List1::List, CountDown> Combi;
              DISPLAY (Combi);
              
              typedef CombineFlags<List1::List> OnOff;
              DISPLAY (OnOff);
            }
          
          
        };
      
      
      /** Register this test class... */
      LAUNCHER (TypeListManip_test, "unit common");
      
      
      
}}} // namespace lib::meta::test
