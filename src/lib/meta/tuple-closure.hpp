/*
  TUPLE-CLOSURE.hpp  -  metaprogramming tools for tuple-likes with partial closure

   Copyright (C)
     2025,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file tuple-closure.hpp
 ** Partial binding for construction of tuple-like records.
 ** Sometimes tuple-like structures must be created as part of library code,
 ** in a situation where _some of the values_ are known and should be fixed-in,
 ** while other values need to be supplied late. Obviously this implies creating
 ** a functor, and then partially to close some arguments. While seemingly simple,
 ** this task is often complicated by the need to support _»tuple-like«_ records,
 ** i.e. anything which adheres to the »C++ tuple protocol« (e.g. std::array).
 ** Which requires to pick up the actual container type by template pattern match
 ** and to rely solely on the helper functions from the STLIB for data access.
 ** 
 ** A relevant use-case is the handling of invocation parameters for Render Nodes:
 ** typically, some parameters are of technical nature and can be configured during
 ** the setup-phase of the render network, while other parameters allow the user
 ** to exert artistic control and will be supplied later, through automation.
 ** 
 ** @see weaving-pattern-builder.hpp
 ** @see NodeBuilder_test::build_Node_closedParam()
 ** 
 */


#ifndef LIB_META_TUPLE_CLOSURE_H
#define LIB_META_TUPLE_CLOSURE_H

#include "lib/meta/function-closure.hpp"
#include "lib/meta/tuple-helper.hpp"

#include <utility>
#include <tuple>
#include <array>



namespace lib {
namespace meta{
  
  /**
   * Metaprogramming helper to build a constructor-function
   * for »tuple-like« records, where some of the initialisation
   * values are immediately closed (≙ fixed), while the remaining
   * ones are supplied as function arguments.
   */
  template<class PAR>
  struct TupleClosureBuilder;
  
  template<template<typename...> class TUP, typename...PARS>
  struct TupleClosureBuilder<TUP<PARS...>>
    {
      using Tuple = TUP<PARS...>;
      using TupleBuilderSig = Tuple(PARS...);
      
      static Tuple
      buildRecord (PARS ...params)
        {
          return {std::move(params)...};
        }
      
      template<typename...VALS>
      static auto
      closeFront (VALS&& ...vs)
        {
          using ClosedTypes = TySeq<std::decay_t<VALS>...>;
          auto  boundArgs = std::make_tuple (std::forward<VALS> (vs)...);                      // Note: must be passed by-val here
          return wrapBuilder (func::PApply<TupleBuilderSig, ClosedTypes>::bindFront (buildRecord, move(boundArgs)));
        }
      
      template<typename...VALS>
      static auto
      closeBack (VALS&& ...vs)
        {
          using ClosedTypes = TySeq<std::decay_t<VALS>...>;
          auto  boundArgs = std::make_tuple (std::forward<VALS> (vs)...);                      // Note: must be passed by-val here
          return wrapBuilder (func::PApply<TupleBuilderSig, ClosedTypes>::bindBack  (buildRecord, move(boundArgs)));
        }
      
      template<size_t idx, typename VAL>
      static auto
      close (VAL&& val)
        {
          using BoundVal = std::decay_t<VAL>;
          return wrapBuilder (func::BindToArgument<TupleBuilderSig,BoundVal,idx>::reduced (buildRecord, std::forward<VAL>(val)));
        }
      
    private:
      template<class CLO>
      static auto
      wrapBuilder (CLO partialClosure)
        {
          using RemainingArgs = typename _Fun<CLO>::Args;
          using RemainingParams = typename lib::meta::RebindVariadic<TUP, RemainingArgs>::Type;
          return [closure = move(partialClosure)
                 ]
                 (RemainingParams remPar)
                    {
                      return std::apply (closure, remPar);
                    };
        }
    };
  
  
  
  /* ===== adapt array for tuple-like signature ===== */
  
  /**
   * Metaprogramming adapter to overlay a tuple-like signature
   * on top of std::array, with N times the same type.
   */
  template<typename...TTT>
  struct ArrayAdapt;
  
  
  /** Metafunction to detect if a type-sequence holds uniform types */
  template<typename...TTT>
  struct AllSame
    : std::true_type // trivially true for empty conjunction and single element
    { };
  
  template<typename T1, typename T2, typename...TS>
  struct AllSame<T1,T2,TS...>
    : __and_<is_same<T1,T2>
            ,AllSame<T2,TS...>
            >
    { };
  
  
  namespace {
    /** Type constructor */
    template<typename T, size_t N>
    struct _Adapt
      {
        using NFold = typename Repeat<T,N>::Seq;
        using Array = typename RebindVariadic<ArrayAdapt, NFold>::Type;
      };
    
    template<typename T, size_t N>
    using _AdaptArray_t = typename _Adapt<T,N>::Array;
  }
  
  /** @note adding seamless conversion and compount-init */
  template<typename T, typename...TT>
  struct ArrayAdapt<T,TT...>
    : std::array<T, 1+sizeof...(TT)>
    {
      static_assert (AllSame<T,TT...>()
                    ,"Array can only hold elements of uniform type");
      using Array = std::array<T, 1+sizeof...(TT)>;
      
      
      ArrayAdapt (Array const& o) : Array{o}       { }
      ArrayAdapt (Array     && r) : Array{move(r)} { }
      
      template<typename...XS>
      ArrayAdapt (XS&& ...inits)
        : Array{std::forward<XS> (inits)...}
        { }
    };
  
  
  /** partial specialisation to handle a std::array.
   * @note the expected input on partially closures
   *       is then also an array, with fewer elements.
   */
  template<typename T, size_t N>
  struct TupleClosureBuilder<std::array<T,N>>
    : TupleClosureBuilder<_AdaptArray_t<T,N>>
    {
    };
  
  
}} // namespace lib::meta


namespace std { // Specialisation to support C++ »Tuple Protocol« and structured bindings.
  
  /** determine compile-time fixed size of the adapted std::array */
  template<typename...TTT>
  struct tuple_size<lib::meta::ArrayAdapt<TTT...> >
    : integral_constant<size_t, sizeof...(TTT)>
    { };
  
  /** expose the element type of the adapted std::array */
  template<size_t I, typename...TTT>
  struct tuple_element<I, lib::meta::ArrayAdapt<TTT...> >
    : tuple_element<I, typename lib::meta::ArrayAdapt<TTT...>::Array>
    { };
  
  // Note: the std::get<i> function will pick the baseclass anyway
}
#endif /*LIB_META_TUPLE_CLOSURE_H*/
