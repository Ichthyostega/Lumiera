/*
  FunctionClosure(Test)  -  appending, mixing and filtering typelists

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file function-closure-test.cpp
 ** Testing a combination of std::function objects and metaprogramming.
 ** Argument types will be extracted and represented as typelist, so they
 ** can be manipulated at compile time. This test uses some test functions
 ** and systematically applies or binds them to corresponding data tuples.
 ** Moreover, closure objects will be constructed in various flavours,
 ** combining a function object and a set of parameters.
 ** 
 ** @see function-closure.hpp
 ** @see control::CmdClosure real world usage example
 **
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-manip.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/function-closure.hpp"
#include "meta/typelist-diagnostics.hpp"
#include "meta/tuple-diagnostics.hpp"


using std::string;


namespace lib  {
namespace meta {
namespace test {
      
      
      namespace { // test data
        
        using List1 = Types<Num<1>, Num<2>, Num<3> >::List;
        using List2 = Types<Num<5>, Num<6>, Num<7> >::List;
        
        
        /** special test fun 
         *  accepting the terrific Num types */
        template<char i,char ii, char iii>
        int
        getNumberz (Num<i> one, Num<ii> two, Num<iii> three)
          {
            return one.o_ + two.o_ + three.o_;
          }
        
        
        int fun0 ()                       { return -1;       }
        int fun1 (int i1)                 { return i1;       }
        int fun2 (int i1, int i2)         { return i1+i2;    }
        int fun3 (int i1, int i2, int i3) { return i1+i2+i3; }
        
      } // (End) test data
  
  
  
  
  using func::bindArgTuple;
  
  
  /*********************************************************************//**
   * @test building a function closure for a given function or functor,
   *       while arguments are passed in as tuple
   *       - accessing signatures as typelists
   *       - bind free function to tuple
   *       - bind functor to tuple
   * @remark this test is _rather low-level_ and documents the construction
   *         of the implementation; furthermore, most of this construction
   *         was obsoleted by newer language features, notably std::apply
   *         and the technique to unpack variadic-λ arguments.
   *         What remains, is now largely a definition how to handle
   *         function argument list signatures, to build suitable
   *         argument tuple types by metaprogramming, and finally
   *         to pass them to construct a binder.
   * @see function-composition-test.cpp (advanced features like partial application)
   */
  class FunctionClosure_test : public Test
    {
      virtual void
      run (Arg)
        {
          verify_setup();
          check_signatureTypeManip();
          check_bindFree();
          check_bindFunc();
        }
      
      
      /** verify the test input data
       *  @see TypeListManipl_test#check_diagnostics() for
       *       explanation of the DISPLAY and EXPECT macros.
       */
      void
      verify_setup()
        {
          DISPLAY (List1);
          DISPLAY (List2);
          ;
          CHECK (6 == (getNumberz<1,2,3> (Num<1>(), Num<2>(), Num<3>())));
          CHECK (6 == (getNumberz<1,1,1> (Num<1>(), Num<1>(2), Num<1>(3))));
        }
      
      
      void
      check_signatureTypeManip ()
        {
          typedef int someFunc(Num<5>,Num<9>);
          typedef _Fun<someFunc>::Ret RetType;  // should be int
          typedef _Fun<someFunc>::Args Args;
          DISPLAY (Args);
          
          typedef Prepend<Num<1>, Args>::Seq NewArgs;               // manipulate the argument type(s)
          DISPLAY (NewArgs);
          
          typedef BuildFunType<RetType,NewArgs>::Sig NewSig;     // re-build a new function signature
          
          NewSig& fun =  getNumberz<1,5,9>;                    //...which is compatible to an existing real function signature!
          
          CHECK (1+5+9 == fun(Num<1>(), Num<5>(), Num<9>()));
        }
      
      
      void
      check_bindFree ()
        {
          cout << "\t:\n\t: ---Bind----\n";
          
          Tuple<Types<>>            tup0 ;
          Tuple<Types<int>>         tup1 (11);
          Tuple<Types<int,int>>     tup2 (11,12);
          Tuple<Types<int,int,int>> tup3 (11,12,13);
          DUMPVAL (tup0);
          DUMPVAL (tup1);
          DUMPVAL (tup2);
          DUMPVAL (tup3);
          
          using BoundFun = function<int()>;
          
          BoundFun functor0 = bindArgTuple (fun0, tup0);
          BoundFun functor1 = bindArgTuple (fun1, tup1);
          BoundFun functor2 = bindArgTuple (fun2, tup2);
          BoundFun functor3 = bindArgTuple (fun3, tup3);
          
          CHECK (-1       == functor0() );
          CHECK (11       == functor1() );
          CHECK (11+12    == functor2() );
          CHECK (11+12+13 == functor3() );
          
        }
      
      
      void
      check_bindFunc ()
        {
          Tuple<Types<>>             tup0 ;
          Tuple<Types<int>>          tup1 (11);
          Tuple<Types<int,int>>      tup2 (11,12);
          Tuple<Types<int,int,int>>  tup3 (11,12,13);
          function<int()>            unbound_functor0 (fun0);
          function<int(int)>         unbound_functor1 (fun1);
          function<int(int,int)>     unbound_functor2 (fun2);
          function<int(int,int,int)> unbound_functor3 (fun3);
          
          using BoundFun = function<int()>;
          
          BoundFun functor0 = bindArgTuple (unbound_functor0, tup0);
          BoundFun functor1 = bindArgTuple (unbound_functor1, tup1);
          BoundFun functor2 = bindArgTuple (unbound_functor2, tup2);
          BoundFun functor3 = bindArgTuple (unbound_functor3, tup3);
          
          CHECK (-1       == functor0() );
          CHECK (11       == functor1() );
          CHECK (11+12    == functor2() );
          CHECK (11+12+13 == functor3() );
          
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (FunctionClosure_test, "unit common");
  
  
  
}}} // namespace lib::meta::test
