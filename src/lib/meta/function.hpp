/*
  FUNCTION.hpp  -  metaprogramming utilities for transforming function types

   Copyright (C)
     2009-2025,       Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file function.hpp
 ** Metaprogramming tools for detecting and transforming function types.
 ** Sometimes it is necessary to build and remould a function signature, e.g. for
 ** creating a functor or a closure based on an existing function of function pointer.
 ** Functors, especially in the form of Lambdas, can be used to parametrise a common
 ** skeleton of computation logic to make it work with concrete implementation parts
 ** tied directly to the actual usage. A _functor centric_ approach places the focus
 ** on the _computation structure_ and often complements the more _object centric_ one,
 ** where relations and responsibilities are structured and built from building blocks
 ** with fixed type relations.
 ** 
 ** However, handling arbitrary functions as part of a common structure scheme requires
 ** to cope with generally unknown argument and result types. Notably the number and
 ** sequence of the expected arguments of an unknown function can be arbitrary, which
 ** places the onus upon the handling and receiving context to adapt to actual result
 ** types and to provide suitable invocation arguments. Typically, failure to do so
 ** will produce a sway of almost impenetrable compilation failure messages, making
 ** the reason of the failure difficult to spot. So to handle this challenge, ways
 ** to inspect and isolate parts of the function's _signature_ become essential.
 ** 
 ** The _trait template_ [_Fun](\ref lib::meta::_Fun) is the entrance point to
 ** extract information regarding the function signature at compile time. Defined as
 ** a series partial template specialisations, it allows to create a level playing field
 ** and abstract over the wide array of different flavours of »invokable entities«:
 ** this can be direct references to some function in the code, function pointers,
 ** pointers to function members of an object, generally any object with an _function call_
 ** `operator()`, and especially the relevant features of the standard library, which are
 ** the generic function adapter `std::function` and the _binder_ objects created when
 ** _partially closing_ (binding) some function arguments ahead of the actual invocation.
 ** And last but not least the _Lambda functions_, which represent an abstracted, opaque
 ** and anonymous function and are supported by a special syntactical construct, allowing
 ** to pass _blocks of C++ code_ directly as argument to some other function or object.
 ** 
 ** Code to control the process of compilation, by detecting some aspects of the involved
 ** types and react accordingly, is a form of **metaprogramming** — and allows for dynamic
 ** **code generation** right from the ongoing compilation process. The flexible and open
 ** nature of function arguments often directly leads into such metaprogramming tasks.
 ** While the contemporary C++ language offers the language construct of _variadics_,
 ** such variadic template argument packs can only be dissected and processed further
 ** by instantiating another template with the argument pack and then exploiting
 ** _partial template specialisation_ to perform a _pattern match_ against the variadic
 ** arguments. Sometimes this approach comes in natural, yet in many commonplace situations
 ** if feels kind of »backwards« and leads to very tricky, hard to decipher template code.
 ** The Lumiera support library thus offers special features to handle _type sequences_
 ** and _type lists_ directly, allowing to process by recursive list programming patterns
 ** as known from the LISP language. The `_Fun` trait mentioned above directly provides
 ** an entrance point to this kind of in-compilation programming, as it exposes a nested
 ** type definition `_Fun<F>::Args`, which is an instance of the **type sequence** template
 ** \ref lib::meta::Types. This in turn exposes a nested type definition `List`, leading
 ** to the [Loki Type Lists](\ref typelist.hpp).
 ** 
 ** Furthermore, this header defines some very widely used abbreviations for the most
 ** common tasks of detecting aspects of a function. These are often used in conjunction
 ** with the _constexpr if_ available since C++17:
 ** - _FunRet is a templated type definition to extract the return type
 ** - _FunArg extracts the type of single-arg function and fails compilation else
 ** - has_Arity<N> is a _meta-function_ to check for a specific number of arguments
 ** - is_NullaryFun,  is_UnaryFun, is_BinaryFun, is_TernaryFun (common abbreviations)
 ** - the metafunction has_Sig is often placed close to the entrance point of an
 **   elaborate, template based ecosystem and helps to produce better error messages
 ** 
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
  
  
  /**************************************************************************//**
   * Trait template for uniform access to function signature types.
   * Extracts the type information contained in a function or functor type,
   * so it can be manipulated by metaprogramming. This template works on
   * anything _function like_, irrespective if the parameter is given
   * as function reference, function pointer, member function pointer,
   * functor object, `std::function` or lambda. The embedded typedefs
   * allow to expose
   * - `Ret` : the return type
   * - `Args`: the sequence of argument types as type sequence `Types<ARGS...>`
   * - `Sig` : the bare function signature type
   * - `Functor` : corresponding Functor type which can be instantiated or copied.
   * - `ARITY` : number of arguments
   * 
   * This template can also be used in metaprogramming with `enable_if` to enable
   * some definition or specialisation only if a function-like type was detected; thus
   * the base case holds no nested type definitions and inherits from std::false_type.
   * The primary, catch-all case gets activated whenever used on functor objects
   * proper, i.e. anything with an `operator()`.
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
   *          - on results of std::bind
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
      using Args = Types<ARGS...>;
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
  
  
  
  
  
  /**
   * Build function types from given Argument types.
   * As embedded typedefs, you'll find a std::function #Func
   * and the bare function signature #Sig
   * @param RET the function return type
   * @param ARGS a type sequence describing the arguments
   */
  template<typename RET, typename ARGS>
  struct BuildFunType;
  
  template<typename RET, typename...ARGS>
  struct BuildFunType<RET, Types<ARGS...>>
  {
    using Sig = RET(ARGS...);
    using Fun = _Fun<Sig>;
    using Func = function<Sig>;
    using Functor = Func;
  };
  
  
  
  
  
  /** abbreviation for referring to a function's return type */
  template<typename FUN>
  using _FunRet = _Fun<FUN>::Ret;
  
  namespace {
    template<typename FUN>
    struct _DetectSingleArgFunction
      {
        static_assert(_Fun<FUN>()           , "something funktion-like required");
        static_assert(_Fun<FUN>::ARITY == 1 , "function with exactly one argument required");
        
        using Sig = _Fun<FUN>::Sig;
        using Arg = _Fun<Sig>::Args::List::Head;
      };
  }
  
  /** abbreviation for referring to a function's single Argument type */
  template<typename FUN>
  using _FunArg = _DetectSingleArgFunction<FUN>::Arg;
  
  
  
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
  
  
  
}} // namespace lib::meta
#endif
