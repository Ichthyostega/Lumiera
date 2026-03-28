/*
  TUPLE-CONCEPT.hpp  -  metaprogramming support for tuple-like data types

   Copyright (C)
     2025,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file tuple-concept.hpp
 ** Metaprogramming with tuples-of-types and the `std::tuple` record.
 ** This header complements typelist.hpp and provides a bridge from type sequences
 ** to the tuple type provided by the standard library, including traits and
 ** helpers to build tuple types from metaprogramming and to pretty-print tuples.
 ** 
 ** Notably, a `concept tuple_like` is provided here, which is satisfied for any type
 ** in compliance with the »[tuple protocol]«. This solution reaches beyond the scope
 ** of the C++ standard — and in fact opposes the decision of the C++ Committee
 ** _not to provide such a concept_, as this can not be defined in full genericity
 ** for any type that can be handled by the _structural bindings of the C++ language.
 ** 
 ** Since the Lumiera support library is part of a specific application framework, and
 ** does not intend to be a general purpose library, we prefer a partially complete
 ** definition that allows us to subsume and handle custom types under a »Tuple« concept.
 ** @note Unfortunately the C++ committee did not only decline to provide a tuple_like concept,
 **       rather they choose to lock-down essential parts of the accompanying library to their
 **       internal and hard-wired selection of what is considered "tuple-like". This forces us
 **       to redefine and replace also the crucial function `std::apply`, so that a generic
 **       function can be applied to any _tuple-like_ entity.
 ** @see TupleHelper_test
 ** @see tuple-helper.hpp extended tuple support functions
 ** [tuple protocol]: https://en.cppreference.com/w/cpp/language/structured_binding.html#Case_2:_binding_a_type_implementing_the_tuple_operations
 */


#ifndef LIB_META_TUPLE_CONCEPT_H
#define LIB_META_TUPLE_CONCEPT_H

#include "lib/meta/util.hpp"
#include "lib/meta/variadic-helper.hpp"

#include <type_traits>
#include <functional>
#include <tuple>


namespace lib {
namespace meta {
  
  using std::remove_cv_t;
  using std::is_reference_v;
  using std::remove_reference_t;
  
  
  /** @internal building-block: a type supporting the `tuple_size` metafunction */
  template<class TUP>
  concept tuple_sized = requires
    {
      { std::tuple_size<TUP>::value } -> std::convertible_to<size_t>;
    };
  
  
  /** @internal building-block: a type where elements can be accessed through a `get` friend function */
  template<class TUP, std::size_t idx>
  concept tuple_adl_accessible = requires(TUP tup)
    {
      typename std::tuple_element_t<idx, TUP>;
      { get<idx>(tup) } -> std::convertible_to<std::tuple_element_t<idx, TUP>&>;
    };
  
  /** @internal building-block: a type where elements can be accessed through a `get` member function */
  template<class TUP, std::size_t idx>
  concept tuple_mem_accessible = requires(TUP tup)
    {
      typename std::tuple_element_t<idx, TUP>;
      { tup.template get<idx>() } -> std::convertible_to<std::tuple_element_t<idx, TUP>&>;
    };
  
  template<class TUP, std::size_t idx>
  concept tuple_element_accessible = tuple_mem_accessible<TUP,idx> or tuple_adl_accessible<TUP,idx>;
  
  template<class TUP>
  concept tuple_accessible =
    tuple_sized<TUP> and
    WithIdxSeq<std::tuple_size_v<TUP>>::andAll([](auto idx)
                                                {
                                                 return tuple_element_accessible<TUP,idx>;
                                                });
  
  
  /**
   * Concept to mark any type compliant to the »tuple protocol«
   * @note a type satisfying this concept can participate in _structured bindings_,
   *       yet not every type that can be used in such bindings will fulfil this concept,
   *       since structured bindings can also be applied to _some POD like_ data records.
   */
  template<class TUP>
  concept tuple_like = not is_reference_v<TUP>
                   and tuple_sized<remove_cv_t<TUP>>
                   and tuple_accessible<remove_cv_t<TUP>>;
  
  
  /**
   * Helper for abstracted / unified access to member elements of any _tuple-like_
   * @remark preferably uses a `get<i>` member function, falling back to a
   *         free function `get`, which is found by ADL.
   */
  template<std::size_t idx, class TUP>
                            requires(tuple_like<std::remove_reference_t<TUP>>)
  decltype(auto)
  getElm (TUP&& tup)
  {
    using Tup = std::remove_reference_t<TUP>;
    static_assert (0 < std::tuple_size_v<Tup>);
    if constexpr (tuple_mem_accessible<Tup,0>)
      {
        if constexpr (std::is_reference_v<TUP>)
          return tup.template get<idx>();
        else
          { // return value copy when tuple given as RValue
            using Elm = std::tuple_element_t<idx, TUP>;
            Elm elm(tup.template get<idx>());
            return elm;
          }
      }
    else
      {     // ▽▽▽ ADL
        using std::get;
        return get<idx> (std::forward<TUP> (tup));
      }
  }
  
  
  
  
  /**
   * Specialisation of variadic access for any tuple-like
   * @see variadic-helper.hpp
   */
  template<tuple_like TUP>
  struct ElmTypes<TUP>
    {
      template<typename>
      struct Extract;
      template<size_t...idx>
      struct Extract<std::index_sequence<idx...>>
        {
          using ElmTypes = Types<std::tuple_element_t<idx,TUP> ...>;
        };
      
