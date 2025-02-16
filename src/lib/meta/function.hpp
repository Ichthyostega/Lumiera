/*
  FUNCTION.hpp  -  metaprogramming utilities for transforming function types

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file function.hpp
 ** Metaprogramming tools for transforming functor types.
 ** Sometimes it is necessary to build and remould a function signature, e.g. for
 ** creating a functor or a closure based on an existing function of function pointer.
 ** This is a core task of functional programming, but sadly C++ in its current shape
 ** is still lacking in this area. (C++11 significantly improved this situation).
 ** As an \em pragmatic fix, we define here a collection of templates, specialising
 ** them in a very repetitive way for up to 9 function arguments. Doing so enables
 ** us to capture a function, access the return type and argument types as a typelist,
 ** eventually to manipulate them and re-build a different signature, or to create
 ** specifically tailored bindings.
 ** 
 ** If the following code makes you feel like vomiting, please look away,
 ** and rest assured: you aren't alone.
 ** 
 ** @todo get rid of the repetitive specialisations
 **       and use variadic templates to represent the arguments             /////////////////////////////////TICKET #994
 ** 
 ** 
 ** @see control::CommandDef usage example
 ** @see function-closure.hpp generic function application
 ** @see typelist.hpp
 ** @see tuple.hpp
 ** 
 */


#ifndef LIB_META_FUNCTION_H
#define LIB_META_FUNCTION_H

#include "lib/meta/typelist.hpp"
#include "lib/meta/util.hpp"

#include <functional>



namespace lib {
namespace meta{

  using std::function;
  
  
  /**
   * Helper for uniform access to function signature types.
   * Extract the type information contained in a function or functor type,
   * so it can be manipulated by metaprogramming. This template works on
   * anything _function like_, irrespective if the parameter is given
   * as function reference, function pointer, member function pointer,
   * functor object, `std::function` or lambda. The embedded typedefs
   * allow to pick up
   * - `Ret` : the return type
   * - `Args`: the sequence of argument types as type sequence `Types<ARGS...>`
   * - `Sig` : the bare function signature type
   * - `Functor` : corresponding Functor type which can be instantiated or copied.
   * 
   * This template can also be used in metaprogramming with `enable_if` to enable
   * some definition or specialisation only if a function-like type was detected; thus
   * the base case holds no nested type definitions and inherits from std::false_type.
   * The primary, catch-all case gets activated whenever on functor objects, i.e. anything
   * with an `operator()`.
   * The following explicit specialisations handle the other cases, which are
   * not objects, but primitive types (function (member) pointers and references).
   * @remarks The key trick of this solution is to rely on `decltype` of `operator()`
   *          and was proposed 10/2011 by user «[kennytm]» in this [stackoverflow].
   * @note for a member pointer to function, only the actual arguments in the
   *          function signature are reflected. But if you bind such a member
   *          pointer into a `std::function`, an additional first parameter
   *          will show up to take the `this` pointer of the class instance.
   * @warning this detection scheme fails when the signature of a function call
   *          operator is ambiguous, which is especially the case
   *          - when there are several overloads of `operator()`
   *          - when the function call operator is templated
   *          - on *generic lambdas*
   *          All these cases will activate the base (false) case, as if the
   *          tested subject was not a function at all. Generally speaking,
   *          it is _not possible_ to probe a generic lambda or templated function,
   *          unless you bind it beforehand into a std::function with correct signature.
   * @see FunctionSignature_test
   * 
   * [kennytm]: http://stackoverflow.com/users/224671/kennytm
   * [stackoverflow]: http://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda/7943765#7943765 "figure out parameter and return type of a Lambda"
   */
  template<typename FUN, typename SEL =void>
  struct _Fun
    : std::false_type
    {
      using Functor = FUN;
    };
  
  /** Specialisation for function objects and lambdas */
  template<typename FUN>
  struct _Fun<FUN,  enable_if<has_FunctionOperator<FUN>> >
    : _Fun<decltype(&FUN::operator())>
    {
      using Functor = FUN;
    };
  
  /** Specialisation for a bare function signature */
  template<typename RET, typename...ARGS>
  struct _Fun<RET(ARGS...)>
    : std::true_type
    {
      using Ret  = RET;
      using Args = TySeq<ARGS...>;
      using Sig  = RET(ARGS...);
      using Functor = std::function<Sig>;
      enum { ARITY = sizeof...(ARGS) };
    };
  
  /** Specialisation to strip `noexcept` from the signature */
  template<typename RET, typename...ARGS>
  struct _Fun<RET(ARGS...) noexcept>
    : _Fun<RET(ARGS...)>
    { };
  
  /** Specialisation for using a function pointer */
  template<typename SIG>
  struct _Fun<SIG*>
    : _Fun<SIG>
    { };
  
  /** Specialisation to strip spurious const for type analysis */
  template<typename SIG>
  struct _Fun<SIG const&>
    : _Fun<SIG>
    { };
  
  /** Specialisation when using a function reference */
  template<typename SIG>
  struct _Fun<SIG&>
    : _Fun<SIG>
    { };
  
