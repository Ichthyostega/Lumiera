/*
  FUNCTION-CLOSURE.hpp  -  metaprogramming tools for closing a function over given arguments
 
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
 
*/


/** @file function-closure.hpp
 ** Partial function application and building a complete function closure.
 ** This is a small addendum to (and thin wrapper for) tr1/functional, supporting
 ** the case when a function should be closed over (all) arguments, where especially
 ** the parameter values to close on are provided as a tuple. 
 ** 
 ** @see control::CommandDef usage example
 ** @see function.hpp
 ** @see tuple.hpp
 ** 
 */


#ifndef LUMIERA_META_FUNCTION_CLOSURE_H
#define LUMIERA_META_FUNCTION_CLOSURE_H

#include "lib/meta/function.hpp"
#include "lib/meta/tuple.hpp"

#include <tr1/functional>



namespace lumiera {
namespace typelist{

  using std::tr1::function;
  using std::tr1::bind;
  
  
  
  namespace func { ///< helpers for binding and applying a function to an argument tuple 
    
    using tuple::element;
    
    template<typename SIG>
    struct _Fun
      {
        typedef typename FunctionSignature<function<SIG> >::Ret Ret;
        typedef typename FunctionSignature<function<SIG> >::Args Args;
      };
    template<typename SIG>
    struct _Fun<function<SIG> >
      {
        typedef typename FunctionSignature<function<SIG> >::Ret Ret;
        typedef typename FunctionSignature<function<SIG> >::Args Args;
      };
    
    template<typename FUN>
    struct is_Functor                 { static const bool value = false; };
    template<typename SIG>
    struct is_Functor<function<SIG> > { static const bool value = true;  };
    
    
    
    /**
     * this Helper with repetitive specialisations for up to nine arguments
     * is used either to apply a function to arguments given as a tuple, or
     * to create the actual closure (functor) over all function arguments. 
     */
    template<uint n>
    struct Apply;
    
    
    template<>                                            //__________________________________
    struct Apply<0>                                      ///< Apply function without Arguments
      {
        template<typename RET, class FUN, class TUP>
        static RET
        invoke (FUN& f, TUP&)
          {
            return f ();
          }
        
        template<typename RET, class FUN, class TUP>
        static RET
        bind (FUN& f, TUP&)
          {
            return std::tr1::bind (f);
          }
      };
    
    
    template<>                                            //_________________________________
    struct Apply<1>                                      ///< Apply function with 1 Argument
      {
        template<typename RET, class FUN, class TUP>
        static RET
        invoke (FUN& f, TUP & arg)
          {
            return f (element<0>(arg));
          }
        
        template<typename RET, class FUN, class TUP>
        static RET
        bind (FUN& f, TUP & arg)
          {
            return std::tr1::bind (f, element<0>(arg));
          }
      };
    
    
    template<>                                            //_________________________________
    struct Apply<2>                                      ///< Apply function with 2 Arguments
      {
        template<typename RET, class FUN, class TUP>
        static RET
        invoke (FUN& f, TUP & arg)
          {
            return f ( element<0>(arg)
                     , element<1>(arg)
                     );
          }
        
        template<typename RET, class FUN, class TUP>
        static RET
        bind (FUN& f, TUP & arg)
          {
            return std::tr1::bind (f, element<0>(arg)
                                    , element<1>(arg)
                                   );
          }
      };
    
    
    template<>                                            //_________________________________
    struct Apply<3>                                      ///< Apply function with 3 Arguments
      {
        template<typename RET, class FUN, class TUP>
        static RET
        invoke (FUN& f, TUP & arg)
          {
            return f ( element<0>(arg)
                     , element<1>(arg)
                     , element<2>(arg)
                     );
          }
        
        template<typename RET, class FUN, class TUP>
        static RET
        bind (FUN& f, TUP & arg)
          {
            return std::tr1::bind (f, element<0>(arg)
                                    , element<1>(arg)
                                    , element<2>(arg)
                                   );
          }
      };
    
    
    template<>                                            //_________________________________
    struct Apply<4>                                      ///< Apply function with 4 Arguments
      {
        template<typename RET, class FUN, class TUP>
        static RET
        invoke (FUN& f, TUP & arg)
          {
            return f ( element<0>(arg)
                     , element<1>(arg)
                     , element<2>(arg)
                     , element<3>(arg)
                     );
          }
        
