/*
  FunctionClosure(Test)  -  appending, mixing and filtering typelists
 
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


/** @file function-closure-test.cpp
 ** 
 ** @todo define function-closure-test
 **
 ** @see function-closure.hpp
 ** @see control::CmdClosure real world usage example
 **
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/meta/typelist.hpp"
#include "lib/meta/typelistutil.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/function-closure.hpp"
#include "meta/dummy-functions.hpp"
#include "meta/typelist-diagnostics.hpp"
#include "meta/tuple-diagnostics.hpp"
//#include "lib/util.hpp"

//#include <boost/format.hpp>
#include <iostream>

using lib::test::showSizeof;
using lib::test::showType;

using ::test::Test;
using std::string;
using std::cout;
using std::endl;


namespace lumiera {
namespace typelist{
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
        
        
        /** special test fun 
         *  accepting the terrific Num types */
        template<char i,char ii, char iii>
        int
        getNumberz (Num<i> one, Num<ii> two, Num<iii> three)
          {
            return one.o_ + two.o_ + three.o_;
          }
        
      } // (End) test data
      
      
      
      
      
      
  /*************************************************************************
   * @test building a function closure for a given functor
   *       and arguments passed in as tuple
   *       - accessing signatures as typelists
   *       - apply free function to tuple
   *       - apply functor to tuple 
   *       - bind free function to tuple
   *       - bind functor to tuple
   *       - build a simple "tuple closure"
   */
  class FunctionClosure_test : public Test
    {
      virtual void
      run (Arg) 
        {
          check_diagnostics ();
          check_signatureTypeManip ();
          check_applyFree ();
          check_applyFunc ();
          check_bindFree  ();
          check_bindFunc  ();
          build_closure ();
        }
      
      
      /** verify the test input data
       *  @see TypeListManipl_test#check_diagnostics()
       *       for an explanation of the DISPLAY macro
       */
      void
      check_diagnostics ()
        {
          DISPLAY (List1);
          DISPLAY (List2);
          ;
          ASSERT (6 == (getNumberz<1,2,3> (Num<1>(), Num<2>(), Num<3>())));
          ASSERT (6 == (getNumberz<1,1,1> (Num<1>(), Num<1>(2), Num<1>(3))));
        }
      
      
      void
      check_signatureTypeManip ()
        {
          typedef int someFunc(Num<5>,Num<9>);
          typedef FunctionSignature<function<someFunc> >::Ret RetType;  // should be int
          typedef FunctionSignature<function<someFunc> >::Args Args;
          DISPLAY (Args);
          
          typedef Prepend<Num<1>, Args>::Tuple NewArgs;             // manipulate the argument type(s)
          DISPLAY (NewArgs);
          
          typedef FunctionTypedef<RetType,NewArgs>::Sig NewSig;  // re-build a new function signature
          
          NewSig& fun =  getNumberz<1,5,9>;                    //...which is compatible to an existing real function signature!
          
          ASSERT (1+5+9 == fun(Num<1>(), Num<5>(), Num<9>()));
        }
      
      
      void
      check_applyFree ()
        {
          UNIMPLEMENTED ("verify apply free function to tuple");
        }
      
      
      void
      check_applyFunc ()
        {
          UNIMPLEMENTED ("verify apply functor to tuple");
        }
      
      
      void
      check_bindFree ()
        {
          UNIMPLEMENTED ("verify bind free function to tuple");
        }
      
      
      void
      check_bindFunc ()
        {
          UNIMPLEMENTED ("verify bind functor to tuple");
        }
      
      
      void
      build_closure ()
        {
          UNIMPLEMENTED ("build a simple tuple closure");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (FunctionClosure_test, "unit common");
  
  
  
}}} // namespace lumiera::typelist::test