  /** Specialisation for passing a rvalue reference */
  template<typename SIG>
  struct _Fun<SIG&&>
    : _Fun<SIG>
    { };
  
  /** Specialisation to deal with member pointer to function */
  template<class C, typename RET, typename...ARGS>
  struct _Fun<RET (C::*) (ARGS...)>
    : _Fun<RET(ARGS...)>
    { };
  
  /** Specialisation to deal with member pointer to noexcept function */
  template<class C, typename RET, typename...ARGS>
  struct _Fun<RET (C::*) (ARGS...) noexcept>
    : _Fun<RET(ARGS...)>
    { };
  
  /** Specialisation to handle member pointer to const function;
   *  indirectly this specialisation also handles lambdas,
   *  as redirected by the main template (via `decltype`) */
  template<class C, typename RET, typename...ARGS>
  struct _Fun<RET (C::*) (ARGS...)  const>
    : _Fun<RET(ARGS...)>
    { };
  
  /** allow also to probe _plain member fields,_ which may hold a functor */
  template<class C, typename FUN>
  struct _Fun<FUN (C::*)>
    : _Fun<FUN>
    { };
  
  
  
  
  
  /** abbreviation for referring to a function's return type */
  template<typename FUN>
  using _FunRet = typename _Fun<FUN>::Ret;
  
  namespace {
    template<typename FUN>
    struct _DetectSingleArgFunction
      {
        static_assert(_Fun<FUN>()           , "something funktion-like required");
        static_assert(_Fun<FUN>::ARITY == 1 , "function with exactly one argument required");
        
        using Sig = typename _Fun<FUN>::Sig;
        using Arg = typename _Fun<Sig>::Args::List::Head;
      };
  }
  
  /** abbreviation for referring to a function's single Argument type */
  template<typename FUN>
  using _FunArg = typename _DetectSingleArgFunction<FUN>::Arg;
  
  
  
  template<typename FUN, uint a>
  using has_Arity = std::bool_constant<_Fun<FUN>::ARITY == a>;
  
  template<typename FUN>
  using is_NullaryFun = has_Arity<FUN,0>;
  
  template<typename FUN>
  using is_UnaryFun  = has_Arity<FUN,1>;
  
  template<typename FUN>
  using is_BinaryFun = has_Arity<FUN,2>;
  
  template<typename FUN>
  using is_TernaryFun = has_Arity<FUN,3>;
  
  
  
  
  
  /**
   * Meta-function to check that some _function like_ entity
   * offers the expected signature
   * @tparam SIG signature type (e.g. `float(int, bool)`)
   * @tparam FUN anything _function like_ (class with function call operator
   *             or std::function instance, or λ instance or language function
   *             reference or function pointer
   */
  template<typename FUN, typename SIG,  bool =_Fun<FUN>()>
  struct has_Sig
    : std::is_same<SIG, typename _Fun<FUN>::Sig>
    { };
  
  /** catch-all to prevent compilation failure for anything not function-like. */
  template<typename FUN, typename X>
  struct has_Sig<FUN,X, false>
    : std::false_type
    { };
  
  /**
   * Macro for a compile-time check to verify the given
   * generic functors or lambdas expose some expected signature.
   * @remark typically used when configuring a template with custom adapters.
   */
#define ASSERT_VALID_SIGNATURE(_FUN_, _SIG_) \
        static_assert (lib::meta::has_Sig<_FUN_, _SIG_>::value, \
                       "Function " STRINGIFY(_FUN_) " unsuitable, expected signature: " STRINGIFY(_SIG_));
  
  
  
  /**
   * Helper to pick up a member field for verification
   * @tparam SIG signature of the _function like_ entity expected
   * @tparam FUN address- or member-pointer, e.g. `&Class::member`
   * @return suitably parametrised \ref has_Sig instance (which is bool convertible)
   * @remark intended for use with generic types, when expecting a _somehow invokable_
   *         member, irrespective if a static function, member function or functor object
   */
  template<typename SIG, typename FUN>
  constexpr inline auto
  isFunMember (FUN)
  {
    return has_Sig<FUN,SIG>{};
  }
  
  /**
   * Macro for a compile-time check to verify some member is present
   * and comprises something invokable with a specific signature.
   * @remark typically used with _generic types_ or bindings
   */
#define ASSERT_MEMBER_FUNCTOR(_EXPR_, _SIG_) \
        static_assert (lib::meta::isFunMember<_SIG_>(_EXPR_), \
                       "Member " STRINGIFY(_EXPR_) " unsuitable, expect function signature: " STRINGIFY(_SIG_));
  
  
  
  
  
  
  /** Placeholder marker for a special argument position to be supplied later */
  template<class TAR>
  struct InstancePlaceholder { };
  
  namespace {// Helper to inject instance-pointer instead of placeholder...
    /**
     * @internal helper to detect an InstancePlaceholder
     * @return   an instance-pointer, statically casted to the marked target type.
     */
    template<class W, class TAR>
    constexpr inline TAR*
    maybeInject (W& instance, InstancePlaceholder<TAR>)
    {
      return static_cast<TAR*> (&instance);
    }
    