        template<typename RET, class FUN, class TUP>
        static RET
        bind (FUN& f, TUP & arg)
          {
            return std::tr1::bind (f, element<0>(arg)
                                    , element<1>(arg)
                                    , element<2>(arg)
                                    , element<3>(arg)
                                   );
          }
      };
    
    
    template<>                                            //_________________________________
    struct Apply<5>                                      ///< Apply function with 5 Arguments
      {
        template<typename RET, class FUN, class TUP>
        static RET
        invoke (FUN& f, TUP & arg)
          {
            return f ( element<0>(arg)
                     , element<1>(arg)
                     , element<2>(arg)
                     , element<3>(arg)
                     , element<4>(arg)
                     );
          }
        
        template<typename RET, class FUN, class TUP>
        static RET
        bind (FUN& f, TUP & arg)
          {
            return std::tr1::bind (f, element<0>(arg)
                                    , element<1>(arg)
                                    , element<2>(arg)
                                    , element<3>(arg)
                                    , element<4>(arg)
                                   );
          }
      };
    
    
    template<>                                            //_________________________________
    struct Apply<6>                                      ///< Apply function with 6 Arguments
      {
        template<typename RET, class FUN, class TUP>
        static RET
        invoke (FUN& f, TUP & arg)
          {
            return f ( element<0>(arg)
                     , element<1>(arg)
                     , element<2>(arg)
                     , element<3>(arg)
                     , element<4>(arg)
                     , element<5>(arg)
                     );
          }
        
        template<typename RET, class FUN, class TUP>
        static RET
        bind (FUN& f, TUP & arg)
          {
            return std::tr1::bind (f, element<0>(arg)
                                    , element<1>(arg)
                                    , element<2>(arg)
                                    , element<3>(arg)
                                    , element<4>(arg)
                                    , element<5>(arg)
                                   );
          }
      };
    
    
    template<>                                            //_________________________________
    struct Apply<7>                                      ///< Apply function with 7 Arguments
      {
        template<typename RET, class FUN, class TUP>
        static RET
        invoke (FUN& f, TUP & arg)
          {
            return f ( element<0>(arg)
                     , element<1>(arg)
                     , element<2>(arg)
                     , element<3>(arg)
                     , element<4>(arg)
                     , element<5>(arg)
                     , element<6>(arg)
                     );
          }
        
        template<typename RET, class FUN, class TUP>
        static RET
        bind (FUN& f, TUP & arg)
          {
            return std::tr1::bind (f, element<0>(arg)
                                    , element<1>(arg)
                                    , element<2>(arg)
                                    , element<3>(arg)
                                    , element<4>(arg)
                                    , element<5>(arg)
                                    , element<6>(arg)
                                   );
          }
      };
    
    
    template<>                                            //_________________________________
    struct Apply<8>                                      ///< Apply function with 8 Arguments
      {
        template<typename RET, class FUN, class TUP>
        static RET
        invoke (FUN& f, TUP & arg)
          {
            return f ( element<0>(arg)
                     , element<1>(arg)
                     , element<2>(arg)
                     , element<3>(arg)
                     , element<4>(arg)
                     , element<5>(arg)
                     , element<6>(arg)
                     , element<7>(arg)
                     );
          }
        
        template<typename RET, class FUN, class TUP>
        static RET
        bind (FUN& f, TUP & arg)
          {
            return std::tr1::bind (f, element<0>(arg)
                                    , element<1>(arg)
                                    , element<2>(arg)
                                    , element<3>(arg)
                                    , element<4>(arg)
                                    , element<5>(arg)
                                    , element<6>(arg)
                                    , element<7>(arg)
                                   );
          }
      };
    
    
    template<>                                            //_________________________________
    struct Apply<9>                                      ///< Apply function with 9 Arguments
      {
        template<typename RET, class FUN, class TUP>
        static RET
        invoke (FUN& f, TUP & arg)
          {
            return f ( element<0>(arg)
                     , element<1>(arg)
                     , element<2>(arg)
                     , element<3>(arg)
                     , element<4>(arg)
                     , element<5>(arg)
                     , element<6>(arg)
                     , element<7>(arg)
                     , element<8>(arg)
                     );
          }
        
