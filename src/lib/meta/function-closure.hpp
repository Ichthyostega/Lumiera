/*
  FUNCTION-CLOSURE.hpp  -  metaprogramming tools for closing a function over given arguments

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file function-closure.hpp
 ** Partial function application and building a complete function closure.
 ** This is a addendum to std::bind, to support especially the case when a
 ** function should be _closed_ over (partially or all) arguments. This implies
 ** to bind some arguments immediately, while keeping other arguments open to
 ** be supplied on function invocation.
 ** Additionally, we allow for composing (chaining) of two functions.
 ** @warning this header is in a state of transition as of 2/2025, because functionality
 **       of this kind will certainly needed in future, but with full support for lambdas,
 **       move-only types and perfect forwarding. A gradual rework has been started, and
 **       will lead to a complete rewrite of the core functionality eventually, making
 **       better use of variadic templates and library functions like std::apply, which
 **       were not available at the time of the first implementation.
 ** 
 ** @todo the implementation is able to handle partial application with N arguments,
 **       but currently we need just one argument, thus only this case was wrapped
 **       up into a convenient functions func::applyFirst and func::applyLast
 ** @todo 11/23 these functor-utils were written at a time when support for handling
 **       generic functions in C++ was woefully inadequate; at that time, we neither
 **       had Lambda-support in the language, nor the ability to use variadic arguments.
 **       Providing a one-shot function-style interface for this kind of manipulations
 **       is still considered beneficial, and thus we should gradually modernise
 **       the tools we want to retain...
 ** @todo 2/25 note that there is a bind_front in C++20 and C++23 will provide a bind_back
 **       helper, which would provide the implementation fully in accordance with current
 **       expectations (including move, constexpr); if we choose to retain a generic
 **       function-style front-end, it should be aligned with these standard facilities.
 **       We might want to retain a simple generic interface especially for binding some
 **       selected argument, which handles the intricacies of storing the functor.
 ** 
 ** @see control::CommandDef usage example
 ** @see function-closure-test.hpp
 ** @see function-composition-test.hpp
 ** @see function.hpp
 ** 
 */


#ifndef LIB_META_FUNCTION_CLOSURE_H
#define LIB_META_FUNCTION_CLOSURE_H

#include "lib/meta/function.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/util.hpp"

#include <functional>
#include <utility>
#include <tuple>



namespace lib {
namespace meta{
namespace func{
  
  using std::tuple;
  using std::function;
  using std::forward;
  using std::move;
  
  
  
  
  namespace { // helpers for binding and applying a function to an argument tuple
    
    using std::get;
    
