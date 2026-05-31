/*
  VARIADIC-HELPER.hpp  -  metaprogramming utilities for parameter- and type sequences

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file variadic-helper.hpp
 ** Metaprogramming with type sequences based on variadic template parameters.
 ** The type rebinding- and helper templates in this header allow to perform
 ** simple sequence manipulations on sequences of template parameters extracted
 ** from variadic parameter packs. Among other things, this allows the sequence
 ** of function arguments to be flexibly reshaped and remoulded at compile-time.
 ** Driven by template instantiation, this enables adaptation in response to
 ** specific argument types for each actual instance. Such metaprogramming
 ** capabilities can be amplified significantly through the use of generic-λ.
 ** @note with C++23, this framework could be expanded to cover also any
 **       »tuple-like« product type, drawing from our non-standard extension
 **       with the `concept tuple_like`, as defined in \ref tuple-concept.hpp.
 ** 
 ** @remark in Lumiera, over time three different approaches were developed for
 **         handling sequences of types in metaprogramming; some of these techniques
 **         are better suited for specific kinds of tasks than others
 **         - templates with variadic arguments (e.g. std::tuple) can be manipulated directly
 **         - a type-sequence `Types<T...>` can be primed / rebound from other variadic templates
 **         - Loki-style type-lists are created from type-sequences and enable elaborate manipulations
 ** 
 ** @see feed-manifold.hpp advanced usage example in the Render Engine
 ** @see VariadicHelper_test
 ** @see tuple-concept.hpp
 ** @see typelist.hpp
 ** @see function.hpp
 ** @see generator.hpp
 ** 
 */


#ifndef LIB_META_VARIADIC_HELPER_H
#define LIB_META_VARIADIC_HELPER_H

#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-util.hpp"
#include "lib/meta/typeseq-util.hpp"
#include "lib/meta/variadic-rebind.hpp"
#include "lib/meta/trait.hpp"

namespace lib {
namespace meta {
 
  
  
  /* ==== Build Variadic Sequences ==== **/
  
  
  /** Hold a sequence of index numbers as template parameters */
  template<size_t...idx>
  struct IndexSeq
    {
      template<size_t i>
      using AppendElm = IndexSeq<idx..., i>;
      
      template<size_t i>
      using PrependElm = IndexSeq<i, idx...>;
    };
  
  /**
   * build regular sequences of index number
   * e.g. `IndexSeq<0, 1, 2, ..., n-1>`
   */
  template<size_t n>
  struct BuildIndexSeq
    {
      using Ascending  = BuildIndexSeq<n-1>::Ascending::template AppendElm<n-1>;
      using Descending = BuildIndexSeq<n-1>::Descending::template PrependElm<n-1>;
      
      template<size_t d>
      using OffsetBy   = BuildIndexSeq<n-1>::template OffsetBy<d>::template AppendElm<n-1+d>;
      
      template<size_t x>
      using FilledWith = BuildIndexSeq<n-1>::template FilledWith<x>::template AppendElm<x>;
      
      template<size_t c>
      using First      = BuildIndexSeq<std::min(c,n)>::Ascending;
      
      template<size_t c>
      using After      = BuildIndexSeq< (n>c)? n-c : 0>::template OffsetBy<c>;
    };
  
  template<>
  struct BuildIndexSeq<0>
    {
      using EmptySeq = IndexSeq<>;
      
      using Ascending  = EmptySeq;
      using Descending = EmptySeq;
      
      template<size_t>
      using OffsetBy   = EmptySeq;
      
      template<size_t>
      using FilledWith = EmptySeq;
      
      template<size_t>
      using First = EmptySeq;
      
      template<size_t>
      using After = EmptySeq;
    };
  
  
  
  /**
   * build a sequence of index numbers based on a type sequence
   */
  template<typename...TYPES>
  struct BuildIdxIter
    {
      enum {SIZ = sizeof...(TYPES) };
      using Builder = BuildIndexSeq<SIZ>;
      
      using Ascending  = Builder::Ascending;
      using Descending = Builder::Descending;
      
      template<size_t d>
      using OffsetBy   = Builder::template OffsetBy<d>;
      
      template<size_t x>
      using FilledWith = Builder::template FilledWith<x>;
      
      template<size_t c>
      using First      = Builder::template First<c>;
      
      template<size_t c>
      using After      = Builder::template After<c>;
    };