        template<typename RET, class FUN, class TUP>
        static RET
        bind (FUN& f, TUP & arg)
          {
            return std::tr1::bind (f, element<0>(arg)
                                    , element<1>(arg)
                                    , element<2>(arg)
                                    , element<3>(arg)
                                    , element<4>(arg)
                                    , element<5>(arg)
                                    , element<6>(arg)
                                    , element<7>(arg)
                                    , element<8>(arg)
                                   );
          }
      };
    
    
    /* ===== Helpers for partial function application ===== */
    
    using std::tr1::_Placeholder;     // what is the "official" way to import them?
    
    template<typename TYPES, uint i=0>
    class PlaceholderTuple
      {
//        typedef typename Tuple<TYPES>::HeadType HeadType;
//        typedef typename Tuple<TYPES>::ArgList  TypeList;
//        typedef typename Tuple<TYPES>::Tail::ArgList TailList;
        typedef typename Tuple<TYPES>::Type     TypeSeq;
        typedef typename Tuple<TYPES>::TailType TailSeq;
        
        typedef typename PlaceholderTuple<TailSeq, i+1>::PlaceholderSeq  TailPlaceholderSeq;
        
      public:
        typedef typename Prepend<_Placeholder<i>, TailPlaceholderSeq>::Seq PlaceholderSeq;
        typedef Tuple<PlaceholderSeq> Type;
        
      };
    
    template<uint i>
    struct PlaceholderTuple<Types<>, i> : PlaceholderTuple<NullType, i> {};
    
    template<uint i>
    struct PlaceholderTuple<NullType, i>
      {
        typedef Types<> PlaceholderSeq;
        typedef Tuple<Types<> > Type;
      };
    
    
    
    
    
    
  } // (END) impl-namespace (func)
  
  
  
  /**
   * Closure-creating template.
   * @note taking functor objects \em and parameters per reference
   */
  template<typename SIG>
  class TupleApplicator
    {
      typedef typename FunctionSignature< function<SIG> >::Args Args;
      typedef typename FunctionSignature< function<SIG> >::Ret  Ret;
      
      typedef function<Ret()> BoundFunc;
      
      enum { ARG_CNT = count<typename Args::List>::value };
      
      
      /** storing a ref to the parameter tuple */
      Tuple<Args>& params_;
      
    public:
      TupleApplicator (Tuple<Args>& args)
        : params_(args)
        { }
      
      BoundFunc bind (SIG& f)                 { return func::Apply<ARG_CNT>::template bind<BoundFunc> (f, params_); }
      BoundFunc bind (function<SIG> const& f) { return func::Apply<ARG_CNT>::template bind<BoundFunc> (f, params_); }
      
      Ret operator() (SIG& f)                 { return func::Apply<ARG_CNT>::template invoke<Ret> (f, params_); }
      Ret operator() (function<SIG>& f)       { return func::Apply<ARG_CNT>::template invoke<Ret> (f, params_); }
    };
  
  
  /**
   * Partial function application
   * Takes a function and a value tuple,
   * using the latter to close function arguments 
   * either from the front (left) or aligned to the end
   * of the function argument list. Result is a "reduced" function,
   * expecting only the remaining "un-closed" arguments at invocation.  
   */
  template<typename SIG, typename VAL>
  class PApply
    {
      typedef typename func::_Fun<SIG>::Args Args;
      typedef typename func::_Fun<SIG>::Ret  Ret;
      
      enum { ARG_CNT = count<typename Args::List>::value
           , VAL_CNT = count<typename VAL::List>::value
           , ROFFSET = (VAL_CNT < ARG_CNT)? ARG_CNT-VAL_CNT : 0
           };
      