    /**
     * this Helper with repetitive specialisations for up to nine arguments
     * is used either to apply a function to arguments given as a tuple, or
     * to create the actual closure (functor) over all function arguments.
     * @todo 2/2025 should be replaced by a single variadic template, and
     *       implemented using std::apply. Note also that std::bind would
     *       support perfect-forwarding, especially also for the functor;
     *       the latter would allow to use move-only functors.
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
            return std::bind (f);
          }
      };
    
    
    template<>                                            //_________________________________
    struct Apply<1>                                      ///< Apply function with 1 Argument
      {
        template<typename RET, class FUN, class TUP>
        static RET
        invoke (FUN& f, TUP & arg)
          {
            return f (get<0>(arg));
          }
        
        template<typename RET, class FUN, class TUP>
        static RET
        bind (FUN& f, TUP & arg)
          {
            return std::bind (f, get<0>(arg));
          }
      };
    
    
    template<>                                            //_________________________________
    struct Apply<2>                                      ///< Apply function with 2 Arguments
      {
        template<typename RET, class FUN, class TUP>
        static RET
        invoke (FUN& f, TUP & arg)
          {
            return f ( get<0>(arg)
                     , get<1>(arg)
                     );
          }
        
        template<typename RET, class FUN, class TUP>
        static RET
        bind (FUN& f, TUP & arg)
          {
            return std::bind (f, get<0>(arg)
                               , get<1>(arg)
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
            return f ( get<0>(arg)
                     , get<1>(arg)
                     , get<2>(arg)
                     );
          }
        
        template<typename RET, class FUN, class TUP>
        static RET
        bind (FUN& f, TUP & arg)
          {
            return std::bind (f, get<0>(arg)
                               , get<1>(arg)
                               , get<2>(arg)
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
            return f ( get<0>(arg)
                     , get<1>(arg)
                     , get<2>(arg)
                     , get<3>(arg)
                     );
          }
        
        template<typename RET, class FUN, class TUP>
        static RET
        bind (FUN& f, TUP & arg)
          {
            return std::bind (f, get<0>(arg)
                               , get<1>(arg)
                               , get<2>(arg)
                               , get<3>(arg)
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
            return f ( get<0>(arg)
                     , get<1>(arg)
                     , get<2>(arg)
                     , get<3>(arg)
                     , get<4>(arg)
                     );
          }
        
        template<typename RET, class FUN, class TUP>
        static RET
        bind (FUN& f, TUP & arg)
          {
            return std::bind (f, get<0>(arg)
                               , get<1>(arg)
                               , get<2>(arg)
                               , get<3>(arg)
                               , get<4>(arg)
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
            return f ( get<0>(arg)
                     , get<1>(arg)
                     , get<2>(arg)
                     , get<3>(arg)
                     , get<4>(arg)
                     , get<5>(arg)
                     );
          }
        
        template<typename RET, class FUN, class TUP>
        static RET
        bind (FUN& f, TUP & arg)
          {
            return std::bind (f, get<0>(arg)
                               , get<1>(arg)
                               , get<2>(arg)
                               , get<3>(arg)
                               , get<4>(arg)
                               , get<5>(arg)
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
            return f ( get<0>(arg)
                     , get<1>(arg)
                     , get<2>(arg)
                     , get<3>(arg)
                     , get<4>(arg)
                     , get<5>(arg)
                     , get<6>(arg)
                     );
          }
        
        template<typename RET, class FUN, class TUP>
        static RET
        bind (FUN& f, TUP & arg)
          {
            return std::bind (f, get<0>(arg)
                               , get<1>(arg)
                               , get<2>(arg)
                               , get<3>(arg)
                               , get<4>(arg)
                               , get<5>(arg)
                               , get<6>(arg)
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
            return f ( get<0>(arg)
                     , get<1>(arg)
                     , get<2>(arg)
                     , get<3>(arg)
                     , get<4>(arg)
                     , get<5>(arg)
                     , get<6>(arg)
                     , get<7>(arg)
                     );
          }
        
        template<typename RET, class FUN, class TUP>
        static RET
        bind (FUN& f, TUP & arg)
          {
            return std::bind (f, get<0>(arg)
                               , get<1>(arg)
                               , get<2>(arg)
                               , get<3>(arg)
                               , get<4>(arg)
                               , get<5>(arg)
                               , get<6>(arg)
                               , get<7>(arg)
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
            return f ( get<0>(arg)
                     , get<1>(arg)
                     , get<2>(arg)
                     , get<3>(arg)
                     , get<4>(arg)
                     , get<5>(arg)
                     , get<6>(arg)
                     , get<7>(arg)
                     , get<8>(arg)
                     );
          }
        
        template<typename RET, class FUN, class TUP>
        static RET
        bind (FUN& f, TUP & arg)
          {
            return std::bind (f, get<0>(arg)
                               , get<1>(arg)
                               , get<2>(arg)
                               , get<3>(arg)
                               , get<4>(arg)
                               , get<5>(arg)
                               , get<6>(arg)
                               , get<7>(arg)
                               , get<8>(arg)
                              );
          }
      };
    
    
    
    
    
    
    /* ===== Helpers for partial function application ===== */
    