  /** build an index number sequence from a type sequence */
  template<typename...TYPES>
  struct BuildIdxIter<Types<TYPES...>>
    : BuildIdxIter<TYPES...>
    { };
  
  
  
  
  
  
  
  
  /* ==== Build and Rebuild variadic type sequences ==== **/
  
  /**
   * Variadic type sequence builder.
   * This metaprogramming helper template provides an unified view
   * to handle _»tuple-like« types and variadic _type sequences._
   * - the constant #SIZ gives the number of elements
   * - the nested type #Idx can be used as _index sequence_
   * - #Seq is a _variadic type sequence_ with the extracted types
   * - #Tup is a std::tuple over these types
   * - the nested template #Apply wraps each type into another template
   * - #Rebind likewise instantiates another template with the element types
   * - #AndAll applies a predicate and combines the result with _logical and_
   * - #OrAny similarly evaluates _logical or_ on the application results
   */
  template<class X,  typename =void>
  struct ElmTypes
    {
      static constexpr size_t SIZ = 1;
      using Idx = std::index_sequence<SIZ>;
      using Seq = Types<X>;
      using Tup = std::tuple<X>;
      
      template<template<class> class META>
      using Apply = Types<META<X>>;
      template<template<typename...> class O>
      using Rebind = O<X>;
      template<template<class> class PRED>
      using AndAll = std::__and_<PRED<X>>;
      template<template<class> class PRED>
      using OrAny  = std::__or_<PRED<X>>;
    };
  
  /** Partial specialisation to handle type sequences */
  template<typename...TYPES>
  struct ElmTypes<Types<TYPES...>>
    {
      static constexpr size_t SIZ = sizeof...(TYPES);
      using Idx = std::make_index_sequence<SIZ>;
      using Seq = Types<TYPES...>;
      using Tup = std::tuple<TYPES...>;
      
      template<template<class> class META>
      using Apply = Types<META<TYPES>...>;
      
      template<template<typename...> class O>
      using Rebind = lib::meta::RebindVariadic<O, Seq>::Type;
      
      template<template<class> class PRED>
      using AndAll = ElmTypes<Apply<PRED>>::template Rebind<std::__and_>;
      
      template<template<class> class PRED>
      using OrAny  = ElmTypes<Apply<PRED>>::template Rebind<std::__or_>;
    };
  
  // Note: a further specialisation for any »tuple-like« is defined in tuple-concept.hpp
  
  
  
  template<class SEQ>
  using Any_Const = ElmTypes<SEQ>::template OrAny<isConst>;
  template<class SEQ>
  using All_LRef = ElmTypes<SEQ>::template AndAll<isLRef>;
  
  template<class SEQ>
  static constexpr bool Any_Const_v = Any_Const<SEQ>::value;
  template<class SEQ>
  static constexpr bool All_LRef_v = All_LRef<SEQ>::value;
  
  
  
  
  
  
  
  /* ==== Invoke with index from variadic ==== **/
  
  /** helper to invoke a functor, passing instances of std::integral_constant
   * @tparam N size of the index-sequence to use for instantiation
   * @remark the functor is given for...
   *  - to be invoked either (as void) for each index
   *  - or as a predicate, combining the results with AND / OR
   */
  template<size_t N>
  class WithIdxSeq
    {
      template<class FUN, size_t...idx>
      static constexpr void
      invoke_forEach (FUN&& fun, std::index_sequence<idx...>)
        {
          (fun (std::integral_constant<size_t,idx>{}), ...);
        }
      
      template<class FUN, size_t...idx>
      static constexpr bool
      and_forEach (FUN&& fun, std::index_sequence<idx...>)
        {
          return (fun (std::integral_constant<size_t,idx>{}) and ...);
        }
      
      template<class FUN, size_t...idx>
      static constexpr bool
      or_forEach (FUN&& fun, std::index_sequence<idx...>)
        {
          return (fun (std::integral_constant<size_t,idx>{}) or ...);
        }
      
      using IdxSeq = std::make_index_sequence<N>;
      
    public:
      template<class FUN>
      static constexpr void
      invoke (FUN&& fun)
        {
          invoke_forEach (std::forward<FUN>(fun), IdxSeq{});
        }
      
      template<class FUN>
      static constexpr bool
      andAll (FUN&& fun)
        {
          return and_forEach (std::forward<FUN>(fun), IdxSeq{});
        }
      