      typedef typename func::PlaceholderTuple<Args>::PlaceholderSeq::List Placeholders;
      typedef typename Args::List ArgsList;
      typedef typename VAL::List ValList;
      
      typedef typename Splice<Placeholders, ValList>::List           LeftReplaced;
      typedef typename Splice<Placeholders, ValList, ROFFSET>::List  RightReplaced;
      
      typedef typename Splice<ArgsList, ValList>::Back           LeftReduced;
      typedef typename Splice<ArgsList, ValList, ROFFSET>::Front RightReduced;
      
      typedef Tuple<LeftReplaced>  TupleL;
      typedef Tuple<RightReplaced> TupleR;
      
      typedef typename Tuple<LeftReduced>::Type  ArgsL;
      typedef typename Tuple<RightReduced>::Type ArgsR;
      
      typedef tuple::BuildTuple<LeftReplaced, ValList>           BuildL;
      typedef tuple::BuildTuple<RightReplaced, ValList, ROFFSET> BuildR;
      
      
    public:
      typedef function<typename FunctionTypedef<Ret,ArgsL>::Sig> LeftReducedFunc;
      typedef function<typename FunctionTypedef<Ret,ArgsR>::Sig> RightReducedFunc;
      
      
      /** Contains the argument values, starting from left.
       *  Any remaining positions are occupied by binding placeholders */
      struct LeftReplacedArgs
        : TupleL
        {
          LeftReplacedArgs (Tuple<VAL> const& arg)
            : TupleL ( BuildL::create(arg))
            { }
        };
      
      /** Contains the argument values, aligned to the end of the function argument list.
       *  Any remaining positions before are occupied by binding placeholders */
      struct RightReplacedArgs
        : TupleR
        {
          RightReplacedArgs (Tuple<VAL> const& arg)
            : TupleR ( BuildR::create(arg))
            { }
        };
        
      /** do a partial function application, closing the first arguments
       *  f(a,b,c)->res  +  (a,b)  yields  f(c)->res
       *  
       *  @param f   function, function pointer or functor
       *  @param arg value tuple, used to close function arguments starting from left
       *  @return new function object, holding copies of the values and using them at the
       *          closed arguments; on invocation, only the remaining arguments need to be supplied.
       */
      static LeftReducedFunc
      bindFront (SIG& f, Tuple<VAL> const& arg)
        {
          LeftReplacedArgs params (arg);
          return func::Apply<ARG_CNT>::template bind<LeftReducedFunc> (f, params.tupleCast());
        }
      
      /** do a partial function application, closing the last arguments
       *  f(a,b,c)->res  +  (b,c)  yields  f(a)->res
       *  
       *  @param f   function, function pointer or functor
       *  @param arg value tuple, used to close function arguments starting from right
       *  @return new function object, holding copies of the values and using them at the
       *          closed arguments; on invocation, only the remaining arguments need to be supplied.
       */
      static RightReducedFunc
      bindBack (SIG& f, Tuple<VAL> const& arg)
        {
          RightReplacedArgs params (arg);
          return func::Apply<ARG_CNT>::template bind<RightReducedFunc> (f, params.tupleCast());
        }
    };
  
  
  
  /**
   * Closing a function over its arguments.
   * This is a small usage example or spin-off,
   * having almost the same effect than invoking tr1::bind.
   * The notable difference is that the function arguments for
   * creating the closure are passed in as one compound tuple.
   */
  template<typename SIG>
  class FunctionClosure
    {
      typedef typename func::_Fun<SIG>::Args Args;
      typedef typename func::_Fun<SIG>::Ret  Ret;
      
      function<Ret(void)> closure_;
      
    public:
      FunctionClosure (SIG& f, Tuple<Args>& arg)
        : closure_(TupleApplicator<SIG>(arg).bind(f))
        { }
      FunctionClosure (function<SIG> const& f, Tuple<Args>& arg)
        : closure_(TupleApplicator<SIG>(arg).bind(f))
        { }
      