    /** @note relying on the implementation type
     *        since we need to _build_ placeholders */
    using std::_Placeholder;
    
    
    /**
     *  Build a list of standard function argument placeholder types.
     *  For each of the elements of the provided reference list,
     *  a Placeholder is added, numbers counting up starting with 1 (!)
     */
    template<typename TYPES, size_t i=1>
    struct PlaceholderTuple
      : PlaceholderTuple<typename TYPES::List>
      { };
    
    template<typename X, typename TAIL, size_t i>
    struct PlaceholderTuple<Node<X,TAIL>, i>
      {
        using TailPlaceholders = typename PlaceholderTuple<TAIL,i+1>::List;
        
        using List = Node<_Placeholder<i>, TailPlaceholders>;
      };
    
    template<size_t i>
    struct PlaceholderTuple<NullType, i>
      {
        using List = NullType;
      };
    
    
    
    using std::tuple_element;
    using std::tuple_size;
    using std::get;
    
    
    /**
     * Builder for a tuple instance, where only some ctor parameters are supplied,
     * while the remaining arguments will be default constructed. The use case is
     * creating of a function binder, where some arguments shall be passed through
     * (and thus be stored in the resulting closure), while other arguments are just
     * marked as "Placeholder" with `std::_Placeholder<i>`.
     * These placeholder marker terms just need to be default constructed, and will
     * then be stored into the desired positions. Later on, when actually invoking
     * such a partially closed function, only the arguments marked with placeholders
     * need to be supplied, while the other arguments will use the values hereby
     * "baked" into the closure.
     * @tparam TAR full target tuple type. Some oft the elements within this tuple will
     *             be default constructed, some will be initialised from the SRC tuple
     * @tparam SRC argument tuple type, for the values _actually to be initialised_ here.
     * @tparam start position within \a SRC, at which the sequence of init-arguments starts;
     *             all other positions will just be default initialised
     * @see lib::meta::TupleConstructor
     */
    template<typename SRC, typename TAR, size_t start>
    struct PartiallyInitTuple
      {
        template<size_t i>
        using DestType = typename std::tuple_element<i, TAR>::type;
        
        
        /**
         * define those index positions in the target tuple,
         * where init arguments shall be used on construction.
         * All other arguments will just be default initialised.
         */
        static constexpr bool
        useArg (size_t idx)
          {
            return (start <= idx)
               and (idx < start + std::tuple_size<SRC>());
          }
        
        
        
        template<size_t idx,   bool doPick = PartiallyInitTuple::useArg(idx)>
        struct IndexMapper
          {
            SRC const& initArgs;
            
            operator DestType<idx>()
              {
                return std::get<idx-start> (initArgs);
              }
          };
        
        template<size_t idx>
        struct IndexMapper<idx, false>
          {
            SRC const& initArgs;
            
            operator DestType<idx>()
              {
                return DestType<idx>();
              }
          };
      };
    
  } // (END) impl-namespace
  
  
  
  
  /* ======= core operations: closures and partial application ========= */
  
  /**
   * Closure-creating template.
   * The instance is linked (reference) to a given concrete argument tuple.
   * A functor with a matching signature may then either be _closed_ over
   * this argument values, or even be invoked right away with theses arguments.
   * @warning we take functor objects _and parameters_ by reference
   */
  template<typename SIG>
  class TupleApplicator
    {
      using Args = typename _Fun<SIG>::Args;
      using Ret  = typename _Fun<SIG>::Ret;
      
      using BoundFunc = function<Ret()>;
      
      enum { ARG_CNT = count<typename Args::List>::value };
      
      
      /** storing a ref to the parameter tuple */
      Tuple<Args>& params_;
      
    public:
      TupleApplicator (Tuple<Args>& args)
        : params_(args)
        { }
      
      BoundFunc bind (SIG& f)                 { return Apply<ARG_CNT>::template bind<BoundFunc> (f, params_); }
      BoundFunc bind (function<SIG> const& f) { return Apply<ARG_CNT>::template bind<BoundFunc> (f, params_); }
      