    /** (default case: fall-through) */
    template<class W, typename X>
    constexpr inline X
    maybeInject (W&, X&& x)
    {
      return std::move(x);
    }
  }//(End)Helper for lateBindInstance.

  /**
   * Fix-up the arguments for a member-function invocation,
   * allowing to inject the actual `this` instance into an existing argument sequence.
   * @remark invocation of a member function requires to supply the _object instance_ as
   *         first element in the argument list; sometimes this poses a design challenge,
   *         since the actual instance may not be known at the point where the other arguments
   *         are prepared. As a remedy, the position of the instance pointer can be marked with
   *         the \ref InstancePlaceholder, allowing to splice in the actual pointer when known.
   */
  template<class W, class TUP>
  constexpr inline auto
  lateBindInstance (W& instance, TUP&& invocation)
  {
    auto splice = [&instance](auto&& ...xs)
                    {
                      return std::tuple{maybeInject (instance, std::move(xs))...};
                    };
    return std::apply (splice, std::forward<TUP> (invocation));
  }
  
  
  
  
  
  
  
  /**
   * Build function types from given Argument types.
   * As embedded typedefs, you'll find a tr1 functor #Func
   * and the bare function signature #Sig
   * @param RET the function return type
   * @param ARGS a type sequence describing the arguments
   */                                  //////////////////////////////////////////////////////////////////////TICKET #987 : make lib::meta::Types<TYPES...> variadic, then replace this by a single variadic template
  template<typename RET, typename ARGS>
  struct BuildFunType;
  
                          ///////////////////////////////////////////////////////////////////////////////////TICKET #987 : this specialisation handles the variadic case and will be the only definition in future
  template<typename RET, typename...ARGS>
  struct BuildFunType<RET, TySeq<ARGS...>>
  {
    using Sig = RET(ARGS...);
    using Fun = _Fun<Sig>;
    using Func = function<Sig>;
    using Functor = Func;
  };
  
                          ///////////////////////////////////////////////////////////////////////////////////TICKET #987 : the following specialisations become obsolete with the old-style type-sequence
  template< typename RET>
  struct BuildFunType<RET, Types<> >
  {
    using Sig = RET(void);
    using Fun = _Fun<Sig>;
    using Func = function<Sig>;
    using Functor = Func;
  };
  
  
  template< typename RET
          , typename A1
          >
  struct BuildFunType<RET, Types<A1>>
  {
    using Sig = RET(A1);
    using Fun = _Fun<Sig>;
    using Func = function<Sig>;
    using Functor = Func;
  };
  
  
  template< typename RET
          , typename A1
          , typename A2
          >
  struct BuildFunType<RET, Types<A1,A2>>
  {
    using Sig = RET(A1,A2);
    using Fun = _Fun<Sig>;
    using Func = function<Sig>;
    using Functor = Func;
  };
  
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          >
  struct BuildFunType<RET, Types<A1,A2,A3>>
  {
    using Sig = RET(A1,A2,A3);
    using Fun = _Fun<Sig>;
    using Func = function<Sig>;
    using Functor = Func;
  };
  
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          >
  struct BuildFunType<RET, Types<A1,A2,A3,A4>>
  {
    using Sig = RET(A1,A2,A3,A4);
    using Fun = _Fun<Sig>;
    using Func = function<Sig>;
    using Functor = Func;
  };
  
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          , typename A5
          >
  struct BuildFunType<RET, Types<A1,A2,A3,A4,A5>>
  {
    using Sig = RET(A1,A2,A3,A4,A5);
    using Fun = _Fun<Sig>;
    using Func = function<Sig>;
    using Functor = Func;
  };
  
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          , typename A5
          , typename A6
          >
  struct BuildFunType<RET, Types<A1,A2,A3,A4,A5,A6>>
  {
    using Sig = RET(A1,A2,A3,A4,A5,A6);
    using Fun = _Fun<Sig>;
    using Func = function<Sig>;
    using Functor = Func;
  };
  
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          , typename A5
          , typename A6
          , typename A7
          >
  struct BuildFunType<RET, Types<A1,A2,A3,A4,A5,A6,A7>>
  {
    using Sig = RET(A1,A2,A3,A4,A5,A6,A7);
    using Fun = _Fun<Sig>;
    using Func = function<Sig>;
    using Functor = Func;
  };
  
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          , typename A5
          , typename A6
          , typename A7
          , typename A8
          >
  struct BuildFunType<RET, Types<A1,A2,A3,A4,A5,A6,A7,A8>>
  {
    using Sig = RET(A1,A2,A3,A4,A5,A6,A7,A8);
    using Fun = _Fun<Sig>;
    using Func = function<Sig>;
    using Functor = Func;
  };
  
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          , typename A5
          , typename A6
          , typename A7
          , typename A8
          , typename A9
          >
  struct BuildFunType<RET, Types<A1,A2,A3,A4,A5,A6,A7,A8,A9>>
  {
    using Sig = RET(A1,A2,A3,A4,A5,A6,A7,A8,A9);
    using Fun = _Fun<Sig>;
    using Func = function<Sig>;
    using Functor = Func;
  };
  
  
  
  
  
}} // namespace lib::meta
#endif