      static constexpr size_t SIZ = std::tuple_size_v<TUP>;
      
      using Idx = std::make_index_sequence<SIZ>;
      using Seq = Extract<Idx>::ElmTypes;
      using Tup = RebindVariadic<std::tuple, Seq>::Type;
      
      template<template<class> class META>
      using Apply = ElmTypes<Seq>::template Apply<META>;
      
      template<template<typename...> class O>
      using Rebind = RebindVariadic<O, Seq>::Type;
      
      template<template<class> class PRED>
      using AndAll = ElmTypes<Apply<PRED>>::template Rebind<std::__and_>;
      
      template<template<class> class PRED>
      using OrAll  = ElmTypes<Apply<PRED>>::template Rebind<std::__or_>;
    };
  
  
  
  
  
  
  
  namespace { // apply to tuple-like : helpers...
    
    /** @internal invocation helper similar to C++17 — but preferring a custom `get` impl */
    template<typename FUN, typename TUP, size_t...Idx>
    constexpr decltype(auto)
    __unpack_and_apply (FUN&& f, TUP&& tup, std::index_sequence<Idx...>)
    {
      return std::invoke (std::forward<FUN> (f)
                         ,getElm<Idx> (std::forward<TUP>(tup))...
                         );
    }
    
    
    /** @internal invoke a metafunction with \a FUN and all element types from the _tuple-like_ \a TUP */
    template<template<typename...> class META, class FUN, class TUP>
    struct _InvokeMetafunTup
      {
        using Tupl = std::decay_t<TUP>;
        using Elms = ElmTypes<Tupl>::Seq;
        using Args = Prepend<FUN, Elms>::Seq;
        using Type = RebindVariadic<META, Args>::Type;
      };
    template<template<typename...> class META, class FUN, class TUP>
    struct _InvokeMetafunTup<META, FUN, TUP&>
      {
        using Tupl = std::decay_t<TUP>;
        using Elms = typename ElmTypes<Tupl>::template Apply<std::add_lvalue_reference_t>;
        using Args = Prepend<FUN, Elms>::Seq;
        using Type = RebindVariadic<META, Args>::Type;
      };
    
    template<class FUN, class TUP>
    inline constexpr bool can_nothrow_invoke_tup = _InvokeMetafunTup<std::is_nothrow_invocable,FUN,TUP>::Type::value;
  }
  
  /**
   * Replacement for `std::apply` — yet applicable to _tuple-like custom types_.
   * For unclear reasons, the standard chooses to reject such custom types, and
   * only allows a fixed set of explicitly defined facilities from the Stdlib
   * (tuple, pair, array, and some ranges stuff).
   * @todo 6/2025 as a first step, this replicates the implementation from C++17;
   *       the second step would be to constrain this to a concept `tuple_like`
   */
  template<class FUN, class TUP>   requires(tuple_like<remove_reference_t<TUP>>)
  constexpr decltype(auto)
  apply (FUN&& f, TUP&& tup)  noexcept (can_nothrow_invoke_tup<FUN,TUP> )
  {
    using Indices = std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<TUP>>>;
    
    return __unpack_and_apply (std::forward<FUN> (f)
                              ,std::forward<TUP> (tup)
                              ,Indices{}
                              );
  }
  
  
  /**
   * Tuple iteration: perform some arbitrary operation on each element of a tuple.
   * @note the given functor must be generic, since each position of the tuple
   *       may hold a data element of different type.
   * @remark credits to David Vandevoorde (member of C++ committee) for using
   *       std::apply to unpack the tuple's contents into an argument pack and
   *       then employ a fold expression with the comma operator.
   */
  template<class TUP, class FUN>   requires(tuple_like<remove_reference_t<TUP>>)
  constexpr void
  forEach (TUP&& tuple, FUN fun)
  {
           lib::meta::apply ([&fun]<typename...ELMS>(ELMS&&... elms)
                                  {
                                    (fun (std::forward<ELMS>(elms)), ...);
                                  }
                            ,std::forward<TUP> (tuple));
  }
  
  /**
   * Apply some arbitrary function onto all elements of a tuple.
   * @return a new tuple constructed from the results of this function
   * @note the functor must be generic and able to work with all element types
   * @warning pay attention to references; the function can take arguments by-ref
   *          and manipulate them (side-effect), and it may return references,
   *          which will be placed as such into the result tuple; furthermore,
   *          the argument tuple can also be taken as reference...
   * @remark The tuple constructor invocation is preceded by a regular argument evaluation,
   *          which has unspecified evaluation order (even in C++17); _no assumptions_ can be
   *          made regarding the order the functor will see the source tuple elements.
   *          Notably this differs from #forEach, where a fold-expression with comma-operator
   *          is used, which is guaranteed to evaluate from left to right.
   */
  template<class TUP, class FUN>   requires(tuple_like<remove_reference_t<TUP>>)
  constexpr auto
  mapEach (TUP&& tuple, FUN fun)
  {
    return lib::meta::apply ([&fun]<typename...ELMS>(ELMS&&... elms)
                                  {  //..construct the type explicitly (make_tuple would decay fun result types)
                                    using Tuple = std::tuple<decltype(fun (std::forward<ELMS>(elms))) ...>;
                                    return Tuple (fun (std::forward<ELMS>(elms)) ...);
                                  }
                            ,std::forward<TUP> (tuple));
  }
  
}} // namespace lib::meta
#endif /*LIB_META_TUPLE_CONCEPT_H*/
