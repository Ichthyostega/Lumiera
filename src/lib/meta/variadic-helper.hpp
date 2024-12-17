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
 ** from variadic parameter packs. The goal is to (pre)process flexible argument
 ** lists _at compile time,_ driven by template instantiation, allowing to specialise
 ** and react specifically on some concrete pattern of argument types.
 ** 
 ** @warning the metaprogramming part of Lumiera to deal with type sequences is in a
 **          state of transition, since C++11 now offers direct language support for
 **          processing of flexible template parameter sequences ("parameter packs").
 **          It is planned to regroup and simplify our homemade type sequence framework
 **          to rely on variadic templates and integrate better with std::tuple.
 **          It is clear that we will _retain some parts_ of our own framework,
 **          since programming with _Loki-style typelists_ is way more obvious
 **          and straight forward than handling of template parameter packs,
 **          since the latter can only be rebound through pattern matching.
 ** @todo transition lib::meta::Types to variadic parameters  /////////////////////////////////TICKET #987
 ** 
 ** @see control::CommandDef usage example
 ** @see TupleHelper_test
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
#include "lib/meta/util.hpp"

namespace lib {
namespace meta {
  
   //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #987 temporary WORKAROUND -- to be obsoleted
  /**
   * temporary workaround:
   * alternative definition of "type sequence",
   * already using variadic template parameters.
   * @remarks the problem with our existing type sequence type
   *    is that it fills the end of each sequence with NullType,
   *    which was the only way to get a flexible type sequence
   *    prior to C++11. Unfortunately these trailing NullType
   *    entries do not play well with other variadic defs.
   * @deprecated when we switch our primary type sequence type
   *    to variadic parameters, this type will be obsoleted.             ////////////////////////////////////TICKET #987 : make lib::meta::Types<TYPES...> variadic
   */
  template<typename...TYPES>
  struct TySeq
    {
      using Seq = TySeq;
      using List = typename Types<TYPES...>::List;
    };
  
  
  /**
   * temporary workaround: additional specialisation for the template
   * `Prepend` to work also with the (alternative) variadic TySeq.
   * @see typeseq-util.hpp
   */
  template<typename T, typename...TYPES>
  struct Prepend<T, TySeq<TYPES...>>
  {
    using Seq  = TySeq<T, TYPES...>;
    using List = typename Types<T, TYPES...>::List;
  };
  
  
  /**
   * temporary workaround: strip trailing NullType entries from a
   * type sequence, to make it compatible with new-style variadic
   * template definitions.
   * @note the result type is a TySec, to keep it apart from our
   *    legacy (non-variadic) lib::meta::Types
   * @deprecated necessary for the transition to variadic sequences      ////////////////////////////////////TICKET #987 : make lib::meta::Types<TYPES...> variadic
   */
  template<typename SEQ>
  struct StripNullType;
  
  template<typename T, typename...TYPES>
  struct StripNullType<Types<T,TYPES...>>
    {
      using TailSeq = typename StripNullType<Types<TYPES...>>::Seq;
      
      using Seq = typename Prepend<T, TailSeq>::Seq;
    };
  
  template<typename...TYPES>
  struct StripNullType<Types<NullType, TYPES...>>
    {
      using Seq = TySeq<>;  // NOTE: this causes the result to be a TySeq
    };
   //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #987 temporary WORKAROUND(End) -- to be obsoleted
  
  
  
  
  
  
  
  
  
  
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
      using Ascending  = typename BuildIndexSeq<n-1>::Ascending::template AppendElm<n-1>;
      using Descending = typename BuildIndexSeq<n-1>::Descending::template PrependElm<n-1>;
      
      template<size_t d>
      using OffsetBy   = typename BuildIndexSeq<n-1>::template OffsetBy<d>::template AppendElm<n-1+d>;
      
      template<size_t x>
      using FilledWith = typename BuildIndexSeq<n-1>::template FilledWith<x>::template AppendElm<x>;
      
      template<size_t c>
      using First = typename BuildIndexSeq<std::min(c,n)>::Ascending;
      
      template<size_t c>
      using After = typename BuildIndexSeq< (n>c)? n-c : 0>::template OffsetBy<c>;
    };
  
  template<>
  struct BuildIndexSeq<0>
    {
      using Empty = IndexSeq<>;
      
      using Ascending  = Empty;
      using Descending = Empty;
      
