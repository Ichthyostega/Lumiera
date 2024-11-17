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

#include <iostream>

using ::test::Test;
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
        
        
        int fun0 ()                       { return -1;       }
        int fun1 (int i1)                 { return i1;       }
        int fun2 (int i1, int i2)         { return i1+i2;    }
        int fun3 (int i1, int i2, int i3) { return i1+i2+i3; }
        
      } // (End) test data
  
  
  
  
  using func::Apply;
  using func::TupleApplicator;
  using func::FunctionClosure;
  using func::closure;
  using func::apply;
  
  
  /*********************************************************************//**
   * @test building a function closure for a given function or functor,
   *       while arguments are passed in as tuple
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
      check_applyFree ()
        {
          cout << "\t:\n\t: ---Apply---\n";
          
          Tuple<Types<>>            tup0 ;
          Tuple<Types<int>>         tup1 (11);
          Tuple<Types<int,int>>     tup2 (11,12);
          Tuple<Types<int,int,int>> tup3 (11,12,13);
          DUMPVAL (tup0);
          DUMPVAL (tup1);
          DUMPVAL (tup2);
          DUMPVAL (tup3);
          
          CHECK (-1       == Apply<0>::invoke<int> (fun0, tup0) );
          CHECK (11       == Apply<1>::invoke<int> (fun1, tup1) );
          CHECK (11+12    == Apply<2>::invoke<int> (fun2, tup2) );
          CHECK (11+12+13 == Apply<3>::invoke<int> (fun3, tup3) );
          
          CHECK (-1       == TupleApplicator<int()>            (tup0) (fun0) );
          CHECK (11       == TupleApplicator<int(int)>         (tup1) (fun1) );
          CHECK (11+12    == TupleApplicator<int(int,int)>     (tup2) (fun2) );
          CHECK (11+12+13 == TupleApplicator<int(int,int,int)> (tup3) (fun3) );
          
          CHECK (-1       == apply(fun0, tup0) );
          CHECK (11       == apply(fun1, tup1) );
          CHECK (11+12    == apply(fun2, tup2) );
          CHECK (11+12+13 == apply(fun3, tup3) );
        
        }
      
      
      void
      check_applyFunc ()
        {
          Tuple<Types<>>             tup0 ;
          Tuple<Types<int>>          tup1 (11);
          Tuple<Types<int,int>>      tup2 (11,12);
          Tuple<Types<int,int,int>>  tup3 (11,12,13);
          function<int()>            functor0 (fun0);
          function<int(int)>         functor1 (fun1);
          function<int(int,int)>     functor2 (fun2);
          function<int(int,int,int)> functor3 (fun3);
          
          CHECK (-1       == Apply<0>::invoke<int> (functor0, tup0) );
          CHECK (11       == Apply<1>::invoke<int> (functor1, tup1) );
          CHECK (11+12    == Apply<2>::invoke<int> (functor2, tup2) );
          CHECK (11+12+13 == Apply<3>::invoke<int> (functor3, tup3) );
          
          CHECK (-1       == TupleApplicator<int()>            (tup0) (functor0) );
          CHECK (11       == TupleApplicator<int(int)>         (tup1) (functor1) );
          CHECK (11+12    == TupleApplicator<int(int,int)>     (tup2) (functor2) );
          CHECK (11+12+13 == TupleApplicator<int(int,int,int)> (tup3) (functor3) );
          
          CHECK (-1       == apply(functor0, tup0) );
          CHECK (11       == apply(functor1, tup1) );
          CHECK (11+12    == apply(functor2, tup2) );
          CHECK (11+12+13 == apply(functor3, tup3) );
          
        }
      
      
      void
      check_bindFree ()
        {
          cout << "\t:\n\t: ---Bind----\n";
          
          Tuple<Types<>>            tup0 ;
          Tuple<Types<int>>         tup1 (11);
          Tuple<Types<int,int>>     tup2 (11,12);
          Tuple<Types<int,int,int>> tup3 (11,12,13);
          
          typedef function<int()> BoundFun;
          
          BoundFun functor0 = Apply<0>::bind<BoundFun> (fun0, tup0);
          BoundFun functor1 = Apply<1>::bind<BoundFun> (fun1, tup1);
          BoundFun functor2 = Apply<2>::bind<BoundFun> (fun2, tup3);
          BoundFun functor3 = Apply<3>::bind<BoundFun> (fun3, tup3);
          
          CHECK (-1       == functor0() );
          CHECK (11       == functor1() );
          CHECK (11+12    == functor2() );
          CHECK (11+12+13 == functor3() );
          
          functor0 = TupleApplicator<int()>            (tup0).bind (fun0);
          functor1 = TupleApplicator<int(int)>         (tup1).bind (fun1);
          functor2 = TupleApplicator<int(int,int)>     (tup2).bind (fun2);
          functor3 = TupleApplicator<int(int,int,int)> (tup3).bind (fun3);
          
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
          
          typedef function<int()> BoundFun;
          
          BoundFun functor0 = Apply<0>::bind<BoundFun> (unbound_functor0, tup0);
          BoundFun functor1 = Apply<1>::bind<BoundFun> (unbound_functor1, tup1);
          BoundFun functor2 = Apply<2>::bind<BoundFun> (unbound_functor2, tup3);
          BoundFun functor3 = Apply<3>::bind<BoundFun> (unbound_functor3, tup3);
          
          CHECK (-1       == functor0() );
          CHECK (11       == functor1() );
          CHECK (11+12    == functor2() );
          CHECK (11+12+13 == functor3() );
          
          functor0 = TupleApplicator<int()>            (tup0).bind (unbound_functor0);
          functor1 = TupleApplicator<int(int)>         (tup1).bind (unbound_functor1);
          functor2 = TupleApplicator<int(int,int)>     (tup2).bind (unbound_functor2);
          functor3 = TupleApplicator<int(int,int,int)> (tup3).bind (unbound_functor3);
          
          CHECK (-1       == functor0() );
          CHECK (11       == functor1() );
          CHECK (11+12    == functor2() );
          CHECK (11+12+13 == functor3() );
          
        }
      
      
      void
      build_closure ()
        {
          Tuple<Types<>>            tup0 ;
          Tuple<Types<int>>         tup1 (11);
          Tuple<Types<int,int>>     tup2 (11,12);
          Tuple<Types<int,int,int>> tup3 (11,12,13);
          
          FunctionClosure<int()>            clo0 (fun0,tup0);
          FunctionClosure<int(int)>         clo1 (fun1,tup1);
          FunctionClosure<int(int,int)>     clo2 (fun2,tup2);
          FunctionClosure<int(int,int,int)> clo3 (fun3,tup3);
          
          CHECK (-1       == clo0() );
          CHECK (11       == clo1() );
          CHECK (11+12    == clo2() );
          CHECK (11+12+13 == clo3() );
          
          function<int()>            unbound_functor0 (fun0);
          function<int(int)>         unbound_functor1 (fun1);
          function<int(int,int)>     unbound_functor2 (fun2);
          function<int(int,int,int)> unbound_functor3 (fun3);
          
          clo0 = FunctionClosure<int()>            (unbound_functor0,tup0);
          clo1 = FunctionClosure<int(int)>         (unbound_functor1,tup1);
          clo2 = FunctionClosure<int(int,int)>     (unbound_functor2,tup2);
          clo3 = FunctionClosure<int(int,int,int)> (unbound_functor3,tup3);
          
          CHECK (-1       == clo0() );
          CHECK (11       == clo1() );
          CHECK (11+12    == clo2() );
          CHECK (11+12+13 == clo3() );
          
          CHECK (-1       == closure(fun0,tup0) () );
          CHECK (11       == closure(fun1,tup1) () );
          CHECK (11+12    == closure(fun2,tup2) () );
          CHECK (11+12+13 == closure(fun3,tup3) () );
          
          CHECK (-1       == closure(unbound_functor0,tup0) () );
          CHECK (11       == closure(unbound_functor1,tup1) () );
          CHECK (11+12    == closure(unbound_functor2,tup2) () );
          CHECK (11+12+13 == closure(unbound_functor3,tup3) () );
          
          
          // finally combine all techniques....
          using NumberzArg = Types<List2>::Seq;
          using NumberzSig = BuildFunType<int,NumberzArg>::Sig;
          
          Tuple<NumberzArg> numberzTup (Num<5>(22), Num<6>(33), Num<7>(44));
          
          FunctionClosure<NumberzSig> numClo (getNumberz<5,6,7>, numberzTup );
          
          CHECK (22+33+44 == numClo() );
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (FunctionClosure_test, "unit common");
  
  
  
}}} // namespace lib::meta::test
