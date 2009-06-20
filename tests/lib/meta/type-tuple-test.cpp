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
   * @test //////////////////////////////////////////
   *       - building combinations and permutations
   */
  class TypeTuple_test : public Test
    {
      virtual void
      run (Arg) 
        {
          check_diagnostics();
          
          UNIMPLEMENTED ("verify type tuples");
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
    };
  
  
  /** Register this test class... */
  LAUNCHER (TypeTuple_test, "unit common");
  
  
  
}}} // namespace lumiera::typelist::test