      Ret operator() (SIG& f)                 { return Apply<ARG_CNT>::template invoke<Ret> (f, params_); }
      Ret operator() (function<SIG>& f)       { return Apply<ARG_CNT>::template invoke<Ret> (f, params_); }
    };
  
  
  
  /**
   * Closing a function over its arguments.
   * This is a small usage example or spin-off,
   * having almost the same effect than invoking `std::bind()`.
   * The notable difference is that the function arguments for
   * creating the closure are passed in as one tuple compound.
   */
  template<typename SIG>
  class FunctionClosure
    {
      typedef typename _Fun<SIG>::Args Args;
      typedef typename _Fun<SIG>::Ret  Ret;
      
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
  
  
  
  
  /**
   * Partial function application
   * Takes a function and a value tuple,
   * using the latter to close function arguments
   * either from the front (left) or aligned to the end
   * of the function argument list. Result is a "reduced" function,
   * expecting only the remaining "un-closed" arguments at invocation.
   * @tparam SIG signature of the function to be closed, either as
   *             function reference type or `std::function` object
   * @tparam VAL type sequence describing the tuple of values
   *             used for closing arguments
   * @note the construction of this helper template does not verify or
   *       match types to the signature. In case of mismatch, you'll get
   *       a compilation failure from `std::bind` (which can be confusing)
   * @todo 11/2023 started with modernising these functor utils.
   *       The most relevant bindFirst() / bindLast() operations do no longer
   *       rely on the PApply template.  There is however the more general case
   *       of _binding multiple arguments,_ which is still used at a few places.
   *       Possibly PApply should be rewritten from scratch, using modern tooling.
   */
  template<typename SIG, typename VAL>
  class PApply
    {
      using Args     = typename _Fun<SIG>::Args;
      using Ret      = typename _Fun<SIG>::Ret;
      using ArgsList = typename Args::List;
      using ValList  = typename VAL::List;
      using ValTypes = typename Types<ValList>::Seq;
      
      enum { ARG_CNT = count<ArgsList>::value
           , VAL_CNT = count<ValList> ::value
           , ROFFSET = (VAL_CNT < ARG_CNT)? ARG_CNT-VAL_CNT : 0
           };
      
      
      // create list of the *remaining* arguments, after applying the ValList
      using LeftReduced = typename Splice<ArgsList, ValList>::Back;
      using RightReduced = typename Splice<ArgsList, ValList, ROFFSET>::Front;
      
      using ArgsL = typename Types<LeftReduced>::Seq;
      using ArgsR = typename Types<RightReduced>::Seq;
      
      
      // build a list, where each of the *remaining* arguments is replaced by a placeholder marker
      using TrailingPlaceholders = typename func::PlaceholderTuple<LeftReduced>::List;
      using LeadingPlaceholders  = typename func::PlaceholderTuple<RightReduced>::List;
      
      // ... and splice these placeholders on top of the original argument type list,
      // thus retaining the types to be closed, but setting a placeholder for each remaining argument
      using LeftReplaced  = typename Splice<ArgsList, TrailingPlaceholders, VAL_CNT>::List;
      using RightReplaced = typename Splice<ArgsList, LeadingPlaceholders,  0      >::List;
      
      using LeftReplacedTypes = typename Types<LeftReplaced>::Seq;
      using RightReplacedTypes = typename Types<RightReplaced>::Seq;
      
      // create a "builder" helper, which accepts exactly the value tuple elements
      // and puts them at the right location, while default-constructing the remaining
      // (=placeholder)-arguments. Using this builder helper, we can finally set up
      // the argument tuples (Left/RightReplacedArgs) used for the std::bind call
      template<class SRC, class TAR, size_t i>
      using IdxSelectorL = typename PartiallyInitTuple<SRC, TAR, 0>::template IndexMapper<i>;
      
      template<class SRC, class TAR, size_t i>
      using IdxSelectorR = typename PartiallyInitTuple<SRC, TAR, ROFFSET>::template IndexMapper<i>;
      
