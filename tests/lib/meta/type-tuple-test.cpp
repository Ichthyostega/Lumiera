/*
  TypeTuple(Test)  -  checking type tuples and records based on them
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/


/** @file type-tuple-test.cpp
 ** Interconnection of typelists, type tuples and simple record
 ** data types build on top of them.  
 ** @todo define function-closure-test
 **
 ** @see lumiera::typelist::Tuple
 ** @see tuple.hpp
 ** @see function-closure.hpp
 ** @see control::CmdClosure real world usage example
 **
 */


#include "lib/test/run.hpp"
#include "lib/meta/typelist.hpp"   ////////////TODO really?
#include "lib/meta/tuple.hpp"
#include "meta/typelist-diagnostics.hpp"
#include "meta/tuple-diagnostics.hpp"

#include <boost/utility/enable_if.hpp>
#include <boost/format.hpp>
#include <iostream>

using ::test::Test;
  
using std::string;
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
   *       at runtime. Effectively, these are simple record types, which are
   *       synthesised by recursion over the related typelist.
   *       - create tuples from a list of values
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
          typedef Types1::List L1;  // starting from an existing Typelist...
          
          typedef Tuple<L1> T_L1;           // ListType based tuple type
          typedef Tuple<L1>::ThisTuple T1;  // corresponding plain tuple type
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
        }
      
      
      void
      check_sub_tuple_types()
        {
          cout << "\t:\n\t: ---Head-and-Tail---\n";
          
          typedef Append<Types2::List, Types1::List>::List L2;
          
          typedef Tuple<L2> T_L2;
          typedef Types<T_L2::HeadType> Head;
          typedef T_L2::TailType Tail;
          DISPLAY (T_L2);
          DISPLAY (Head);
          DISPLAY (Tail);
          
          typedef T_L2::ThisTuple T2;
          typedef Types<T2::HeadType> Head2;
          typedef T2::TailType Tail2;
          DISPLAY (T2);
          DISPLAY (Head2);
          DISPLAY (Tail2);
        }
      
      
      void
      check_shiftedTuple()
        {
          cout << "\t:\n\t: ---Shifted-Tuple---\n";
          
          typedef Append<Types2::List, Types3::List>::List L3;
          typedef Tuple<L3>::Type Ty3;
          typedef Tuple<Ty3>      T3;
          
          typedef Shifted<Ty3,0>::Type Ty_0;  DISPLAY (Ty_0);
          typedef Shifted<Ty3,1>::Type Ty_1;  DISPLAY (Ty_1);
          typedef Shifted<Ty3,2>::Type Ty_2;  DISPLAY (Ty_2);
          typedef Shifted<Ty3,3>::Type Ty_3;  DISPLAY (Ty_3);
          typedef Shifted<Ty3,4>::Type Ty_4;  DISPLAY (Ty_4);
          
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
          UNIMPLEMENTED ("verify tuple creation");
          
        }
      
      
      void
      check_tuple_copy()
        {
          UNIMPLEMENTED ("verify tuple copying and assignment");
          
        }
      
      
      void
      check_value_access()
        {
          UNIMPLEMENTED ("verify tuple value access");
          
        }

    };
  
  
  /** Register this test class... */
  LAUNCHER (TypeTuple_test, "unit common");
  
  
  
}}} // namespace lumiera::typelist::test
