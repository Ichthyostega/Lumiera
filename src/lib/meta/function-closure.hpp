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
 ** These are features known from functional programming and are here implemented
 ** based on std::bind, to support especially the case when some arguments of a
 ** function are known at an earlier stage, and should thus be »baked in«, while
 ** further arguments will be supplied later, for the actual invocation. This
 ** implies to _close_ (and thus bind) some arguments immediately, while keeping
 ** other arguments open to — so the result of this operation is again a function,
 ** albeit with fewer arguments.
 ** Additionally, we allow for _composing_ (chaining) of two functions.
 ** @warning this header is in a state of transition as of 6/2025, because functionality
 **       of this kind will certainly be needed in future, but with full support for lambdas,
 **       move-only types and perfect forwarding. The current implementation of std::bind
 **       is already quite optimal regarding this support. A gradual rework has been started,
 **       and will lead to a complete rewrite of the core functionality eventually, making
 **       better use of variadic templates and library functions like std::apply, which
 **       were not available at the time of the first implementation.
 ** 
 ** @todo the implementation is able to handle partial application with N arguments,
 **       but currently we need just one argument, thus only this case was wrapped
 **       up into a convenient functions func::applyFirst and func::applyLast
 ** @todo 2/25 note that there is a bind_front in C++20 and C++23 will provide a bind_back
 **       helper, which would provide the implementation fully in accordance with current
 **       expectations (including move, constexpr); if we choose to retain a generic
 **       function-style front-end, it should be aligned with these standard facilities.
 **       We might want to retain a simple generic interface especially for binding some
 **       selected argument, which handles the intricacies of storing the functor.
 ** @todo 6/25 note however that the full-fledged partial function application is a
 **       **relevant core feature** and is used in the NodeBuilder (see tuple-closure.hpp).
 **       Further improvement of the implementation is thus mandated...  ////////////////////////////////////TICKET #1394
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
    struct PlaceholderTuple<Nil, i>
      {
        using List = Nil;
      };
    
    
    
    using std::tuple_element;
    using std::tuple_size;
    using std::get;
    
    
    /**
     * Builder for a tuple instance, where only some ctor parameters are supplied,
     * while the remaining arguments will be default constructed. The use case is
     * to create a function binder, where some arguments shall be passed through
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
        using DestType = typename std::tuple_element_t<i, TAR>;
        
        
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
            SRC&& initArgs;
            
            operator DestType<idx>()
              {                             // std::get passes-through reference kind
                return std::get<idx-start> (forward<SRC>(initArgs));
              }
          };
        
        template<size_t idx>
        struct IndexMapper<idx, false>
          {
            SRC&& initArgs;
            
            operator DestType<idx>()
              {
                return DestType<idx>();
              }
          };
      };
    
    
    /**
     * Helper to package a given invokable,
     * so that it has a well defined function signature.
     * @warning defined argument types must be suitable.
     * @remark need to define the wrapper functor class explicitly,
     *         to be able to work around any const / non-const cases;
     *         we can not overload `operator()` because then `decltype(operator())`
     *         would be ambiguous, breaking the detection of the function signature.
     */
    template<typename...ARGS>
    struct AdaptInvokable
      {
        template<class FUN>
        static auto
        buildWrapper (FUN&& fun)
          {
                  struct Wrap
                    {
                      FUN fun_;
                      
                      auto
                      operator() (ARGS... args)
                        {
                          return fun_(forward<ARGS>(args)...);
                        }
                    };
            
            return Wrap{forward<FUN>(fun)};
          }
      };

  } // (END) impl-namespace
  
  
  
  
  
  /* ======= core operations: closures and partial application ========= */
  
  /**
   * Base technique: build a binder with arguments from a tuple.
   * @remark based on the apply-λ-trick by David Vandervoorde
   *         to unpack the tuple elements as argument pack
   * @note both the functor and the arguments can be passed
   *       by _perfect forwarding_, yet both will be **copied**
   *       into the resulting binder
   * @param fun   anything »invokable«
   * @param tuple a suitable set of arguments, or std binding placeholders
   * @return a _binder functor_, as generated by `std::bind`; all arguments
   *         supplied with explicitly given values will be _closed_, while
   *         argument positions marked with `std::_Placeholder<N>` instances
   *         will remain _open_ to accept arguments on the resulting function.
   */
  template<class FUN, class TUP, typename = enable_if_Tuple<TUP>>
  auto
  bindArgTuple (FUN&& fun, TUP&& tuple)
  {
    return std::apply ([functor = forward<FUN>(fun)]
                       (auto&&... args)
                            {
                              return std::bind (move(functor)
                                               ,forward<decltype(args)> (args) ...);
                            }
                      ,std::forward<TUP> (tuple));
  }
  
  /**
   * Workaround to yield std::bind functors
   * with a clearly defined function signature.
   * @tparam TYPES type-sequence or type-list of the function arguments to take
   * @remark the result of `std::bind` exposes several overloaded `operator()`,
   *         which unfortunately defeats detection of the resulting function signature
   *         by downstream code, which needs this information to guide further processing.
   * @see TupleClosureBuilder::wrapBuilder(closureFun)
   */
  template<class TYPES, class FUN>
  auto
  buildInvokableWrapper (FUN&& fun)
    {
      static_assert (is_Typelist<TYPES>::value);
      using ArgTypes = typename TYPES::Seq;
      using Builder = typename lib::meta::RebindVariadic<AdaptInvokable, ArgTypes>::Type;
      
      return Builder::buildWrapper (forward<FUN> (fun));
    }

  
  
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
   * @todo 6/2025 while this is the core of this feature, the implementation is
   *       still confusing and bears traces from being the first attempt at solving
   *       this problem, with a pre-C++11 feature set. Driven by other requirements,
   *       over time several variations were added, so that now the main functionality
   *       is now _implemented twice_, in a very similar way in BindToArgument.
   *       Actually, the latter seems much clearer, and possibly PApply could be
   *       rewritten into a front-end and delegate to BindToArgument...  ////////////////////////////////////TICKET #1394
   */
  template<typename SIG, typename VAL>
  class PApply
    {
      using Args     = typename _Fun<SIG>::Args;
      using Ret      = typename _Fun<SIG>::Ret;
      using ArgsList = typename Args::List;
      using ValList  = typename VAL::List;
      using ValTypes = typename Types<ValList>::Seq;           // reconstruct a type-seq from a type-list
      
      enum { ARG_CNT = count<ArgsList>()
           , VAL_CNT = count<ValList>()
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
       *  @note   BuildL, and consequently std::bind _must take the arguments by-value._ Any attempt
       *          towards »perfect-forwarding« would be potentially fragile and not worth the effort,
       *          since the optimiser sees the operation as a whole.
       *  @todo 2/2025 However, the LeftReplacedArgs _could_ then possibly moved into the bind function,
       *          as could the functor, once we replace the Apply-template by STDLIB features.
       *  @todo 5/2025 seems indeed we could perfect-forward everything into the binder object.
       */
      static LeftReducedFunc
      bindFront (SIG const& f, Tuple<ValTypes> arg)
        {
          LeftReplacedArgs params {BuildL(std::move(arg))};
          return bindArgTuple (f, params);
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
          return bindArgTuple (f, params);
        }
    };
  
  
  
  /**
   * Bind a specific argument to an arbitrary value.
   * Notably this "value" might be another binder.
   * 
   * @todo 6/2025 while this was added later to handle a special case,
   *       now after clean-up (see #987) this seems to be the much cleaner
   *       implementation approach and can easily be generalised to cover
   *       all the general features available through PApply; it would be
   *       sufficient to replace the single template parameter \a X by a
   *       type-sequence of the values to bind. All use cases of
   *       partial application could be modelled this way...   //////////////////////////////////////////////TICKET #1394
   */
  template<typename SIG, typename X, uint pos>
  class BindToArgument
    {
      using Args     = typename _Fun<SIG>::Args;
      using Ret      = typename _Fun<SIG>::Ret;
      using ArgsList = typename     Args::List;
      using ValList  = typename Types<X>::List;
      
      enum { ARG_CNT = count<ArgsList>() };

      using RemainingFront     = typename Splice<ArgsList, ValList, pos>::Front;
      using RemainingBack      = typename Splice<ArgsList, ValList, pos>::Back;
      using PlaceholdersBefore = typename func::PlaceholderTuple<RemainingFront>::List;
      using PlaceholdersBehind = typename func::PlaceholderTuple<RemainingBack,pos+1>::List;
      
      using PreparedArgsRaw    = typename Append<typename Append<PlaceholdersBefore    // arguments before the splice: passed-through
                                                                ,ValList >::List       // splice in the value tuple
                                                ,PlaceholdersBehind                    // arguments behind the splice: passed-through
                                                >::List;
      using PreparedArgs     = Prefix<PreparedArgsRaw, ARG_CNT>;
      using ReducedArgs      = typename Append<RemainingFront, RemainingBack>::List;
      
      using PreparedArgTypes = typename Types<PreparedArgs>::Seq;
      using RemainingArgs    = typename Types<ReducedArgs>::Seq;
      
      
      template<class SRC, class TAR, size_t i>
      using IdxSelector = typename PartiallyInitTuple<SRC, TAR, pos>::template IndexMapper<i>;
      
      using BuildPreparedArgs = TupleConstructor<PreparedArgTypes, IdxSelector>;
      
      
      
    public:
      template<class FUN, class VAL>
      static auto
      reduced (FUN&& f, VAL&& val)
        {
          Tuple<PreparedArgTypes> bindingTuple {
            BuildPreparedArgs{
              std::forward_as_tuple (
                  forward<VAL>(val))}};
          
          return buildInvokableWrapper<RemainingArgs>(
                  bindArgTuple (forward<FUN>(f)
                              , move(bindingTuple)));
        }
    };
  
  
  
  namespace { // ...helpers for specifying types in function declarations....
    
    using std::get;
    using util::unConst;
    
    
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
    
  } // (End) argument type shortcuts
  
  
  
  
  /*  ========== function-style interface =============  */
  
  /** bind (close) the first function argument to an arbitrary term.
   * @note this term may be a (nested) binder, which will
   *       be _activated late_, at the time of invocation
   * @warning when an ownership-managing object is tied by-value,
   *       it will be moved and stored in the binder functor.
   */
  template<typename FUN, typename TERM>
  inline auto
  bindFirst (FUN&& f, TERM&& arg)
  {
    return BindToArgument<FUN,TERM, 0>::reduced (std::forward<FUN> (f)
                                                ,std::forward<TERM> (arg));
  }
  
  template<typename FUN, typename TERM>
  inline auto
  bindLast (FUN&& f, TERM&& arg)
  {
    enum { LAST_POS = -1 + count<typename _Fun<FUN>::Args>() };
    return BindToArgument<FUN,TERM,LAST_POS>::reduced (std::forward<FUN> (f)
                                                      ,std::forward<TERM> (arg));
  }
  
  
  /** build a functor chaining the given functions: feed the result of f1 into f2.
   * @note the mathematical notation would be `chained ≔ f2 ∘f1`
   * @warning the »function« here are taken as _invokable_ and will be stored as-given
   *       into a tuple in the generated functor. If a _reference_ is passed in,
   *       then a reference is stored into the closure (beware!). This was
   *       added deliberately to support ove-only functors.
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
