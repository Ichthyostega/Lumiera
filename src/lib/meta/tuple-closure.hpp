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



namespace lib {
namespace meta{
  
  template<class PAR>
  struct TupleClosureBuilder;
  
  template<template<typename...> class TUP, typename...PARS>
  struct TupleClosureBuilder<TUP<PARS...>>
    {
      using Params = TUP<PARS...>;
      
      static Params
      buildParam (PARS ...params)
        {
          return {params...};
        }
      
      template<typename...VALS>
      static auto
      closeFront (VALS ...vs)
        {
          using lib::meta::_Fun;
          using lib::meta::TySeq;
          using lib::meta::func::PApply;
          using ClosedTypes = TySeq<VALS...>;
          using ParamBuilderSig = Params(PARS...);
          auto  partialClosure = PApply<ParamBuilderSig, ClosedTypes>::bindFront (buildParam, std::make_tuple(vs...));
          using RemainingArgs = typename _Fun<decltype(partialClosure)>::Args;
          using RemainingParams = typename lib::meta::RebindVariadic<TUP, RemainingArgs>::Type;
          return [closure = move(partialClosure)
                 ]
                 (RemainingParams remPar)
                    {
                      return std::apply (closure, remPar);
                    };
        }
    };
  
  
}} // namespace lib::meta
#endif