      template<size_t>
      using OffsetBy   = Empty;
      
      template<size_t>
      using FilledWith = Empty;
      
      template<size_t>
      using First = Empty;
      
      template<size_t>
      using After = Empty;
    };
  
  
  
  /**
   * build a sequence of index numbers based on a type sequence
   */
  template<typename...TYPES>
  struct BuildIdxIter
    {
      enum {SIZ = sizeof...(TYPES) };
      using Builder = BuildIndexSeq<SIZ>;
      
      using Ascending  = typename Builder::Ascending;
      using Descending = typename Builder::Descending;
      
      template<size_t d>
      using OffsetBy   = typename Builder::template OffsetBy<d>;
      
      template<size_t x>
      using FilledWith = typename Builder::template FilledWith<x>;
      
      template<size_t c>
      using First = typename Builder::template First<c>;
      
      template<size_t c>
      using After = typename Builder::template After<c>;
    };

  /** build an index number sequence from a type sequence */
  template<typename...TYPES>
  struct BuildIdxIter<Types<TYPES...>>
    {
      ///////////////////////TICKET #987 : since Types<T...> is not variadic, need to strip NullType here (instead of just using sizeof...(TYPES)
      enum {SIZ = lib::meta::count<typename Types<TYPES...>::List>::value };
      using Builder = BuildIndexSeq<SIZ>;
      
      using Ascending  = typename Builder::Ascending;
      using Descending = typename Builder::Descending;
      
      template<size_t d>
      using OffsetBy   = typename Builder::template OffsetBy<d>;
      
      template<size_t x>
      using FilledWith = typename Builder::template FilledWith<x>;
      
      template<size_t c>
      using First = typename Builder::template First<c>;
      
      template<size_t c>
      using After = typename Builder::template After<c>;
    };
  
  
  
  
  /* ==== Rebinding Variadic Arguments ==== **/
  
  /**
   * Metaprogramming helper to transfer variadic arguments.
   * - builds a new type instantiation from the Template \a X
   * - possibly picks up the variadic argument pack from a given
   *   source template `U<ARGS....>`
   * @tparam X a variadic template
   */
  template<template<typename...> class X, typename...ARGS>
  struct RebindVariadic
    {
      using Type = X<ARGS...>;
    };
  
  template<template<typename...> class X
          ,template<typename...> class U
          ,typename...ARGS>
  struct RebindVariadic<X, U<ARGS...>>
    {
      using Type = X<ARGS...>;
    };
  
  
  
  

  /* ==== Invoke with index from variadic ==== **/
  
  /** helper to invoke a functor, passing instances of std::integral_constant
   * @tparam N size of the index-sequence to use for instantiation
   */
  template<size_t N>
  class WithIdxSeq
    {
      template<class FUN, size_t...idx>
      static void
      invoke (FUN&& fun, std::index_sequence<idx...>)
        {
          (fun (std::integral_constant<size_t,idx>{}), ...);
        }
      
    public:
      template<class FUN>
      static void
      invoke (FUN&& fun)
        {
          invoke (std::forward<FUN>(fun), std::make_index_sequence<N>{});
        }
    };
  
  /**
   * Invoke a function (or λ) with index numbers derived from some variadic count.
   * Notably this construct can be used for compile-time iteration over a structure.
   * Instances of `std::integral_constant` are passed in sequence to the functor.
   * The _size_ of the index sequence is derived from the following sources
   * - if the type \a TTX is _tuple-like,_ then std::tuple_size<TTX> is used
   * - otherwise, if the type is a loki-style type sequence or type list,
   *   the number of type nodes is used
   * - otherwise, as fall-back the number of template parameters is used
   */
  template<class TTX, class FUN>
  inline void
  forEachIDX (FUN&& fun)
  {
    auto N = []{
                  if constexpr (is_Structured<TTX>())
                    return size_t(std::tuple_size<TTX>::value);
                  else
                  if constexpr (lib::meta::is_Typelist<TTX>::value)
                    return lib::meta::count<typename TTX::List>::value;
                  else
                    { // Fallback: rebind template arguments into a type sequence
                      using Seq = typename RebindVariadic<TySeq, TTX>::Type;
                      return size_t(count<typename Seq::List>::value);
                    }
               };
    
    WithIdxSeq<N()>::invoke (std::forward<FUN> (fun));
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
