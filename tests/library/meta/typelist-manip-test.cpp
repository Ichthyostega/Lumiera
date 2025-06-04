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

using std::is_same;


namespace lib  {
namespace meta {
namespace test {
      
      namespace { // type-lists to test with
        
        typedef TySeq< Num<1>
                     , Num<2>
                     , Num<3>
                     >::List List1;
        typedef TySeq< Num<5>
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
              demonstrate_diagnostics();
              verify_pick   ();
              verify_last   ();
              verify_append ();
              verify_splice ();
              verify_dissect();
              verify_apply  ();
              verify_filter ();
              verify_prefix ();
              verify_distribute();
              verify_combine();
            }
          
          
          /** @test can print the type list structure for diagnostic */
          void
          demonstrate_diagnostics ()
            {
              // Explanation: the DISPLAY macro results in the following definition....
              typedef InstantiateChained<List1::List, Printer, NullP >  Contents_List1;
              cout << "List1" << "\t:" << Contents_List1::show() << endl;
              
              // That is: we instantiate the "Printer" template for each of the types in List1,
              // forming an inheritance chain. I.e. the defined Type "Contents_List1" inherits
              // from each instantiation (single inheritance).
              // The print() function is defined to create a string showing each.
              
              DISPLAY (List2);
              
              // Furthermore, we use verification against lib::test::ExpectString,
              // which can be given as literal "some text"_expect, to document and
              // verify that the rendered type structure is indeed as expected
              CHECK (showType<List1>() == "-<1>-<2>-<3>-"_expect);
              
              EXPECT (List2, "-<5>-<6>-<7>-");
            }
          
          
          /** @test pick a type by index from a type list */
          void
          verify_pick ()
            {
              using E0 = Pick<List2,0>::Type;
              using E1 = Pick<List2,1>::Type;
              using E2 = Pick<List2,2>::Type;
              
              using E3   = Pick<List2,3>::Type;
              using NilE = Pick<Nil, 23>::Type;
              using Bull = Pick<void*,456>::Type;
              using Shit = Pick<string,0>::Type;
              
              CHECK ((is_same<E0, Num<5> >() ));
              CHECK ((is_same<E1, Num<6> >() ));
              CHECK ((is_same<E2, Num<7> >() ));
              
              CHECK ((is_same<E3,   Nil  >() ));
              CHECK ((is_same<NilE, Nil  >() ));
              CHECK ((is_same<Bull, Nil  >() ));
              CHECK ((is_same<Shit, Nil  >() ));
              
              // note: in the end, all this meta-processing yields types,
              //       which can be instantiated and used as if defined directly
              E0 e0;
              E1 e1;
              E2 e2;
              CHECK (5 == e0);
              CHECK (6 == e1);
              CHECK (7 == e2);
              CHECK (7 == e2.o_);
            }
          
          
          void
          verify_last()
            {
              using    Elm = PickLast<List1>::Type;
              using Prefix = PickLast<List1>::List;
              
              using   ElmL = TySeq<Elm>::List;
              
              EXPECT (Prefix,  "-<1>-<2>-");
              EXPECT (ElmL  ,  "-<3>-"    );
              
              using    Elm1 = PickLast<ElmL>::Type;
              using NPrefix = PickLast<ElmL>::List;
              
              EXPECT (TySeq<Elm1>, "-<3>-");
              EXPECT (NPrefix    ,     "-");
              
              using NilSplit = PickLast<Nil>::Type;
              using NilList  = PickLast<Nil>::List;
              
              EXPECT (NilSplit, "-");
              EXPECT (NilList , "-");
            }
          
          
          /** @test append and concatenate type lists */
          void
          verify_append ()
            {
              using Append1 = Append<Nil     , Nil     >;   EXPECT (Append1, "-"                        );
              using Append2 = Append<Num<11> , Num<22> >;   EXPECT (Append2, "-<11>-<22>-"              );
              using Append3 = Append<Num<111>, Nil     >;   EXPECT (Append3, "-<111>-"                  );
              using Append4 = Append<Nil     , Num<222>>;   EXPECT (Append4, "-<222>-"                  );
              using Append5 = Append<List1   , Nil     >;   EXPECT (Append5, "-<1>-<2>-<3>-"            );
              using Append6 = Append<Nil     , List2   >;   EXPECT (Append6, "-<5>-<6>-<7>-"            );
              using Append7 = Append<Num<111>, List2   >;   EXPECT (Append7, "-<111>-<5>-<6>-<7>-"      );
              using Append8 = Append<List1   , Num<222>>;   EXPECT (Append8, "-<1>-<2>-<3>-<222>-"      );
              using Append9 = Append<List1   , List2   >;   EXPECT (Append9, "-<1>-<2>-<3>-<5>-<6>-<7>-");
            }
          
          
          /** @test splice (or rather paste) a list on top of a base list
           *  @remark the intended use case is to manipulate some parameters
           *          in a given function-type argument list
           */
          void
          verify_splice ()
            {
              using OLi = TySeq<Num<9>,Num<8>>::List;
              // will "paste" the list OLi "on top" of another Typelist...
              
              using Overl01 = Splice<Nil, Nil>;              EXPECT (Overl01, "-");
              using Overl02 = Splice<Nil, OLi>;              EXPECT (Overl02, "-");
              using Overl03 = Splice<Nil, OLi, 5>;           EXPECT (Overl03, "-");
              using Overl04 = Splice<List1, OLi>;            EXPECT (Overl04, "-<9>-<8>-<3>-");
              using Overl05 = Splice<List1, OLi, 1>;         EXPECT (Overl05, "-<1>-<9>-<8>-");
              using Overl06 = Splice<List1, OLi, 2>;         EXPECT (Overl06, "-<1>-<2>-<9>-");
              using Overl07 = Splice<List1, OLi, 3>;         EXPECT (Overl07, "-<1>-<2>-<3>-");
              using Overl08 = Splice<List1, OLi, 5>;         EXPECT (Overl08, "-<1>-<2>-<3>-");
              using Overl09 = Splice<List1, List1>;          EXPECT (Overl09, "-<1>-<2>-<3>-");
              using Overl10 = Splice<List1, List1, 1>;       EXPECT (Overl10, "-<1>-<1>-<2>-");
              using Overl11 = Splice<List1, Nil>;            EXPECT (Overl11, "-<1>-<2>-<3>-");
              using Overl12 = Splice<List1, Nil, 1>;         EXPECT (Overl12, "-<1>-<2>-<3>-");
              using Overl13 = Splice<List1, Nil, 5>;         EXPECT (Overl13, "-<1>-<2>-<3>-");
              
              using OLi2   = TySeq<Num<99>>::List;
              // can retrieve the remaining part of the original list, left and right of splice
              using Front1 = Splice<List1, OLi2, 0>::Front;  EXPECT (Front1, "-"            );
              using Front2 = Splice<List1, OLi2, 1>::Front;  EXPECT (Front2, "-<1>-"        );
              using Front3 = Splice<List1, OLi2, 5>::Front;  EXPECT (Front3, "-<1>-<2>-<3>-");
              using Back1  = Splice<List1, OLi2, 0>::Back;   EXPECT (Back1 , "-<2>-<3>-"    );
              using Back2  = Splice<List1, OLi2, 1>::Back;   EXPECT (Back2 , "-<3>-"        );
              using Back3  = Splice<List1, OLi2, 5>::Back;   EXPECT (Back3 , "-"            );
              
              // Note: with a Null-Overlay, this can be used to extract arbitrary sublists:
              using Front4 = Splice<List1, Nil, 1>::Front;   EXPECT (Front4, "-<1>-"    );
              using Back4  = Splice<List1, Nil, 1>::Back;    EXPECT (Back4 , "-<2>-<3>-");
            }
          
          
          
