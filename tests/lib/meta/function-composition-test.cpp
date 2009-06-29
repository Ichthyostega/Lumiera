/*
  FunctionComposition(Test)  -  functional composition and partial application
 
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


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/meta/typelist.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/function-closure.hpp"
#include "meta/typelist-diagnostics.hpp"

#include <iostream>

using ::test::Test;
using std::string;
using std::cout;
using std::endl;


namespace lumiera {
namespace typelist{
namespace test {
  
  using func::applyFirst;
  using func::applyLast;
  
  
      namespace { // test functions
        
        
        
        typedef Types< Num<1>  ////////////////////////TODO kill kill kill
                     , Num<2>
                     , Num<3>
                     >::List List1;
        typedef Types< Num<5>
                     , Num<6>
                     , Num<7>
                     >::List List2;
        
        Num<1> _1_;                     
        Num<2> _2_;                     
        Num<3> _3_;                     
        Num<4> _4_;                     
        Num<5> _5_;                     
        Num<6> _6_;                     
        Num<7> _7_;                     
        Num<8> _8_;                     
        Num<9> _9_;                     
        
        /** "Function-1" will be used at the front side, accepting a tuple of values */
        template<char i>
        Num<i> 
        fun1 ( Num<i>   val1
             )
          {
            return val1;
          }
        
        template<char i, char ii>
        Num<i> 
        fun1 ( Num<i>   val1
             , Num<ii>  val2
             )
          {
            val1.o_ += val2.o_;
            return val1;
          }
        
        template<char i, char ii, char iii>
        Num<i> 
        fun1 ( Num<i>   val1
             , Num<ii>  val2
             , Num<iii> val3
             )
          {
            val1.o_ += val2.o_ + val3.o_;
            return val1;
          }
        
        template<char i, char ii, char iii, char iv>
        Num<i> 
        fun1 ( Num<i>   val1
             , Num<ii>  val2
             , Num<iii> val3
             , Num<iv>  val4
             )
          {
            val1.o_ += val2.o_ + val3.o_ + val4.o_;
            return val1;
          }
        
        template<char i, char ii, char iii, char iv, char v>
        Num<i> 
        fun1 ( Num<i>   val1
             , Num<ii>  val2
             , Num<iii> val3
             , Num<iv>  val4
             , Num<v>   val5
             )
          {
            val1.o_ += val2.o_ + val3.o_ + val4.o_ + val5.o_;
            return val1;
          }
        
        
        /** "Function-2" can be chained behind fun1 */
        template<class II>
        int
        fun2 (II val)
          {
            return val.o_;
          }
        
      } // (End) test data
  
  
  
  
  
  
  /******************************************************************************
   * @test this test covers some extensions and variations on function closures:
   *       - partial application of a function, returning a binder
   *       - chaining of two functions with suitable arguemnts ("composition")
   */
  class FunctionComposition_test : public Test
    {
      virtual void
      run (Arg) 
        {
          check_diagnostics ();
          check_partialApplication ();
          check_functionalComposition ();
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
          cout << "\t:\n\t: ---Apply---\n";
          
          Tuple<Types<> >            tup0 ;
          Tuple<Types<int> >         tup1 (11);
          Tuple<Types<int,int> >     tup2 (11,12);
          Tuple<Types<int,int,int> > tup3 (11,12,13);
          DUMPVAL (tup0);
          DUMPVAL (tup1);
          DUMPVAL (tup2);
          DUMPVAL (tup3);
          
          ASSERT (-1       == func::Apply<0>::invoke<int> (fun0, tup0) );
          ASSERT (11       == func::Apply<1>::invoke<int> (fun1, tup1) );
          ASSERT (11+12    == func::Apply<2>::invoke<int> (fun2, tup2) );
          ASSERT (11+12+13 == func::Apply<3>::invoke<int> (fun3, tup3) );
          
          ASSERT (-1       == TupleApplicator<int()>            (tup0) (fun0) );
          ASSERT (11       == TupleApplicator<int(int)>         (tup1) (fun1) );
          ASSERT (11+12    == TupleApplicator<int(int,int)>     (tup2) (fun2) );
          ASSERT (11+12+13 == TupleApplicator<int(int,int,int)> (tup3) (fun3) );
          
          ASSERT (-1       == func::apply(fun0, tup0) );
          ASSERT (11       == func::apply(fun1, tup1) );
          ASSERT (11+12    == func::apply(fun2, tup2) );
          ASSERT (11+12+13 == func::apply(fun3, tup3) );
        
        }
      
      
      void
      check_applyFunc ()
        {
          Tuple<Types<> >            tup0 ;
          Tuple<Types<int> >         tup1 (11);
          Tuple<Types<int,int> >     tup2 (11,12);
          Tuple<Types<int,int,int> > tup3 (11,12,13);
          function<int()>            functor0 (fun0);
          function<int(int)>         functor1 (fun1);
          function<int(int,int)>     functor2 (fun2);
          function<int(int,int,int)> functor3 (fun3);
          
          ASSERT (-1       == func::Apply<0>::invoke<int> (functor0, tup0) );
          ASSERT (11       == func::Apply<1>::invoke<int> (functor1, tup1) );
          ASSERT (11+12    == func::Apply<2>::invoke<int> (functor2, tup2) );
          ASSERT (11+12+13 == func::Apply<3>::invoke<int> (functor3, tup3) );
          
          ASSERT (-1       == TupleApplicator<int()>            (tup0) (functor0) );
          ASSERT (11       == TupleApplicator<int(int)>         (tup1) (functor1) );
          ASSERT (11+12    == TupleApplicator<int(int,int)>     (tup2) (functor2) );
          ASSERT (11+12+13 == TupleApplicator<int(int,int,int)> (tup3) (functor3) );
          
          ASSERT (-1       == func::apply(functor0, tup0) );
          ASSERT (11       == func::apply(functor1, tup1) );
          ASSERT (11+12    == func::apply(functor2, tup2) );
          ASSERT (11+12+13 == func::apply(functor3, tup3) );
          
        }
      
      
      void
      check_bindFree ()
        {
          cout << "\t:\n\t: ---Bind----\n";
          
          Tuple<Types<> >            tup0 ;
          Tuple<Types<int> >         tup1 (11);
          Tuple<Types<int,int> >     tup2 (11,12);
          Tuple<Types<int,int,int> > tup3 (11,12,13);
          
          typedef function<int()> BoundFun;
          
          BoundFun functor0 = func::Apply<0>::bind<BoundFun> (fun0, tup0);
          BoundFun functor1 = func::Apply<1>::bind<BoundFun> (fun1, tup1);
          BoundFun functor2 = func::Apply<2>::bind<BoundFun> (fun2, tup3);
          BoundFun functor3 = func::Apply<3>::bind<BoundFun> (fun3, tup3);
          
          ASSERT (-1       == functor0() );
          ASSERT (11       == functor1() );
          ASSERT (11+12    == functor2() );
          ASSERT (11+12+13 == functor3() );
          
          functor0 = TupleApplicator<int()>            (tup0).bind (fun0);
          functor1 = TupleApplicator<int(int)>         (tup1).bind (fun1);
          functor2 = TupleApplicator<int(int,int)>     (tup2).bind (fun2);
          functor3 = TupleApplicator<int(int,int,int)> (tup3).bind (fun3);
          
          ASSERT (-1       == functor0() );
          ASSERT (11       == functor1() );
          ASSERT (11+12    == functor2() );
          ASSERT (11+12+13 == functor3() );
          
        }
      
      
      void
      check_bindFunc ()
        {
          Tuple<Types<> >            tup0 ;
          Tuple<Types<int> >         tup1 (11);
          Tuple<Types<int,int> >     tup2 (11,12);
          Tuple<Types<int,int,int> > tup3 (11,12,13);
          function<int()>            unbound_functor0 (fun0);
          function<int(int)>         unbound_functor1 (fun1);
          function<int(int,int)>     unbound_functor2 (fun2);
          function<int(int,int,int)> unbound_functor3 (fun3);
          
          typedef function<int()> BoundFun;
          
          BoundFun functor0 = func::Apply<0>::bind<BoundFun> (unbound_functor0, tup0);
          BoundFun functor1 = func::Apply<1>::bind<BoundFun> (unbound_functor1, tup1);
          BoundFun functor2 = func::Apply<2>::bind<BoundFun> (unbound_functor2, tup3);
          BoundFun functor3 = func::Apply<3>::bind<BoundFun> (unbound_functor3, tup3);
          
          ASSERT (-1       == functor0() );
          ASSERT (11       == functor1() );
          ASSERT (11+12    == functor2() );
          ASSERT (11+12+13 == functor3() );
          
          functor0 = TupleApplicator<int()>            (tup0).bind (unbound_functor0);
          functor1 = TupleApplicator<int(int)>         (tup1).bind (unbound_functor1);
          functor2 = TupleApplicator<int(int,int)>     (tup2).bind (unbound_functor2);
          functor3 = TupleApplicator<int(int,int,int)> (tup3).bind (unbound_functor3);
          
          ASSERT (-1       == functor0() );
          ASSERT (11       == functor1() );
          ASSERT (11+12    == functor2() );
          ASSERT (11+12+13 == functor3() );
          
        }
      
      
      void
      build_closure ()
        {
          Tuple<Types<> >            tup0 ;
          Tuple<Types<int> >         tup1 (11);
          Tuple<Types<int,int> >     tup2 (11,12);
          Tuple<Types<int,int,int> > tup3 (11,12,13);
          
          FunctionClosure<int()>            clo0 (fun0,tup0);
          FunctionClosure<int(int)>         clo1 (fun1,tup1);
          FunctionClosure<int(int,int)>     clo2 (fun2,tup2);
          FunctionClosure<int(int,int,int)> clo3 (fun3,tup3);
          
          ASSERT (-1       == clo0() );
          ASSERT (11       == clo1() );
          ASSERT (11+12    == clo2() );
          ASSERT (11+12+13 == clo3() );
          
          function<int()>            unbound_functor0 (fun0);
          function<int(int)>         unbound_functor1 (fun1);
          function<int(int,int)>     unbound_functor2 (fun2);
          function<int(int,int,int)> unbound_functor3 (fun3);
          
          clo0 = FunctionClosure<int()>            (unbound_functor0,tup0);
          clo1 = FunctionClosure<int(int)>         (unbound_functor1,tup1);
          clo2 = FunctionClosure<int(int,int)>     (unbound_functor2,tup2);
          clo3 = FunctionClosure<int(int,int,int)> (unbound_functor3,tup3);
          
          ASSERT (-1       == clo0() );
          ASSERT (11       == clo1() );
          ASSERT (11+12    == clo2() );
          ASSERT (11+12+13 == clo3() );
          
          ASSERT (-1       == func::closure(fun0,tup0) () );
          ASSERT (11       == func::closure(fun1,tup1) () );
          ASSERT (11+12    == func::closure(fun2,tup2) () );
          ASSERT (11+12+13 == func::closure(fun3,tup3) () );
          
          ASSERT (-1       == func::closure(unbound_functor0,tup0) () );
          ASSERT (11       == func::closure(unbound_functor1,tup1) () );
          ASSERT (11+12    == func::closure(unbound_functor2,tup2) () );
          ASSERT (11+12+13 == func::closure(unbound_functor3,tup3) () );
          
          
          // finally combine all techniques....
          typedef Tuple<List2>::Type NumberzArg;
          typedef FunctionTypedef<int,NumberzArg>::Sig NumberzSig;
          Tuple<NumberzArg> numberzTup (Num<5>(22), Num<6>(33), Num<7>(44));
          
          FunctionClosure<NumberzSig> numClo (getNumberz<5,6,7>, numberzTup );
          
          ASSERT (22+33+44 == numClo() );
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (FunctionClosure_test, "unit common");
  
  
  
}}} // namespace lumiera::typelist::test
