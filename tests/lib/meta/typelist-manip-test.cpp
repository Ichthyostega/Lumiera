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


#include "test/run.hpp"
#include "lib/meta/generator.hpp"
#include "lib/meta/typelist-manip.hpp"
#include "lib/meta/typelist-diagnostics.hpp"

#include <type_traits>

using std::is_same;


namespace lib  {
namespace meta {
namespace test {
      
      namespace { // type-lists to test with
        
        using List1 = Types< Num<1>
                           , Num<2>
                           , Num<3>
                           >::List;
        using List2 = Types< Num<5>
                           , Num<6>
                           , Num<7>
                           >::List;
        
        
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
              CHECK (renderSeq<List1>() == "-<1>-<2>-<3>-"_expect);
              
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
              
              using   ElmL = Types<Elm>::List;
              
              EXPECT (Prefix,  "-<1>-<2>-");
              EXPECT (ElmL  ,  "-<3>-"    );
              
              using    Elm1 = PickLast<ElmL>::Type;
              using NPrefix = PickLast<ElmL>::List;
              
              EXPECT (Types<Elm1>, "-<3>-");
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
          
          
          
          /** @test splice (or rather paste) a list on top of a base list.
           *  @note a zero-splice can be used to extract arbitrary sublists
           *  @remark the intended use case is to manipulate some parameters
           *          in a given function-type argument list
           *  @remark this test is so extensive (really complete coverage),
           *          since in 2025 a malfunction was mistakenly suspected.
           */
          void
          verify_splice ()
            { // various base lists
              using BaLi1 = Types<Num<1>>::List;                EXPECT (BaLi1, "-<1>-");
              using BaLi2 = Types<Num<1>,Num<2>>::List;         EXPECT (BaLi2, "-<1>-<2>-");
              using BaLi3 = Types<Num<1>,Num<2>,Num<3>>::List;  EXPECT (BaLi3, "-<1>-<2>-<3>-");
              using BaLi5 = Types<Num<1>,Num<2>,Num<3>,Num<4>,Num<5>>::List;
                                                                EXPECT (BaLi5, "-<1>-<2>-<3>-<4>-<5>-");
              
              // will "paste" those overlay lists "on top" the base typelists...
              using OLi1 = Types<Num<9>>::List;                 EXPECT (OLi1, "-<9>-");
              using OLi2 = Types<Num<9>,Num<8>>::List;          EXPECT (OLi2, "-<9>-<8>-");
              using OLi3 = Types<Num<9>,Num<8>,Num<7>>::List;   EXPECT (OLi3, "-<9>-<8>-<7>-");
              
              
               ///////////////////////////////////////////////////
              // (1) simple cases : on top of 3-element base list
              using Spli01 = Splice<BaLi3, OLi1>;            EXPECT (Spli01, "-<9>-<2>-<3>-");
              using Spli02 = Splice<BaLi3, OLi1, 1>;         EXPECT (Spli02, "-<1>-<9>-<3>-");
              using Spli03 = Splice<BaLi3, OLi1, 2>;         EXPECT (Spli03, "-<1>-<2>-<9>-");
              using Spli04 = Splice<BaLi3, OLi1, 3>;         EXPECT (Spli04, "-<1>-<2>-<3>-");
              using Spli05 = Splice<BaLi3, OLi1, 5>;         EXPECT (Spli05, "-<1>-<2>-<3>-");

              using Spli06 = Splice<BaLi3, OLi2, 0>;         EXPECT (Spli06, "-<9>-<8>-<3>-");
              using Spli07 = Splice<BaLi3, OLi2, 1>;         EXPECT (Spli07, "-<1>-<9>-<8>-");
              using Spli08 = Splice<BaLi3, OLi2, 2>;         EXPECT (Spli08, "-<1>-<2>-<9>-");
              using Spli09 = Splice<BaLi3, OLi2, 3>;         EXPECT (Spli09, "-<1>-<2>-<3>-");
              using Spli10 = Splice<BaLi3, OLi2, 5>;         EXPECT (Spli10, "-<1>-<2>-<3>-");

              using Spli11 = Splice<BaLi3, OLi3, 0>;         EXPECT (Spli11, "-<9>-<8>-<7>-");
              using Spli12 = Splice<BaLi3, OLi3, 1>;         EXPECT (Spli12, "-<1>-<9>-<8>-");
              using Spli13 = Splice<BaLi3, OLi3, 2>;         EXPECT (Spli13, "-<1>-<2>-<9>-");
              using Spli14 = Splice<BaLi3, OLi3, 3>;         EXPECT (Spli14, "-<1>-<2>-<3>-");
              using Spli15 = Splice<BaLi3, OLi3, 5>;         EXPECT (Spli15, "-<1>-<2>-<3>-");
              
              // (1b) corresponding Front / Back cases
              using Frnt01 = Splice<BaLi3, OLi1>   ::Front;  EXPECT (Frnt01, "-");
              using Frnt02 = Splice<BaLi3, OLi1, 1>::Front;  EXPECT (Frnt02, "-<1>-");
              using Frnt03 = Splice<BaLi3, OLi1, 2>::Front;  EXPECT (Frnt03, "-<1>-<2>-");
              using Frnt04 = Splice<BaLi3, OLi1, 3>::Front;  EXPECT (Frnt04, "-<1>-<2>-<3>-");
              using Frnt05 = Splice<BaLi3, OLi1, 5>::Front;  EXPECT (Frnt05, "-<1>-<2>-<3>-");
              
              using Frnt06 = Splice<BaLi3, OLi2, 0>::Front;  EXPECT (Frnt06, "-");
              using Frnt07 = Splice<BaLi3, OLi2, 1>::Front;  EXPECT (Frnt07, "-<1>-");
              using Frnt08 = Splice<BaLi3, OLi2, 2>::Front;  EXPECT (Frnt08, "-<1>-<2>-");
              using Frnt09 = Splice<BaLi3, OLi2, 3>::Front;  EXPECT (Frnt09, "-<1>-<2>-<3>-");
              using Frnt10 = Splice<BaLi3, OLi2, 5>::Front;  EXPECT (Frnt10, "-<1>-<2>-<3>-");
              
              using Frnt11 = Splice<BaLi3, OLi3, 0>::Front;  EXPECT (Frnt11, "-");
              using Frnt12 = Splice<BaLi3, OLi3, 1>::Front;  EXPECT (Frnt12, "-<1>-");
              using Frnt13 = Splice<BaLi3, OLi3, 2>::Front;  EXPECT (Frnt13, "-<1>-<2>-");
              using Frnt14 = Splice<BaLi3, OLi3, 3>::Front;  EXPECT (Frnt14, "-<1>-<2>-<3>-");
              using Frnt15 = Splice<BaLi3, OLi3, 5>::Front;  EXPECT (Frnt15, "-<1>-<2>-<3>-");

              using Back01 = Splice<BaLi3, OLi1>   ::Back;   EXPECT (Back01, "-<2>-<3>-");
              using Back02 = Splice<BaLi3, OLi1, 1>::Back;   EXPECT (Back02, "-<3>-");
              using Back03 = Splice<BaLi3, OLi1, 2>::Back;   EXPECT (Back03, "-");
              using Back04 = Splice<BaLi3, OLi1, 3>::Back;   EXPECT (Back04, "-");
              using Back05 = Splice<BaLi3, OLi1, 5>::Back;   EXPECT (Back05, "-");
              
              using Back06 = Splice<BaLi3, OLi2, 0>::Back;   EXPECT (Back06, "-<3>-");
              using Back07 = Splice<BaLi3, OLi2, 1>::Back;   EXPECT (Back07, "-");
              using Back08 = Splice<BaLi3, OLi2, 2>::Back;   EXPECT (Back08, "-");
              using Back09 = Splice<BaLi3, OLi2, 3>::Back;   EXPECT (Back09, "-");
              using Back10 = Splice<BaLi3, OLi2, 5>::Back;   EXPECT (Back10, "-");
              
              using Back11 = Splice<BaLi3, OLi3, 0>::Back;   EXPECT (Back11, "-");
              using Back12 = Splice<BaLi3, OLi3, 1>::Back;   EXPECT (Back12, "-");
              using Back13 = Splice<BaLi3, OLi3, 2>::Back;   EXPECT (Back13, "-");
              using Back14 = Splice<BaLi3, OLi3, 3>::Back;   EXPECT (Back14, "-");
              using Back15 = Splice<BaLi3, OLi3, 5>::Back;   EXPECT (Back15, "-");

              
               ///////////////////////////////////////////////////
              // (2) simple cases : on top of 5-element base list
              using Spli16 = Splice<BaLi5, OLi1>;            EXPECT (Spli16, "-<9>-<2>-<3>-<4>-<5>-");
              using Spli17 = Splice<BaLi5, OLi1, 1>;         EXPECT (Spli17, "-<1>-<9>-<3>-<4>-<5>-");
              using Spli18 = Splice<BaLi5, OLi1, 2>;         EXPECT (Spli18, "-<1>-<2>-<9>-<4>-<5>-");
              using Spli19 = Splice<BaLi5, OLi1, 3>;         EXPECT (Spli19, "-<1>-<2>-<3>-<9>-<5>-");
              using Spli20 = Splice<BaLi5, OLi1, 4>;         EXPECT (Spli20, "-<1>-<2>-<3>-<4>-<9>-");
              using Spli21 = Splice<BaLi5, OLi1, 5>;         EXPECT (Spli21, "-<1>-<2>-<3>-<4>-<5>-");
              using Spli22 = Splice<BaLi5, OLi1, 8>;         EXPECT (Spli22, "-<1>-<2>-<3>-<4>-<5>-");

              using Spli23 = Splice<BaLi5, OLi2, 0>;         EXPECT (Spli23, "-<9>-<8>-<3>-<4>-<5>-");
              using Spli24 = Splice<BaLi5, OLi2, 1>;         EXPECT (Spli24, "-<1>-<9>-<8>-<4>-<5>-");
              using Spli25 = Splice<BaLi5, OLi2, 2>;         EXPECT (Spli25, "-<1>-<2>-<9>-<8>-<5>-");
              using Spli26 = Splice<BaLi5, OLi2, 3>;         EXPECT (Spli26, "-<1>-<2>-<3>-<9>-<8>-");
              using Spli27 = Splice<BaLi5, OLi2, 4>;         EXPECT (Spli27, "-<1>-<2>-<3>-<4>-<9>-");
              using Spli28 = Splice<BaLi5, OLi2, 5>;         EXPECT (Spli28, "-<1>-<2>-<3>-<4>-<5>-");
              using Spli29 = Splice<BaLi5, OLi2, 8>;         EXPECT (Spli29, "-<1>-<2>-<3>-<4>-<5>-");

              using Spli30 = Splice<BaLi5, OLi3, 0>;         EXPECT (Spli30, "-<9>-<8>-<7>-<4>-<5>-");
              using Spli31 = Splice<BaLi5, OLi3, 1>;         EXPECT (Spli31, "-<1>-<9>-<8>-<7>-<5>-");
              using Spli32 = Splice<BaLi5, OLi3, 2>;         EXPECT (Spli32, "-<1>-<2>-<9>-<8>-<7>-");
              using Spli33 = Splice<BaLi5, OLi3, 3>;         EXPECT (Spli33, "-<1>-<2>-<3>-<9>-<8>-");
              using Spli34 = Splice<BaLi5, OLi3, 4>;         EXPECT (Spli34, "-<1>-<2>-<3>-<4>-<9>-");
              using Spli35 = Splice<BaLi5, OLi3, 5>;         EXPECT (Spli35, "-<1>-<2>-<3>-<4>-<5>-");
              using Spli36 = Splice<BaLi5, OLi3, 8>;         EXPECT (Spli36, "-<1>-<2>-<3>-<4>-<5>-");
              
              // (2b) corresponding Front / Back cases
              using Frnt16 = Splice<BaLi5, OLi1>   ::Front;  EXPECT (Frnt16, "-");
              using Frnt17 = Splice<BaLi5, OLi1, 1>::Front;  EXPECT (Frnt17, "-<1>-");
              using Frnt18 = Splice<BaLi5, OLi1, 2>::Front;  EXPECT (Frnt18, "-<1>-<2>-");
              using Frnt19 = Splice<BaLi5, OLi1, 3>::Front;  EXPECT (Frnt19, "-<1>-<2>-<3>-");
              using Frnt20 = Splice<BaLi5, OLi1, 4>::Front;  EXPECT (Frnt20, "-<1>-<2>-<3>-<4>-");
              using Frnt21 = Splice<BaLi5, OLi1, 5>::Front;  EXPECT (Frnt21, "-<1>-<2>-<3>-<4>-<5>-");
              using Frnt22 = Splice<BaLi5, OLi1, 8>::Front;  EXPECT (Frnt22, "-<1>-<2>-<3>-<4>-<5>-");
              
              using Frnt23 = Splice<BaLi5, OLi2, 0>::Front;  EXPECT (Frnt23, "-");
              using Frnt24 = Splice<BaLi5, OLi2, 1>::Front;  EXPECT (Frnt24, "-<1>-");
              using Frnt25 = Splice<BaLi5, OLi2, 2>::Front;  EXPECT (Frnt25, "-<1>-<2>-");
              using Frnt26 = Splice<BaLi5, OLi2, 3>::Front;  EXPECT (Frnt26, "-<1>-<2>-<3>-");
              using Frnt27 = Splice<BaLi5, OLi2, 4>::Front;  EXPECT (Frnt27, "-<1>-<2>-<3>-<4>-");
              using Frnt28 = Splice<BaLi5, OLi2, 5>::Front;  EXPECT (Frnt28, "-<1>-<2>-<3>-<4>-<5>-");
              using Frnt29 = Splice<BaLi5, OLi2, 8>::Front;  EXPECT (Frnt29, "-<1>-<2>-<3>-<4>-<5>-");
              
              using Frnt30 = Splice<BaLi5, OLi3, 0>::Front;  EXPECT (Frnt30, "-");
              using Frnt31 = Splice<BaLi5, OLi3, 1>::Front;  EXPECT (Frnt31, "-<1>-");
              using Frnt32 = Splice<BaLi5, OLi3, 2>::Front;  EXPECT (Frnt32, "-<1>-<2>-");
              using Frnt33 = Splice<BaLi5, OLi3, 3>::Front;  EXPECT (Frnt33, "-<1>-<2>-<3>-");
              using Frnt34 = Splice<BaLi5, OLi3, 4>::Front;  EXPECT (Frnt34, "-<1>-<2>-<3>-<4>-");
              using Frnt35 = Splice<BaLi5, OLi3, 5>::Front;  EXPECT (Frnt35, "-<1>-<2>-<3>-<4>-<5>-");
              using Frnt36 = Splice<BaLi5, OLi3, 8>::Front;  EXPECT (Frnt36, "-<1>-<2>-<3>-<4>-<5>-");

              using Back16 = Splice<BaLi5, OLi1>   ::Back;   EXPECT (Back16, "-<2>-<3>-<4>-<5>-");
              using Back17 = Splice<BaLi5, OLi1, 1>::Back;   EXPECT (Back17, "-<3>-<4>-<5>-");
              using Back18 = Splice<BaLi5, OLi1, 2>::Back;   EXPECT (Back18, "-<4>-<5>-");
              using Back19 = Splice<BaLi5, OLi1, 3>::Back;   EXPECT (Back19, "-<5>-");
              using Back20 = Splice<BaLi5, OLi1, 4>::Back;   EXPECT (Back20, "-");
              using Back21 = Splice<BaLi5, OLi1, 5>::Back;   EXPECT (Back21, "-");
              using Back22 = Splice<BaLi5, OLi1, 8>::Back;   EXPECT (Back22, "-");
              
              using Back23 = Splice<BaLi5, OLi2, 0>::Back;   EXPECT (Back23, "-<3>-<4>-<5>-");
              using Back24 = Splice<BaLi5, OLi2, 1>::Back;   EXPECT (Back24, "-<4>-<5>-");
              using Back25 = Splice<BaLi5, OLi2, 2>::Back;   EXPECT (Back25, "-<5>-");
              using Back26 = Splice<BaLi5, OLi2, 3>::Back;   EXPECT (Back26, "-");
              using Back27 = Splice<BaLi5, OLi2, 4>::Back;   EXPECT (Back27, "-");
              using Back28 = Splice<BaLi5, OLi2, 5>::Back;   EXPECT (Back28, "-");
              using Back29 = Splice<BaLi5, OLi2, 8>::Back;   EXPECT (Back29, "-");
              
              using Back30 = Splice<BaLi5, OLi3, 0>::Back;   EXPECT (Back30, "-<4>-<5>-");
              using Back31 = Splice<BaLi5, OLi3, 1>::Back;   EXPECT (Back31, "-<5>-");
              using Back32 = Splice<BaLi5, OLi3, 2>::Back;   EXPECT (Back32, "-");
              using Back33 = Splice<BaLi5, OLi3, 3>::Back;   EXPECT (Back33, "-");
              using Back34 = Splice<BaLi5, OLi3, 4>::Back;   EXPECT (Back34, "-");
              using Back35 = Splice<BaLi5, OLi3, 5>::Back;   EXPECT (Back35, "-");
              using Back36 = Splice<BaLi5, OLi3, 8>::Back;   EXPECT (Back36, "-");

              
               /////////////////////////////////////////////////////////
              // (3) degenerate case : excess overlay over smaller base
              using Spli37 = Splice<BaLi2, OLi3, 0>;         EXPECT (Spli37, "-<9>-<8>-");
              using Spli38 = Splice<BaLi2, OLi3, 1>;         EXPECT (Spli38, "-<1>-<9>-");
              using Spli39 = Splice<BaLi2, OLi3, 2>;         EXPECT (Spli39, "-<1>-<2>-");
              using Spli40 = Splice<BaLi2, OLi3, 5>;         EXPECT (Spli40, "-<1>-<2>-");
              
              using Spli41 = Splice<BaLi1, OLi3, 0>;         EXPECT (Spli41, "-<9>-");
              using Spli42 = Splice<BaLi1, OLi3, 1>;         EXPECT (Spli42, "-<1>-");
              using Spli43 = Splice<BaLi1, OLi3, 2>;         EXPECT (Spli43, "-<1>-");
              using Spli44 = Splice<BaLi1, OLi3, 5>;         EXPECT (Spli44, "-<1>-");
              
              // (3b) corresponding Front / Back cases
              using Frnt37 = Splice<BaLi2, OLi3, 0>::Front;  EXPECT (Frnt37, "-");
              using Frnt38 = Splice<BaLi2, OLi3, 1>::Front;  EXPECT (Frnt38, "-<1>-");
              using Frnt39 = Splice<BaLi2, OLi3, 2>::Front;  EXPECT (Frnt39, "-<1>-<2>-");
              using Frnt40 = Splice<BaLi2, OLi3, 5>::Front;  EXPECT (Frnt40, "-<1>-<2>-");
              
              using Frnt41 = Splice<BaLi1, OLi3, 0>::Front;  EXPECT (Frnt41, "-");
              using Frnt42 = Splice<BaLi1, OLi3, 1>::Front;  EXPECT (Frnt42, "-<1>-");
              using Frnt43 = Splice<BaLi1, OLi3, 2>::Front;  EXPECT (Frnt43, "-<1>-");
              using Frnt44 = Splice<BaLi1, OLi3, 5>::Front;  EXPECT (Frnt44, "-<1>-");
              
              using Back37 = Splice<BaLi2, OLi3, 0>::Back;   EXPECT (Back37, "-");
              using Back38 = Splice<BaLi2, OLi3, 1>::Back;   EXPECT (Back38, "-");
              using Back39 = Splice<BaLi2, OLi3, 2>::Back;   EXPECT (Back39, "-");
              using Back40 = Splice<BaLi2, OLi3, 5>::Back;   EXPECT (Back40, "-");
              
              using Back41 = Splice<BaLi1, OLi3, 0>::Back;   EXPECT (Back41, "-");
              using Back42 = Splice<BaLi1, OLi3, 1>::Back;   EXPECT (Back42, "-");
              using Back43 = Splice<BaLi1, OLi3, 2>::Back;   EXPECT (Back43, "-");
              using Back44 = Splice<BaLi1, OLi3, 5>::Back;   EXPECT (Back44, "-");

              
               ///////////////////////////////////
              // (4) degenerate case : empty base
              using Spli45 = Splice<Nil,   OLi3, 0>;         EXPECT (Spli45, "-");
              using Spli46 = Splice<Nil,   OLi3, 1>;         EXPECT (Spli46, "-");
              using Spli47 = Splice<Nil,   OLi3, 5>;         EXPECT (Spli47, "-");
              
              using Spli48 = Splice<Nil,   OLi1, 0>;         EXPECT (Spli48, "-");
              using Spli49 = Splice<Nil,   OLi1, 1>;         EXPECT (Spli49, "-");
              using Spli50 = Splice<Nil,   OLi1, 5>;         EXPECT (Spli50, "-");
              
              using Spli51 = Splice<Nil,   Nil,  0>;         EXPECT (Spli51, "-");
              using Spli52 = Splice<Nil,   Nil,  1>;         EXPECT (Spli52, "-");
              using Spli53 = Splice<Nil,   Nil,  5>;         EXPECT (Spli53, "-");
              
              // (4b) corresponding Front / Back cases
              using Frnt45 = Splice<Nil,   OLi3, 0>::Front;  EXPECT (Frnt45, "-");
              using Frnt46 = Splice<Nil,   OLi3, 1>::Front;  EXPECT (Frnt46, "-");
              using Frnt47 = Splice<Nil,   OLi3, 5>::Front;  EXPECT (Frnt47, "-");
              
              using Frnt48 = Splice<Nil,   OLi1, 0>::Front;  EXPECT (Frnt48, "-");
              using Frnt49 = Splice<Nil,   OLi1, 1>::Front;  EXPECT (Frnt49, "-");
              using Frnt50 = Splice<Nil,   OLi1, 5>::Front;  EXPECT (Frnt50, "-");
              
              using Frnt51 = Splice<Nil,   Nil,  0>::Front;  EXPECT (Frnt51, "-");
              using Frnt52 = Splice<Nil,   Nil,  1>::Front;  EXPECT (Frnt52, "-");
              using Frnt53 = Splice<Nil,   Nil,  5>::Front;  EXPECT (Frnt53, "-");

              using Back45 = Splice<Nil,   OLi3, 0>::Back;   EXPECT (Back45, "-");
              using Back46 = Splice<Nil,   OLi3, 1>::Back;   EXPECT (Back46, "-");
              using Back47 = Splice<Nil,   OLi3, 5>::Back;   EXPECT (Back47, "-");
              
              using Back48 = Splice<Nil,   OLi1, 0>::Back;   EXPECT (Back48, "-");
              using Back49 = Splice<Nil,   OLi1, 1>::Back;   EXPECT (Back49, "-");
              using Back50 = Splice<Nil,   OLi1, 5>::Back;   EXPECT (Back50, "-");
              
              using Back51 = Splice<Nil,   Nil,  0>::Back;   EXPECT (Back51, "-");
              using Back52 = Splice<Nil,   Nil,  1>::Back;   EXPECT (Back52, "-");
              using Back53 = Splice<Nil,   Nil,  5>::Back;   EXPECT (Back53, "-");

              
               //////////////////////////////////////////
              // (4) special case : zero-splice is split
              using Spli54 = Splice<BaLi3, Nil,  0>;         EXPECT (Spli54, "-<1>-<2>-<3>-");
              using Spli55 = Splice<BaLi3, Nil,  1>;         EXPECT (Spli55, "-<1>-<2>-<3>-");
              using Spli56 = Splice<BaLi3, Nil,  2>;         EXPECT (Spli56, "-<1>-<2>-<3>-");
              using Spli57 = Splice<BaLi3, Nil,  3>;         EXPECT (Spli57, "-<1>-<2>-<3>-");
              using Spli58 = Splice<BaLi3, Nil,  4>;         EXPECT (Spli58, "-<1>-<2>-<3>-");
              using Spli59 = Splice<BaLi3, Nil,  5>;         EXPECT (Spli59, "-<1>-<2>-<3>-");
              
              // (4b) Front / Back cases : split parts
              using Frnt54 = Splice<BaLi3, Nil,  0>::Front;  EXPECT (Frnt54, "-");
              using Frnt55 = Splice<BaLi3, Nil,  1>::Front;  EXPECT (Frnt55, "-<1>-");
              using Frnt56 = Splice<BaLi3, Nil,  2>::Front;  EXPECT (Frnt56, "-<1>-<2>-");
              using Frnt57 = Splice<BaLi3, Nil,  3>::Front;  EXPECT (Frnt57, "-<1>-<2>-<3>-");
              using Frnt58 = Splice<BaLi3, Nil,  4>::Front;  EXPECT (Frnt58, "-<1>-<2>-<3>-");
              using Frnt59 = Splice<BaLi3, Nil,  5>::Front;  EXPECT (Frnt59, "-<1>-<2>-<3>-");
              
              using Back54 = Splice<BaLi3, Nil,  0>::Back;   EXPECT (Back54, "-<1>-<2>-<3>-");
              using Back55 = Splice<BaLi3, Nil,  1>::Back;   EXPECT (Back55, "-<2>-<3>-");
              using Back56 = Splice<BaLi3, Nil,  2>::Back;   EXPECT (Back56, "-<3>-");
              using Back57 = Splice<BaLi3, Nil,  3>::Back;   EXPECT (Back57, "-");
              using Back58 = Splice<BaLi3, Nil,  4>::Back;   EXPECT (Back58, "-");
              using Back59 = Splice<BaLi3, Nil,  5>::Back;   EXPECT (Back59, "-");
              
              // Note: these special usages are provided as shorthand
              using Prfx55 = Prefix<BaLi3, 1>;               EXPECT (Prfx55, "-<1>-");
              using Sufx55 = Suffix<BaLi3, 1>;               EXPECT (Sufx55, "-<2>-<3>-");
              using Prfx56 = Prefix<BaLi3, 2>;               EXPECT (Prfx56, "-<1>-<2>-");
              using Sufx56 = Suffix<BaLi3, 2>;               EXPECT (Sufx56, "-<3>-");
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
              
              using HeadEnd = Types<Head,End>;      EXPECT (HeadEnd, "-<1>-<7>-");
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
              using List_of_Lists = Types<List1::List
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
              
              using Prefixes = Types<Num<11>,Num<22>,Num<33>>::List;
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
              
              using LioLi = Types<List1::List,List2::List>::List;
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