          /** @test dissect and access front and back parts of a list */
          void
          verify_dissect()
            {
              using LL = Append<List1,List2>::List; EXPECT (LL    , "-<1>-<2>-<3>-<5>-<6>-<7>-");
              
              using List   = Dissect<LL>::List;     EXPECT (List  , "-<1>-<2>-<3>-<5>-<6>-<7>-");
              using First  = Dissect<LL>::First;    EXPECT (First , "-<1>-"                    );
              using Tail   = Dissect<LL>::Tail;     EXPECT (Tail  , "-<2>-<3>-<5>-<6>-<7>-"    );
              using Prefix = Dissect<LL>::Prefix;   EXPECT (Prefix, "-<1>-<2>-<3>-<5>-<6>-"    );
              using Last   = Dissect<LL>::Last;     EXPECT (Last  , "-<7>-"                    );
              
              using Head   = Dissect<LL>::Head;
              using End    = Dissect<LL>::End;
              
              using HeadEnd = TySeq<Head,End>;      EXPECT (HeadEnd, "-<1>-<7>-");
            }
          
          
          
          
          template<class X> struct AddConst2         { typedef X        Type; };
          template<int I>   struct AddConst2<Num<I>> { typedef Num<I+2> Type; };
          
          /** @test apply a _meta-function_ to each type in list */
          void
          verify_apply()
            {
              using Added2 = Apply<List1, AddConst2>;
              EXPECT (List1 , "-<1>-<2>-<3>-");
              EXPECT (Added2, "-<3>-<4>-<5>-");
            }
          
          
          template<class X> struct IsEven         { enum {value = false };        };
          template<int I>   struct IsEven<Num<I>> { enum {value = (0 == I % 2) }; };
          
