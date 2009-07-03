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
        template<uint i>
        Num<i> 
        fun11 ( Num<i>   val1
              )
          {
            return val1;
          }
        
        template<uint i, uint ii>
        Num<i> 
        fun12 ( Num<i>   val1
              , Num<ii>  val2
              )
          {
            val1.o_ += val2.o_;
            return val1;
          }
        
        template<uint i, uint ii, uint iii>
        Num<i> 
        fun13 ( Num<i>   val1
              , Num<ii>  val2
              , Num<iii> val3
              )
          {
            val1.o_ += val2.o_ + val3.o_;
            return val1;
          }
        
        template<uint i, uint ii, uint iii, uint iv>
        Num<i> 
        fun14 ( Num<i>   val1
              , Num<ii>  val2
              , Num<iii> val3
              , Num<iv>  val4
              )
          {
            val1.o_ += val2.o_ + val3.o_ + val4.o_;
            return val1;
          }
        
        template<uint i, uint ii, uint iii, uint iv, uint v>
        Num<i> 
        fun15 ( Num<i>   val1
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
   *       - chaining of two functions with suitable arguments ("composition")
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
          ASSERT (6 == (fun13<1,2,3> (_1_, _2_, _3_)).o_ );
          ASSERT (6 == (fun13<1,1,1> (Num<1>(3), Num<1>(2), Num<1>(1))).o_ );
          
          ASSERT ( 1 == fun2 (fun11<1> (_1_)) );
          ASSERT ( 3 == fun2 (fun12<1,2> (_1_, _2_)) );
          ASSERT ( 6 == fun2 (fun13<1,2,3> (_1_, _2_, _3_)) );
          ASSERT (10 == fun2 (fun14<1,2,3,4> (_1_, _2_, _3_, _4_)) );
          ASSERT (15 == fun2 (fun15<1,2,3,4,5> (_1_, _2_, _3_, _4_, _5_)) );
          
          ASSERT ( 9 == fun2 (fun13<2,3,4> (_2_, _3_, _4_)) );
          ASSERT (18 == fun2 (fun13<5,6,7> (_5_, _6_, _7_)) );
          ASSERT (24 == fun2 (fun13<9,8,7> (_9_, _8_, _7_)) );
        }
      
      
      void
      check_partialApplication ()
        {
          // Because the code of the partial function application is very technical,
          // the following might serve as explanation what actually happens....
          // (and actually it's a leftover from initial debugging)
          
          typedef Num<1> Sig123(Num<1>, Num<2>, Num<3>);             // signature of the original function
          
          typedef Num<1> Sig23(Num<2>, Num<3>);                      // signature after having closed over the first argument
          typedef function<Sig23> F23;                               // and a tr1::function object to hold such a function
          
          Sig123& f =fun13<1,2,3>;                                   // the actual input: a reference to the bare function
          
          
          // Version1: do a direct argument binding----------------- //
          
          typedef std::tr1::_Placeholder<1> PH1;                     // tr1::function argument placeholders
          typedef std::tr1::_Placeholder<2> PH2;
          
          PH1 ph1;                                                   // these empty structs are used to mark the arguments to be kept "open"
          PH2 ph2;
          Num<1> num18 (18);                                         // ...and this value is for closing the first function argument
          
          F23 fun_23 = std::tr1::bind (f, num18                      // do the actual binding (i.e. close the first argument with a constant value)
                                        , ph1
                                        , ph2
                                      );
          
          int res = 0;
          res = fun_23 (_2_,_3_).o_;                                 // and invoke the resulting functor ("closure"), providing the remaining arguments
          ASSERT (23 == res);
          
          
          
          // Version2: extract the binding arguments from a tuple--- //
          
          typedef Tuple<Types<Num<1>, PH1, PH2> > PartialArg;        // Tuple type to hold the binding values. Note the placeholder types
          PartialArg arg(num18);                                     // Value for partial application (the placeholders are default constructed)
          
          fun_23 = std::tr1::bind (f, tuple::element<0>(arg)         // now extract the values to bind from this tuple
                                    , tuple::element<1>(arg)
                                    , tuple::element<2>(arg)
                                  );
          res = 0;
          res = fun_23 (_2_,_3_).o_;                                 // and invoke the resulting functor....
          ASSERT (23 == res);
          
          
          
          
          // Version3: let the PApply-template do the work for us--- //
          
          typedef Types<Num<1> > ArgTypes;                           // now package just the argument(s) to be applied into a tuple
          Tuple<ArgTypes> args_to_bind (Num<1>(18));
          
          fun_23 = PApply<Sig123, ArgTypes>::bindFront (f , args_to_bind);
                                                                     // "bindFront" will close the parameters starting from left....
          res = 0;
          res = fun_23 (_2_,_3_).o_;                                 // invoke the resulting functor...
          ASSERT (23 == res);
          
          
          
          
          
          
          // Version4: as you'd typically do it in real life-------- //
          
          fun_23 = func::applyFirst (f, Num<1>(18));                 // use the convenience function API to close over a single value
          
          res = 0;
          res = fun_23 (_2_,_3_).o_;                                 // invoke the resulting functor...
          ASSERT (23 == res);
          
          
          
          // what follows is the real unit test...
          function<Sig123> func123 (f);                              // alternatively do it with an tr1::function object
          fun_23 = func::applyFirst (func123, Num<1>(19));
          res = fun_23 (_2_,_3_).o_;
          ASSERT (24 == res);
          
          typedef function<Num<1>(Num<1>, Num<2>)> F12;
          F12 fun_12 = func::applyLast(f, Num<3>(20));               // close the *last* argument of a function
          res = fun_12 (_1_,_2_).o_;
          ASSERT (23 == res);
          
          fun_12 = func::applyLast(func123, Num<3>(21));             // alternatively use a function object
          res = fun_12 (_1_,_2_).o_;
          ASSERT (24 == res);
          
          Sig123 *fP = &f;                                           // a function pointer works too 
          fun_12 = func::applyLast( fP, Num<3>(22));
          res = fun_12 (_1_,_2_).o_;
          ASSERT (25 == res);
                                                                     // cover more cases....
          
          ASSERT (1         == (func::applyLast (fun11<1>        , _1_) ( )              ).o_);
          ASSERT (1+3       == (func::applyLast (fun12<1,3>      , _3_) (_1_)            ).o_);
          ASSERT (1+3+5     == (func::applyLast (fun13<1,3,5>    , _5_) (_1_,_3_)        ).o_);
          ASSERT (1+3+5+7   == (func::applyLast (fun14<1,3,5,7>  , _7_) (_1_,_3_,_5_)    ).o_);
          ASSERT (1+3+5+7+9 == (func::applyLast (fun15<1,3,5,7,9>, _9_) (_1_,_3_,_5_,_7_)).o_);
          
          ASSERT (9+8+7+6+5 == (func::applyFirst(fun15<9,8,7,6,5>, _9_) (_8_,_7_,_6_,_5_)).o_);
          ASSERT (  8+7+6+5 == (func::applyFirst(  fun14<8,7,6,5>, _8_)     (_7_,_6_,_5_)).o_);
          ASSERT (    7+6+5 == (func::applyFirst(    fun13<7,6,5>, _7_)         (_6_,_5_)).o_);
          ASSERT (      6+5 == (func::applyFirst(      fun12<6,5>, _6_)             (_5_)).o_);
          ASSERT (        5 == (func::applyFirst(        fun11<5>, _5_)               ( )).o_);
          
          
          
          // Finally a more convoluted example
          // covering the general case of partial function closure:
          typedef Num<5> Sig54321(Num<5>, Num<4>, Num<3>, Num<2>, Num<1>);   // Signature of the 5-argument function
          typedef Num<5> Sig54   (Num<5>, Num<4>);                           // ...closing the last 3 arguments should yield this 2-argument function
          typedef Types<Num<3>,Num<2>,Num<1> > Args2Close;                   // Tuple type to hold the 3 argument values used for the closure
          
          // Close the trailing 3 arguments of the 5-argument function...
          function<Sig54> fun_54 = PApply<Sig54321, Args2Close>::bindBack(fun15<5,4,3,2,1>, 
                                                                          tuple::make(_3_,_2_,_1_)
                                                                         );
          
          // apply the remaining argument values
          Num<5> resN5 = fun_54 (_5_,_4_);
          ASSERT (5+4+3+2+1 == resN5.o_);
        }
      
      
      
      
      void
      check_functionalComposition ()
        {
          typedef int Sig2(Num<1>);
          typedef Num<1> Sig11(Num<1>);
          typedef Num<1> Sig12(Num<1>,Num<2>);
          typedef Num<1> Sig13(Num<1>,Num<2>,Num<3>);
          typedef Num<1> Sig14(Num<1>,Num<2>,Num<3>,Num<4>);
          typedef Num<1> Sig15(Num<1>,Num<2>,Num<3>,Num<4>,Num<5>);
          
          Sig2 & ff = fun2< Num<1> >;
          Sig11& f1 = fun11<1>;
          Sig12& f2 = fun12<1,2>;
          Sig13& f3 = fun13<1,2,3>;
          Sig14& f4 = fun14<1,2,3,4>;
          Sig15& f5 = fun15<1,2,3,4,5>;
          
          ASSERT (1         == func::chained(f1, ff) (_1_)                 );
          ASSERT (1+2       == func::chained(f2, ff) (_1_,_2_)             );
          ASSERT (1+2+3     == func::chained(f3, ff) (_1_,_2_,_3_)         );
          ASSERT (1+2+3+4   == func::chained(f4, ff) (_1_,_2_,_3_,_4_)     );
          ASSERT (1+2+3+4+5 == func::chained(f5, ff) (_1_,_2_,_3_,_4_,_5_) );
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (FunctionComposition_test, "unit common");
  
  
  
}}} // namespace lumiera::typelist::test