      using BuildL = TupleConstructor<LeftReplacedTypes, IdxSelectorL>;
      using BuildR = TupleConstructor<RightReplacedTypes, IdxSelectorR>;
      
      
      /** Tuple to hold all argument values, starting from left.
       *  Any remaining positions behind the substitute values are occupied by binding placeholders */
      using LeftReplacedArgs  = Tuple<LeftReplacedTypes>;
      
      /** Tuple to hold all argument values, aligned to the end of the function argument list.
       *  Any remaining positions before the substitute values are occupied by binding placeholders */
      using RightReplacedArgs = Tuple<RightReplacedTypes>;
      
      
    public:
      using LeftReducedFunc  = function<typename BuildFunType<Ret,ArgsL>::Sig>;
      using RightReducedFunc = function<typename BuildFunType<Ret,ArgsR>::Sig>;
      
      
      /** do a partial function application, closing the first arguments<br/>
       *  `f(a,b,c)->res  +  (a,b)`  yields  `f(c)->res`
       *  
       *  @param f   function, function pointer or functor
       *  @param arg value tuple, used to close function arguments starting from left
       *  @return new function object, holding copies of the values and using them at the
       *          closed arguments; on invocation, only the remaining arguments need to be supplied.
       *  @note   BuildL, i.e. the TupleApplicator _must take its arguments by-value._ Any attempt
       *          towards »perfect-forwarding« would be potentially fragile and not worth the effort,
       *          since the optimiser sees the operation as a whole.
       *  @todo 2/2025 However, the LeftReplacedArgs _could_ then possibly moved into the bind function,
       *          as could the functor, once we replace the Apply-template by STDLIB features.
       */
      static LeftReducedFunc
      bindFront (SIG const& f, Tuple<ValTypes> arg)
        {
          LeftReplacedArgs params {BuildL(std::move(arg))};
          return func::Apply<ARG_CNT>::template bind<LeftReducedFunc> (f, params);
        }
      
      /** do a partial function application, closing the last arguments<br/>
       *  `f(a,b,c)->res  +  (b,c)`  yields  `f(a)->res`
       *  
       *  @param f   function, function pointer or functor
       *  @param arg value tuple, used to close function arguments, aligned to the right end.
       *  @return new function object, holding copies of the values and using them at the
       *          closed arguments; on invocation, only the remaining arguments need to be supplied.
       */
      static RightReducedFunc
      bindBack (SIG const& f, Tuple<ValTypes> arg)
        {
          RightReplacedArgs params {BuildR(std::move(arg))};
          return func::Apply<ARG_CNT>::template bind<RightReducedFunc> (f, params);
        }
    };
  
  
  
  /**
   * Bind a specific argument to an arbitrary value.
   * Especially, this "value" might be another binder.
   */
  template<typename SIG, typename X, uint pos>
  class BindToArgument
    {
      using Args     = typename _Fun<SIG>::Args;
      using Ret      = typename _Fun<SIG>::Ret;
      using ArgsList = typename     Args::List;
      using ValList  = typename Types<X>::List;
      
      enum { ARG_CNT = count<ArgsList>::value };

      using RemainingFront     = typename Splice<ArgsList, ValList, pos>::Front;
      using RemainingBack      = typename Splice<ArgsList, ValList, pos>::Back;
      using PlaceholdersBefore = typename func::PlaceholderTuple<RemainingFront>::List;
      using PlaceholdersBehind = typename func::PlaceholderTuple<RemainingBack,pos+1>::List;
      
      using PreparedArgs     = typename Append< typename Append< PlaceholdersBefore
                                                               , ValList >::List
                                              , PlaceholdersBehind
                                              >::List;
      using ReducedArgs      = typename Append<RemainingFront, RemainingBack>::List;
      
      using PreparedArgTypes = typename Types<PreparedArgs>::Seq;
      using RemainingArgs    = typename Types<ReducedArgs>::Seq;
      