          /** @test filter a type list based on a meta-predicate */
          void
          verify_filter()
            {
              CHECK (not IsEven<Num<5>>::value);
              CHECK (    IsEven<Num<6>>::value);
              
              using FilterEven = Filter<Append<List1,List2>::List, IsEven >;
              EXPECT (FilterEven, "-<2>-<6>-");
            }
          
          
          
          
          /** @test verify generation of nested lists-of-lists
           *        by distributing the elements from the first list
           *        as prefix-concatenation to the elements of the second list
           *  @note the second list can actually be already a list-of-lists,
           *        allowing to set up quite elaborate generation schemes.
           */
          void
          verify_prefix ()
            {
              using Prefix1 = PrefixAll<Num<11>,Num<22>>;
              // Note this creates a nested two-dimensional structure,
              // i.e. a type-list, whose elements are again type-lists.
              // The diagnostic helper prints each sublist in a new line,
              // and prefixed by a tab:
              EXPECT (Prefix1, "\n\t"  "+---<11>-<22>-+-");
              
              using Prefix2 = PrefixAll<Num<101>,List1>;
              EXPECT (List1  ,         "-<1>-<2>-<3>-");
              EXPECT (Prefix2, "\n\t"  "+---<101>-<1>-+"
                               "\n\t"  "+---<101>-<2>-+"
                               "\n\t"  "+---<101>-<3>-+-");
              
              using Prefix3 = PrefixAll<Nil,List1>;
              EXPECT (Prefix3, "\n\t"  "+---<1>-+"
                               "\n\t"  "+---<2>-+"
                               "\n\t"  "+---<3>-+-");
              
              // Notably this can also be used to distribute into an already nested structure,
              // since the implementation is based on Append, which will actually concatenate lists
              // To demonstrate this, we first create a mixed list, where some elements are nested lists
              using List_of_Lists = TySeq<List1::List
                                         ,Num<0>             // ◁—————————————— this one is a regular element
                                         ,List2::List>::List;
              EXPECT (List_of_Lists,
                               "\n\t"  "+---<1>-<2>-<3>-+"
                                       "-<0>"                // ◁—————————————— here shows the regular element
                               "\n\t"  "+---<5>-<6>-<7>-+-");
              
              using Prefix4       = PrefixAll<Num<111>, List_of_Lists>;
              EXPECT (Prefix4, "\n\t"  "+---<111>-<1>-<2>-<3>-+"    // ◁——————— concatenation »flattens« the lists
                               "\n\t"  "+---<111>-<0>-+"
                               "\n\t"  "+---<111>-<5>-<6>-<7>-+-");
              
              using Prefix5 = PrefixAll<List1,List2>;
              EXPECT (Prefix5, "\n\t"  "+---<1>-<2>-<3>-<5>-+"
                               "\n\t"  "+---<1>-<2>-<3>-<6>-+"
                               "\n\t"  "+---<1>-<2>-<3>-<7>-+-");
              
              using Prefix6 = PrefixAll<List1,List_of_Lists>;
              EXPECT (Prefix6, "\n\t"  "+---<1>-<2>-<3>-<1>-<2>-<3>-+"
                               "\n\t"  "+---<1>-<2>-<3>-<0>-+"
                               "\n\t"  "+---<1>-<2>-<3>-<5>-<6>-<7>-+-");
            }
          
          
          
