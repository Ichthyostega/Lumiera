/*
  TypeTuple(Test)  -  checking type tuples and records based on them

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/


/** @file type-tuple-test.cpp
 ** Interplay of typelists, type tuples and simple record
 ** data types build on top of them.
 ** 
 ** @see lumiera::typelist::Tuple
 ** @see tuple.hpp
 ** @see function-closure.hpp
 ** @see control::CmdClosure real world usage example
 **
 */


#include "lib/test/run.hpp"
#include "lib/meta/tuple.hpp"
#include "meta/typelist-diagnostics.hpp"
#include "meta/tuple-diagnostics.hpp"

#include <iostream>

using ::test::Test;
using std::cout;
using std::endl;


namespace lumiera {
namespace typelist{
namespace test {
  
  
  
  namespace { // test data
    
    
    typedef Types< Num<1>
                 , Num<3>
                 , Num<5>
                 >          Types1;
    typedef Types< Num<2>
                 , Num<4>
                 >          Types2;
    typedef Types< Num<7> > Types3;
    
    
    
  } // (End) test data
  
  
  
  
  /*************************************************************************
   * @test Cover various aspects of the type tuple.
   *       Check the metaprogramming behaviour...
   *       - build a tuple type from an existing typelist
   *       - create sub tuple types and types with shifted parameters
   *       Additionally, check the behaviour when creating tuple instances
   *       at runtime. Effectively, these are simple record types, which
   *       are synthesised by recursion over the related typelist.
   *       - diagnostics through TupleAccessor retrieving stored values
   *       - creating tuples by direct function call, providing values
   *       - creating tuples partially from an existing sub-argument tuple
   *       - copy and copy construct
   *       - access the "head" and access values by numeric index
   *       - create a tuple with shifted values
   */
  class TypeTuple_test : public Test
    {
      virtual void
      run (Arg)
        {
          check_diagnostics();
          check_tuple_from_Typelist();
          check_sub_tuple_types();
          check_shiftedTuple();
          check_tuple_creation();
          check_build_from_subTuple();
          check_tuple_copy();
          check_value_access();
        }
      
      
      /** verify the test input data
       *  @see TypeListManipl_test#check_diagnostics()
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
          Tup1 tup1x (Num<1>(11));
          
          DISPLAY (Tup1);     // prints the type
          DUMPVAL (Tup1());   // prints the contents
          DUMPVAL (tup1x);
        }
      
      
      void
      check_tuple_from_Typelist()
        {
          typedef Types1::List L1; // starting from an existing Typelist...
          
          typedef Tuple<L1> T_L1;           // ListType based tuple type
          typedef Tuple<L1>::TupleType T1;  // corresponding plain tuple type
          typedef Tuple<L1>::Type Type1;    // extract the underlying type sequence
          
          DISPLAY (Type1);
          DISPLAY (T_L1);
          DISPLAY (T1);
          
          T_L1 tup1 (Num<1>(8));            // both flavours can be created at runtime
          T1   tup2 (Num<1>(9));            // (and we provide an explicit value for the 1st element)
          DUMPVAL (tup1);
          DUMPVAL (tup2);
          
          typedef Tuple<Node<int, L1> > Prepend;
          DISPLAY (Prepend);                // another ListType based tuple created by prepending
          
          Prepend prepend (22, tup2);       // but note: the ListType based tuple has an "(head,tail)" style ctor
          DUMPVAL (prepend);                // ... and in construction, tup2 has been copied and coerced to ListType style
          
          typedef Tuple<Types<> > NulT;     // plain-flat empty Tuple
          typedef Tuple<NullType> NulL;     // list-style empty Tuple
          
          CHECK (            is_Tuple<T1>::value);
          CHECK (       is_TuplePlain<T1>::value);
          CHECK (!   is_TupleListType<T1>::value);
          CHECK (!       is_NullTuple<T1>::value);
          
          CHECK (          is_Tuple<T_L1>::value);
          CHECK (!    is_TuplePlain<T_L1>::value);
          CHECK (  is_TupleListType<T_L1>::value);
          CHECK (!     is_NullTuple<T_L1>::value);
          
          CHECK (          is_Tuple<NulT>::value);
          CHECK (     is_TuplePlain<NulT>::value);
          CHECK (! is_TupleListType<NulT>::value);
          CHECK (      is_NullTuple<NulT>::value);
          
          CHECK (          is_Tuple<NulL>::value);
          CHECK (!    is_TuplePlain<NulL>::value);
          CHECK (  is_TupleListType<NulL>::value);
          CHECK (      is_NullTuple<NulL>::value);
          
          CHECK (!        is_Tuple<Type1>::value);
          CHECK (!   is_TuplePlain<Type1>::value);
          CHECK (!is_TupleListType<Type1>::value);
          CHECK (!    is_NullTuple<Type1>::value);
          
          CHECK (!        is_Tuple<Types1::List>::value);
          CHECK (!   is_TuplePlain<Types1::List>::value);
          CHECK (!is_TupleListType<Types1::List>::value);
          CHECK (!    is_NullTuple<Types1::List>::value);
          
        }
      
      
      void
      check_sub_tuple_types()
        {
          cout << "\t:\n\t: ---Sub-Tuple-Types----\n";
          
          typedef Append<Types2::List, Types1::List>::List L2;
          
          typedef Tuple<L2> T_L2;              // list-style Tuple
          typedef Types<T_L2::HeadType> Head;
          typedef T_L2::TailType Tail;
          DISPLAY (T_L2);
          DISPLAY (Head);
          DISPLAY (Tail);
          
          typedef T_L2::TupleType T2;          // plain-flat Tuple
          typedef Types<T2::HeadType> Head2;
          typedef T2::TailType Tail2;
          DISPLAY (T2);
          DISPLAY (Head2);
          DISPLAY (Tail2);
          
          typedef Tuple<Types<> > NulT;        // plain-flat empty Tuple
          typedef Tuple<NullType> NulL;        // list-style empty Tuple
          
          DISPLAY (T2::Type);                  // irrespective of the flavour,
          DISPLAY (T2::TailType);              // a basic set of typedefs is
          DISPLAY (T2::TupleType);             // always available
          DISPLAY (T2::ThisType);
          DISPLAY (T2::Tail);
          DISPLAY (T2::ArgList);
          
          DISPLAY (T_L2::Type);                // the element types as type sequence
          DISPLAY (T_L2::TailType);            // the element types of the "tail" tuple
          DISPLAY (T_L2::TupleType);           // corresponding plain-flat tuple type
          DISPLAY (T_L2::ThisType);            // "type_of(this)"
          DISPLAY (T_L2::Tail);                // tail tuple
          DISPLAY (T_L2::ArgList);             // typelist comprised of the element types
          
          DISPLAY (NulT::Type);
          DISPLAY (NulT::TailType);
          DISPLAY (NulT::TupleType);
          DISPLAY (NulT::ThisType);
          DISPLAY (NulT::Tail);
          DISPLAY (NulT::ArgList);
          
          DISPLAY (NulL::Type);
          DISPLAY (NulL::TailType);
          DISPLAY (NulL::TupleType);
          DISPLAY (NulL::ThisType);
          DISPLAY (NulL::Tail);
          DISPLAY (NulL::ArgList);
          
        }
      
      
      void
      check_shiftedTuple()
        {
          cout << "\t:\n\t: ---Shifted-Tuple---\n";
          
          typedef Append<Types2::List, Types3::List>::List L3;
          typedef Tuple<L3>::Type Ty3;
          typedef Tuple<Ty3>      T3;
          
          typedef Shifted<Ty3,0>::Type Ty_0;     DISPLAY (Ty_0);
          typedef Shifted<Ty3,1>::Type Ty_1;     DISPLAY (Ty_1);
          typedef Shifted<Ty3,2>::Type Ty_2;     DISPLAY (Ty_2);
          typedef Shifted<Ty3,3>::Type Ty_3;     DISPLAY (Ty_3);
          typedef Shifted<Ty3,4>::Type Ty_4;     DISPLAY (Ty_4);
          
          typedef T3::ShiftedTuple<0>::Type T_0; DISPLAY (T_0);
          typedef T3::ShiftedTuple<1>::Type T_1; DISPLAY (T_1);
          typedef T3::ShiftedTuple<2>::Type T_2; DISPLAY (T_2);
          typedef T3::ShiftedTuple<3>::Type T_3; DISPLAY (T_3);
          typedef T3::ShiftedTuple<4>::Type T_4; DISPLAY (T_4);
          
          T3 tu3;                                DUMPVAL (tu3);
          T_0 tu3_0 = tu3.getShifted<0>();       DUMPVAL (tu3_0);
          T_1 tu3_1 = tu3.getShifted<1>();       DUMPVAL (tu3_1);
          T_2 tu3_2 = tu3.getShifted<2>();       DUMPVAL (tu3_2);
          T_3 tu3_3 = tu3.getShifted<3>();       DUMPVAL (tu3_3);
          T_4 tu3_4 = tu3.getShifted<4>();       DUMPVAL (tu3_4);
        }
      
      
      void
      check_tuple_creation()
        {
          cout << "\t:\n\t: ---creating-Tuples---\n";
          
          Tuple<Types1> tup1  ;
          Tuple<Types1> tup11 (Num<1>(11) );
          Tuple<Types1> tup12 (Num<1>(),   Num<3>(33) );
          Tuple<Types1> tup13 (Num<1>(11), Num<3>(33), Num<5>() );
          DUMPVAL (tup1);
          DUMPVAL (tup11);
          DUMPVAL (tup12);
          DUMPVAL (tup13);
          
          typedef Tuple<Types<int,int,Num<11> > > Tup2;
          Tup2 tup2 = tuple::make(41,42, Num<11>(43));   // build tuple from given values
          DISPLAY (Tup2);
          DUMPVAL (tup2);
          
          typedef Tup2::Tail Tup22;
          Tup22 tup22 = tup2.getTail();
          DISPLAY (Tup22);
          DUMPVAL (tup22);
          
          typedef Tup2::Tail::Tail Tup222;
          Tup222 tup222 = tup22.getTail();
          DISPLAY (Tup222);
          DUMPVAL (tup222);
          
          typedef Tuple<Types<> > T0T;
          typedef Tuple<NullType> T0L;
          T0T nullT = tuple::makeNullTuple();
          T0L nullL = tuple::makeNullTuple();
          T0T nulTcpy (nullL);
          T0T& nulTref (nullL.tupleCast());
          DISPLAY (T0T);
          DISPLAY (T0L);
          DUMPVAL (nullT);
          DUMPVAL (nullL);
          DUMPVAL (nulTcpy);
          DUMPVAL (nulTref);
        }
      
      
      void
      check_build_from_subTuple()
        {
          cout << "\t:\n\t: ---build-from-sub-Tuples---\n";
          
          typedef Append<Types1::List, Types3::List>::List TL;
          typedef Tuple<TL>::Type                          TT;
          typedef Tuple<TL> T1357L;
          typedef Tuple<TT> T1357T;
          DISPLAY (T1357L);
          DISPLAY (T1357T);
          
          typedef Tuple<Types1::List> T135L;
          typedef Tuple<Types<Num<5>,Num<7> > > T57T;
          typedef Tuple<Types<Num<3>,Num<5> > > T35T;
          DISPLAY (T135L);
          DISPLAY (T57T);
          DISPLAY (T35T);
          
          T135L sub135;
          T57T sub57;
          T35T sub35 (Num<3>(8),Num<5>(8));
          
          DUMPVAL (sub135);
          T1357T   b_135  = tuple::BuildTuple<T1357T,T135L>::create(sub135);
          DUMPVAL (b_135);
                   b_135  = tuple::BuildTuple<T1357L,T135L>::create(sub135);
          DUMPVAL (b_135);
                   b_135  = tuple::BuildTuple<TL,Types1>::create(sub135);
          DUMPVAL (b_135);
                   b_135  = tuple::BuildTuple<TT,Types1::List>::create(sub135);
          DUMPVAL (b_135);                              // all variations of type specification lead to the same result
          
          DUMPVAL (sub57);
          T1357T   b_57   = tuple::BuildTuple<T1357T,T57T,2>::create(sub57);
          DUMPVAL (b_57);
          
          DUMPVAL (sub35);
          T1357T   b_35   = tuple::BuildTuple<T1357T,T35T,1>::create(sub35);
          DUMPVAL (b_35);
          
                   b_35   = tuple::BuildTuple<T1357T,T35T,2>::create(sub35);
          DUMPVAL (b_35);                               // note: wrong start position, argument tuple ignored completely
                   b_35   = tuple::BuildTuple<T1357T,T35T,4>::create(sub35);
          DUMPVAL (b_35);
          
          // use an argument tuple beyond the last argument of the target tuple...
          typedef  Tuple<Types<Num<7>,Num<8> > >  T78T;
          T78T     sub78 (Num<7>(77),Num<8>(88));
          DUMPVAL (sub78);
          T1357T   b_78   = tuple::BuildTuple<T1357T,T78T,3>::create(sub78);
          DUMPVAL (b_78);                              // note: superfluous arguments ignored
          
          typedef Tuple<Types<> > NulT;
          NulT     nult;
          T1357T   b_nul  = tuple::BuildTuple<T1357T,NulT,1>::create(nult);
          DUMPVAL (b_nul);
                   b_nul  = tuple::BuildTuple<T1357T,NulT,4>::create(nult);
          DUMPVAL (b_nul);
          
          NulT     b_nul2 = tuple::BuildTuple<NulT,T78T>::create(sub78);
          DUMPVAL (b_nul2)
                   b_nul2 = tuple::BuildTuple<NulT,T78T,1>::create(sub78);
          DUMPVAL (b_nul2)
        }
      
      
      void
      check_tuple_copy()
        {
          cout << "\t:\n\t: ---copy-operations---\n";
          
          Tuple<Types1> tup1 (Num<1>(11), Num<3>(33), Num<5>() );
          
          Tuple<Types1> tup11 (tup1);
          tup11.getAt<2>().o_ = 44;
          DUMPVAL (tup1);
          DUMPVAL (tup11);
          
          tup1 = tup11;
          DUMPVAL (tup1);
          
          Tuple<Types1::List> tupL = tup11.getShifted<0>();
          Tuple<Types1> tup1L (tupL);      // create plain tuple from list-style tuple
          DUMPVAL (tupL);
          DUMPVAL (tup1L);
        }
      
      
      void
      check_value_access()
        {
          cout << "\t:\n\t: ---value-access---\n";
          
          typedef Append<Types2::List, Types2::List>::List T2424;
          typedef Tuple<T2424> TupX;
          TupX tupX;
          DISPLAY (TupX);
          DUMPVAL (tupX);
          
          Tuple<Types2> tu2;
          DUMPVAL (tu2);
          tuple::element<1>(tu2).o_ = 5;
          tu2.getHead() = Num<2> (tu2.getAt<1>().o_);
          DUMPVAL (tu2);
          
          
          tupX.getShifted<2>() = tu2;
          DUMPVAL (tupX);
          
          typedef Shifted<TupX::TupleType,2>::TupleType T4;
          T4 t4 (tupX.getShifted<2>());
          DISPLAY (T4);
          DUMPVAL (t4);
          
          DISPLAY (TupX::Type)
          DISPLAY (TupX::TailType)
          DISPLAY (TupX::ThisType)
          DISPLAY (TupX::TupleType)
          
          typedef TupX::TupleType TupT;
          DISPLAY (TupT::Type)
          DISPLAY (TupT::TailType)
          DISPLAY (TupT::ThisType)
          DISPLAY (TupT::TupleType)
          
          TupT tupXcopy (tupX);
          DUMPVAL (tupXcopy);
          
          TupT& tupXcast (tupX.tupleCast());   // (down)cast list-style to plain tuple
          DUMPVAL (tupXcast);
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (TypeTuple_test, "unit common");
  
  
  
}}} // namespace lumiera::typelist::test