      using ReducedSig = typename BuildFunType<Ret,RemainingArgs>::Sig;
      
      template<class SRC, class TAR, size_t i>
      using IdxSelector = typename PartiallyInitTuple<SRC, TAR, pos>::template IndexMapper<i>;
      
      using BuildPreparedArgs = TupleConstructor<PreparedArgTypes, IdxSelector>;
      
      
      
    public:
      using ReducedFunc = function<ReducedSig>;
      
      static ReducedFunc
      reduced (SIG& f, X val)
        {
          Tuple<PreparedArgTypes> params {BuildPreparedArgs{std::make_tuple (val)}};
          return func::Apply<ARG_CNT>::template bind<ReducedFunc> (f, params);
        }
    };
  
  
  
  namespace { // ...helpers for specifying types in function declarations....
    
    using std::get;
    using util::unConst;
    
    
    template<typename RET, typename ARG>
    struct _Sig
      {
        typedef typename BuildFunType<RET, ARG>::Sig Type;
        typedef TupleApplicator<Type> Applicator;
      };
    
    template<typename SIG, typename ARG>
    struct _Clo
      {
        typedef typename _Fun<SIG>::Ret Ret;
        typedef typename _Sig<Ret,ARG>::Type Signature;
        typedef FunctionClosure<Signature> Type;
      };
    
    template<typename FUN1, typename FUN2>
    struct _Chain
      {
        using Ret  = typename _Fun<FUN2>::Ret;
        using Args = typename _Fun<FUN1>::Args;
        
        using FunType = typename BuildFunType<Ret,Args>::Fun;
        static auto adaptedFunType() { return FunType{}; }
        
        
        template<typename F1, typename F2
                ,typename RET, typename... ARGS>
        static auto
        composedFunctions (F1&& f1, F2&& f2, _Fun<RET(ARGS...)>)
          {
            tuple<F1,F2> binding{forward<F1> (f1)
                                ,forward<F2> (f2)
                                };
            return [binding = move(binding)]
                   (ARGS ...args) -> RET
                      {
                        auto& functor1 = get<0>(binding);
                        auto& functor2 = get<1>(binding);
                        //
                        return functor2 (functor1 (forward<ARGS> (args)...));
                      };
          }
      };
    
    template<typename FUN>
    struct _PapS
      {
        using Ret  = typename _Fun<FUN>::Ret;
        using Args = typename _Fun<FUN>::Args;
        using Arg  = typename Split<Args>::Head;
        using Rest = typename Split<Args>::Tail;
        
        using FunType = typename BuildFunType<Ret,Rest>::Fun;
        static auto adaptedFunType() { return FunType{}; }
        
        
        template<typename F, typename A
                ,typename RET, typename... ARGS>
        static auto
        bindFrontArg (F&& fun, A&& arg, _Fun<RET(ARGS...)>)
          {
            tuple<F,A> binding{forward<F> (fun)
                              ,forward<A> (arg)
                              };
            return [binding = move(binding)]
                   (ARGS ...args) -> RET
                      {
                        auto& functor = get<0>(binding);
                        //                           //Warning: might corrupt ownership
                        return functor ( forward<A> (unConst (get<1>(binding)))
                                       , forward<ARGS> (args)...);
                      };
          }
      };
    
    template<typename FUN>
    struct _PapE
      {
        using Ret  = typename _Fun<FUN>::Ret;
        using Args = typename _Fun<FUN>::Args;
        using Arg  = typename Split<Args>::End;
        using Rest = typename Split<Args>::Prefix;
        
        using FunType = typename BuildFunType<Ret,Rest>::Fun;
        static auto adaptedFunType() { return FunType{}; }
        
        
        template<typename F, typename A
                ,typename RET, typename... ARGS>
        static auto
        bindBackArg (F&& fun, A&& arg, _Fun<RET(ARGS...)>)
          {
            tuple<F,A> binding{forward<F> (fun)
                              ,forward<A> (arg)
                              };
            return [binding = move(binding)]
                   (ARGS ...args) -> RET
                      {
                        auto& functor = get<0>(binding);
                        //
                        return functor ( forward<ARGS> (args)...
                                       , forward<A> (unConst (get<1>(binding))));
                      };
          }
      };
    
  } // (End) argument type shortcuts
  
  
  
  
  /*  ========== function-style interface =============  */
  