          /** @test use the Prefix mechanism to generate a _cartesian product_ */
          void
          verify_distribute()
            {
              using Dist1 = Distribute<Num<11>, List1>;
              EXPECT (Dist1,  "\n\t"  "+---<11>-<1>-+"
                              "\n\t"  "+---<11>-<2>-+"
                              "\n\t"  "+---<11>-<3>-+-");
              
              using Prefixes = TySeq<Num<11>,Num<22>,Num<33>>::List;
              using  Dist2 = Distribute<Prefixes, Num<0>>;
              EXPECT (Dist2,   "\n\t"  "+---<11>-<0>-+"
                               "\n\t"  "+---<22>-<0>-+"
                               "\n\t"  "+---<33>-<0>-+-");
              
              using Dist3 = Distribute<Prefixes, List1>;
              EXPECT (Dist3,   "\n\t"  "+---<11>-<1>-+"
                               "\n\t"  "+---<11>-<2>-+"
                               "\n\t"  "+---<11>-<3>-+"
                               "\n\t"  "+---<22>-<1>-+"
                               "\n\t"  "+---<22>-<2>-+"
                               "\n\t"  "+---<22>-<3>-+"
                               "\n\t"  "+---<33>-<1>-+"
                               "\n\t"  "+---<33>-<2>-+"
                               "\n\t"  "+---<33>-<3>-+-");
              
              using LioLi = TySeq<List1::List,List2::List>::List;
              EXPECT (LioLi,   "\n\t"  "+---<1>-<2>-<3>-+"
                               "\n\t"  "+---<5>-<6>-<7>-+-");
              using Dist4 = Distribute<Prefixes, LioLi>;
              EXPECT (Dist4,   "\n\t"  "+---<11>-<1>-<2>-<3>-+"
                               "\n\t"  "+---<11>-<5>-<6>-<7>-+"
                               "\n\t"  "+---<22>-<1>-<2>-<3>-+"
                               "\n\t"  "+---<22>-<5>-<6>-<7>-+"
                               "\n\t"  "+---<33>-<1>-<2>-<3>-+"
                               "\n\t"  "+---<33>-<5>-<6>-<7>-+-");
            }
          
          
          /** @test demonstrate special setup to enumerate case combinations
           *  @remark can be used to pre-generate template instantiations
           *          at compile time, for a complete configuration space
           *  @see GeneratorCombinations_test
           */
          void
          verify_combine()
            {          //   ▽▽▽ ———————————————defined in typelist-diagnostic.cpp
              using Count = CountDown<Num<11>>;
              EXPECT (Count, "-<11>-<10>-<9>-<8>-<7>-<6>-<5>-<4>-<3>-<2>-<1>-<0>-");
              
                       //   Apply on a single type and tabulate
              using Apply = Combine<Num<5>, CountDown>;
              EXPECT (Apply,   "\n\t"  "+---<5>-<·>-+"
                               "\n\t"  "+---<4>-<·>-+"
                               "\n\t"  "+---<3>-<·>-+"
                               "\n\t"  "+---<2>-<·>-+"
                               "\n\t"  "+---<1>-<·>-+"
                               "\n\t"  "+---<0>-<·>-+-");
              
                       //   Apply recursively to generate all combinations
              using Combi = Combine<List1::List, CountDown>;
              EXPECT (Combi,   "\n\t"  "+---<1>-<2>-<3>-<·>-+"
                               "\n\t"  "+---<1>-<2>-<2>-<·>-+"
                               "\n\t"  "+---<1>-<2>-<1>-<·>-+"
                               "\n\t"  "+---<1>-<2>-<0>-<·>-+"
                               "\n\t"  "+---<1>-<1>-<3>-<·>-+"
                               "\n\t"  "+---<1>-<1>-<2>-<·>-+"
                               "\n\t"  "+---<1>-<1>-<1>-<·>-+"
                               "\n\t"  "+---<1>-<1>-<0>-<·>-+"
                               "\n\t"  "+---<1>-<0>-<3>-<·>-+"
                               "\n\t"  "+---<1>-<0>-<2>-<·>-+"
                               "\n\t"  "+---<1>-<0>-<1>-<·>-+"
                               "\n\t"  "+---<1>-<0>-<0>-<·>-+"
                               "\n\t"  "+---<0>-<2>-<3>-<·>-+"
                               "\n\t"  "+---<0>-<2>-<2>-<·>-+"
                               "\n\t"  "+---<0>-<2>-<1>-<·>-+"
                               "\n\t"  "+---<0>-<2>-<0>-<·>-+"
                               "\n\t"  "+---<0>-<1>-<3>-<·>-+"
                               "\n\t"  "+---<0>-<1>-<2>-<·>-+"
                               "\n\t"  "+---<0>-<1>-<1>-<·>-+"
                               "\n\t"  "+---<0>-<1>-<0>-<·>-+"
                               "\n\t"  "+---<0>-<0>-<3>-<·>-+"
                               "\n\t"  "+---<0>-<0>-<2>-<·>-+"
                               "\n\t"  "+---<0>-<0>-<1>-<·>-+"
                               "\n\t"  "+---<0>-<0>-<0>-<·>-+-");
              
                       //   Special use-case: call combinations of a set of flags
              using OnOff = CombineFlags<List1::List>;
              EXPECT (OnOff,   "\n\t"  "+---<1>-<2>-<3>-<·>-+"
                               "\n\t"  "+---<1>-<2>-<·>-+"
                               "\n\t"  "+---<1>-<3>-<·>-+"
                               "\n\t"  "+---<1>-<·>-+"
                               "\n\t"  "+---<2>-<3>-<·>-+"
                               "\n\t"  "+---<2>-<·>-+"
                               "\n\t"  "+---<3>-<·>-+"
                               "\n\t"  "+---<·>-+-");
            }
        };
      
      
      /** Register this test class... */
      LAUNCHER (TypeListManip_test, "unit common");
      
      
      
}}} // namespace lib::meta::test