      Ret operator() () { return closure_(); }
      
      typedef Ret  result_type;  ///< for STL use
      typedef void argument_type;
    };
  
  
  
  namespace func { // ...some convenience shortcuts
    
    
    // helpers for specifying types in function declarations....
  
    template<typename RET, typename ARG>
    struct _Sig
      {
        typedef typename FunctionTypedef<RET, ARG>::Sig Type;
        typedef TupleApplicator<Type> Applicator;
      };
    
    template<typename SIG, typename ARG>
    struct _Clo
      {
        typedef typename _Fun<SIG>::Ret Ret;
        typedef typename _Sig<Ret,ARG>::Type Signature;
        typedef FunctionClosure<Signature> Type;
      };
    
    template<typename SIG>
    struct _PapS
      {
        typedef typename _Fun<SIG>::Ret Ret;
        typedef typename _Fun<SIG>::Args Args;
        typedef typename Split<Args>::Head Arg;
        typedef typename Split<Args>::Tail Rest;
        typedef typename _Sig<Ret,Rest>::Type Signature;
        typedef function<Signature> Function;
      };
    
    template<typename SIG>
    struct _PapE
      {
        typedef typename _Fun<SIG>::Ret Ret;
        typedef typename _Fun<SIG>::Args Args;
        typedef typename Split<Args>::End Arg;
        typedef typename Split<Args>::Prefix Rest;
        typedef typename _Sig<Ret,Rest>::Type Signature;
        typedef function<Signature> Function;
      };
    
    
    
    
    /*  ========== function-style interface =============  */
    
    /** build a TupleApplicator, which embodies the given
     *  argument tuple and can be used to apply various
     *  functions to them.
     */
    template<typename ARG>
    typename _Sig<void, ARG>::Applicator
    tupleApplicator (Tuple<ARG>& args)
    {
      typedef typename _Sig<void,ARG>::Type Signature;
      return TupleApplicator<Signature> (args);
    }
    
    
    /** apply the given function to the argument tuple */
    template<typename SIG, typename ARG>
    typename _Fun<SIG>::Ret
    apply (SIG& f, Tuple<ARG>& args)
    {
      typedef typename _Fun<SIG>::Ret Ret;                //
      typedef typename _Sig<Ret,ARG>::Type Signature;    // Note: deliberately re-building the Signature Type
      return TupleApplicator<Signature> (args) (f);     //        in order to get better error messages here
    }
    
    /** close the given function over all arguments,
     *  using the values from the argument tuple.
     *  @return a closure object, which can be 
     *          invoked later to yield the
     *          function result. */
    template<typename SIG, typename ARG>
    typename _Clo<SIG,ARG>::Type
    closure (SIG& f, Tuple<ARG>& args)
    {
      typedef typename _Fun<SIG>::Ret Ret;
      typedef typename _Sig<Ret,ARG>::Type Signature;
      typedef typename _Clo<SIG,ARG>::Type Closure;
      return Closure (f,args);
    }
    
    
    /** close the given function over the first argument */
    template<typename SIG, typename ARG>
    typename _PapS<SIG>::Function
    applyFirst (SIG& f, ARG arg)
    {
      typedef typename _PapS<SIG>::Arg ArgType;
      typedef Types<ArgType>           ArgTypeSeq;
      typedef Tuple<ArgTypeSeq>        ArgTuple;
      ArgTuple val(arg);
      return PApply<SIG,ArgTypeSeq>::bindFront (f, val);
    }
    
    /** close the given function over the last argument */
    template<typename SIG, typename ARG>
    typename _PapE<SIG>::Function
    applyLast (SIG& f, ARG arg)
    {
      typedef typename _PapE<SIG>::Arg ArgType;
      typedef Types<ArgType>           ArgTypeSeq;
      typedef Tuple<ArgTypeSeq>        ArgTuple;
      ArgTuple val(arg);
      return PApply<SIG,ArgTypeSeq>::bindBack (f, val);
    }
    
  } // (END) namespace func
  
  
  
}} // namespace lumiera::typelist
#endif
