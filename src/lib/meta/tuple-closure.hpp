/*
  TUPLE-CLOSURE.hpp  -  metaprogramming tools for closing a function over given arguments

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
  
  template<class PAR>
  struct TupleClosureBuilder;
  
  template<template<typename...> class TUP, typename...PARS>
  struct TupleClosureBuilder<TUP<PARS...>>
    {
      using Tuple = TUP<PARS...>;
      using TupleBuilderSig = Tuple(PARS...);
      
      static Tuple
      buildParam (PARS ...params)
        {
          return {params...};
        }
      
      template<typename...VALS>
      static auto
      closeFront (VALS ...vs)
        {
          using ClosedTypes = TySeq<VALS...>;
          return wrapBuilder (func::PApply<TupleBuilderSig, ClosedTypes>::bindFront (buildParam, std::make_tuple(vs...)));
        }
      
      template<typename...VALS>
      static auto
      closeBack (VALS ...vs)
        {
          using ClosedTypes = TySeq<VALS...>;
          return wrapBuilder (func::PApply<TupleBuilderSig, ClosedTypes>::bindBack (buildParam, std::make_tuple(vs...)));
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
  
  template<typename...TTT>
  struct ArrayAdapt;
  
  namespace {
    
    template<typename...TTT>
    struct AllSame
      : std::true_type
      { };
    
    template<typename T1, typename T2, typename...TS>
    struct AllSame<T1,T2,TS...>
      : __and_<is_same<T1,T2>
              ,AllSame<T2,TS...>
              >
      { };
    
    
    template<typename T, size_t N>
    struct Repeat
      {
        using Rem = typename Repeat<T, N-1>::Seq;
        using Seq = typename Prepend<T,Rem>::Seq;
      };
      
    template<typename T>
    struct Repeat<T,0>
      {
        using Seq = TySeq<>;
      };
    
    template<typename T, size_t N>
    struct _Adapt
      {
        using NFold = typename Repeat<T,N>::Seq;
        using Array = typename RebindVariadic<ArrayAdapt, NFold>::Type;
      };
    
    template<typename T, size_t N>
    using _AdaptArray_t = typename _Adapt<T,N>::Array;
  }
  
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
  
  // Note: the std::get<i> function will pick the subclass
}
#endif /*LIB_META_TUPLE_CLOSURE_H*/