  /** build a TupleApplicator, which embodies the given
   *  argument tuple and can be used to apply them
   *  to various functions repeatedly.
   */
  template<typename...ARG>
  inline
  typename _Sig<void, Types<ARG...>>::Applicator
  tupleApplicator (std::tuple<ARG...>& args)
  {
    typedef typename _Sig<void,Types<ARG...>>::Type Signature;
    return TupleApplicator<Signature> (args);
  }
  
  
  /** apply the given function to the argument tuple
   * @deprecated 11/23 meanwhile provided by the standard lib! */
  template<typename SIG, typename...ARG>
  inline
  typename _Fun<SIG>::Ret
  apply (SIG& f, std::tuple<ARG...>& args)
  {
    typedef typename _Fun<SIG>::Ret Ret;                          //
    typedef typename _Sig<Ret,Types<ARG...>>::Type Signature;    // Note: deliberately re-building the Signature Type
    return TupleApplicator<Signature> (args) (f);               //        in order to get better error messages here
  }
  
  /** close the given function over all arguments,
   *  using the values from the argument tuple.
   *  @return a closure object, which can be
   *          invoked later to yield the
   *          function result. */
  template<typename SIG, typename...ARG>
  inline
  typename _Clo<SIG,Types<ARG...>>::Type
  closure (SIG& f, std::tuple<ARG...>& args)
  {
    typedef typename _Clo<SIG,Types<ARG...>>::Type Closure;
    return Closure (f,args);
  }
  
  
  /** close the given function over the first argument.
   * @warning never tie an ownership-managing object by-value! */
  template<typename FUN, typename ARG>
  inline auto
  applyFirst (FUN&& fun, ARG&& arg)
  {
    static_assert (_Fun<FUN>(), "expect something function-like");
    return _PapS<FUN>::bindFrontArg (forward<FUN> (fun)
                                    ,forward<ARG> (arg)
                                    ,_PapS<FUN>::adaptedFunType());
  }
  
  /** close the given function over the last argument */
  template<typename FUN, typename ARG>
  inline auto
  applyLast (FUN&& fun, ARG&& arg)
  {
    static_assert (_Fun<FUN>(), "expect something function-like");
    return _PapE<FUN>::bindBackArg (forward<FUN> (fun)
                                   ,forward<ARG> (arg)
                                   ,_PapE<FUN>::adaptedFunType());
  }
  
  
  /** bind the last function argument to an arbitrary term,
   *  which especially might be a (nested) binder... */
  template<typename SIG, typename TERM>
  inline
  typename _PapE<SIG>::FunType::Functor
  bindLast (SIG& f, TERM&& arg)
  {
    enum { LAST_POS = -1 + count<typename _Fun<SIG>::Args::List>::value };
    return BindToArgument<SIG,TERM,LAST_POS>::reduced (f, std::forward<TERM> (arg));
  }
  
  
  /** build a functor chaining the given functions: feed the result of f1 into f2.
   * @note the mathematical notation would be `chained ≔ f2 ∘f1`
   */
  template<typename FUN1, typename FUN2>
  inline auto
  chained (FUN1&& f1, FUN2&& f2)
  {
    static_assert (_Fun<FUN1>(), "expect something function-like for function-1");
    static_assert (_Fun<FUN2>(), "expect something function-like for function-2");
    using Chain = _Chain<FUN1,FUN2>;
    return Chain::composedFunctions (forward<FUN1> (f1)
                                    ,forward<FUN2> (f2)
                                    ,Chain::adaptedFunType());
  }
  
  
  
}}} // namespace lib::meta::func
#endif