      template<class FUN>
      static constexpr bool
      orAny (FUN&& fun)
        {
          return or_forEach (std::forward<FUN>(fun), IdxSeq{});
        }
    };
  
  /**
   * Invoke a function (or λ) with index numbers derived from some variadic count.
   * Notably this construct can be used for compile-time iteration over a structure.
   * Instances of `std::integral_constant` are passed in sequence to the functor.
   * The _size_ of the index sequence is derived using a \ref ElmTypes specialisation
   * - if the type \a TTX is _tuple-like,_ then std::tuple_size<TTX> is used
   * - otherwise, if the type is a _type sequence_ (`Types<T...>`), then
   *   the size of this meta sequence is used
   * - otherwise, sequence-size ≡ 1 is used as fall-back
   */
  template<class TTX, class FUN>
  inline void
  forEachIDX (FUN&& fun)
  {
    WithIdxSeq<ElmTypes<TTX>::SIZ>::invoke (std::forward<FUN> (fun));
  }
  
  template<class TTX, class FUN>
  inline bool
  andAllIDX (FUN&& fun)
  {
    return WithIdxSeq<ElmTypes<TTX>::SIZ>::andAll (std::forward<FUN> (fun));
  }
  
  template<class TTX, class FUN>
  inline bool
  orAnyIDX (FUN&& fun)
  {
    return WithIdxSeq<ElmTypes<TTX>::SIZ>::orAny (std::forward<FUN> (fun));
  }
  
  
  

  
  
  
  
  /* ==== Manipulation of variadic arguments ==== **/
  
  namespace { // Implementation delegate template...
    /**
     * @internal pick a single argument from a variadic parameter pack
     * @tparam i the index number (zero based) of the argument to select
     * @warning i must be smaller than the number of arguments available
     */
    template<size_t i>
    struct SelectVararg
      {
        template<typename ARG, typename...ARGS>
        static auto
        get (ARG, ARGS&& ...args)
          {
            return SelectVararg<i-1>::get (std::forward<ARGS> (args)...);
          }
      };
    
    template<>
    struct SelectVararg<0>
      {
        template<typename ARG, typename...ARGS>
        static auto
        get (ARG&& a, ARGS...)
          {
            return std::forward<ARG>(a);
          }
      };
    
    /**
     * @internal helper to decide if SelectVararg shall be applied.
     * When the boolean condition does not hold, then, instead of selecting
     * from the argument list, an element of type DEFAULT is created as fallback.
     */
    template<bool, typename, size_t idx>
    struct SelectOrInit
      : SelectVararg<idx>
      { };
    
    template<typename DEFAULT, size_t idx>
    struct SelectOrInit<false, DEFAULT, idx>
      {
        template<typename...ARGS>
        static DEFAULT
        get (ARGS&&...)
          {
            return DEFAULT{};
          }
      };
  }//(End)Implementation
  
  
  /**
   * Helper to single out one argument from a variadic argument pack.
   * @tparam idx the index number (zero based) of the argument to select
   * @remark typically this function is used "driven" by an likewise variadic index sequence,
   *         where the index sequence itself is picked up by a pattern match; this usage pattern
   *         allows arbitrarily to handle some of the arguments of a variable argument list,
   *         as determined by the index sequence passed in.
   */
  template<size_t idx, typename...ARGS>
  constexpr inline auto
  pickArg (ARGS&&... args)
    {
      static_assert (idx < sizeof...(args), "insufficient number of arguments");
      
      return SelectVararg<idx>::get (std::forward<ARGS> (args)...);
    }
  
  /**
   * Helper to pick one initialisation argument from a variadic argument pack,
   * falling back to a default constructed element of type `DEFAULT` in case of
   * insufficient number of variadic arguments.
   * @tparam idx the index number (zero based) of the argument to select
   * @tparam DEFALUT type of the default element to construct as fallback
   */
  template<size_t idx, typename DEFAULT, typename...ARGS>
  constexpr inline auto
  pickInit (ARGS&&... args)
    {
      return SelectOrInit<(idx < sizeof...(args)), DEFAULT, idx>::get (std::forward<ARGS> (args)...);
    }
  
  
  
}} // namespace lib::meta
#endif /*LIB_META_VARIADIC_HELPER_H*/
