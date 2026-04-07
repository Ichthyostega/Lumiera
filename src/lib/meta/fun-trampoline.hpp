/*
  FUN-TRAMPOLINE.hpp  -  generate static functor table

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file fun-trampoline.hpp
 ** Build a trampoline table with functors.
 ** This Metaprogramming helper uses a _builder template_ to fill an embedded
 ** static dispatcher table with generated functors. These can then be accessed
 ** by index, typically to be invoked.
 ** 
 ** The setup works entirely on a static level, so that any mention of some
 ** FunTrampoline template instantiation causes the compiler to populate the
 ** corresponding functor table. This arrangement can be used, for example,
 ** to auto-generate an index accessor for a specific tuple, so that a
 ** runtime subscript or iteration can be derived. Obviously doing so
 ** requires the types in the tuple to be [reconcilable](\ref common-type.hpp),
 ** so that a generic functor can be defined to handle all fields in the tuple
 ** based on a common generic scheme. Without that, tuples can in general
 ** only be iterated and transformed by compile-time metaprogramming.
 ** 
 ** @see FunTrampoline_test
 ** @see CommonResult_test
 ** @see tuple-idx-adaptor.hpp
 */


#ifndef LIB_META_FUN_TRAMPOLINE_H
#define LIB_META_FUN_TRAMPOLINE_H


#include "include/logging.h"
#include "lib/meta/function.hpp"
#include "lib/meta/variadic-helper.hpp"

#include <utility>
#include <array>


namespace lib {
namespace meta {
  
  /**
   * FunTrampoline relies on a helper template
   * to be used as _trampoline builder_ to generate
   * a sequence of functors that can be placed into a
   * static dispatcher table. Thus, a trampoline_builder
   * - must be default constructible
   * - must define a Functor type...
   * - that can be used as array element
   * - and must provide a templated build<idx>() function
   */
  template<class B>
  concept trampoline_builder = requires(B builder)
    {
      typename B::Functor;
      std::array<typename B::Functor, 1>{};
      requires std::default_initializable<B>;
      { builder.template build<0>() } -> std::convertible_to<typename B::Functor>;
    };
  
  
  /**
   * A static functor table, populated automatically.
   * @tparam BUILD helper Template to generate functors
   *               that are placed into a static table
   * @tparam TSEQ  sequence of types to instantiate
   *               the functors with, iteratively.
   * @remarks
   *  - the given type sequence is handled with the
   *    ElmTypes helper template (see \ref variadic-helper.hpp)
   *  - the BUILD template is instantiated with that \a TSEQ
   *  - statically-for-each-index, the build<idx>() function
   *    on the builder is invoked, which is expected to produce
   *    an instance of BUILD::Functor
   * @warning as soon as some instance of this FunTrampoline
   *    is used / referred, the compiler will emit the trampoline
   *    table into static storage; the resulting increase of the
   *    executable size is something to be aware of, when using
   *    this construct for transient processing, driven by
   *    actual argument types.
   */
  template<template<class> class BUILD, typeseq TSEQ>
           requires trampoline_builder<BUILD<TSEQ>>
  class FunTrampoline
    {
      using AllElm  = ElmTypes<TSEQ>;
      using Indices = AllElm::Idx;
      using Builder = BUILD<TSEQ>;
      using Functor = Builder::Functor;
      using Table   = std::array<Functor, AllElm::SIZ>;
      
      template<size_t...idx>
      static constexpr Table
      buildFunctors (std::index_sequence<idx...>)
        {
          return {Builder().template build<idx>()...};
        }
      
      static constexpr Table functors = buildFunctors (Indices{});
      
    public:
      static constexpr size_t size()   { return functors.size(); }
      static constexpr size_t memSiz() { return sizeof(functors);}
      
      static Functor const&
      dispatch (size_t idx)
        {
          ENSURE (idx < functors.size());
          return functors[idx];
        }
      
      using ResType = _Fun<Functor>::Ret;
    };
  
}}// namespace lib::meta
#endif /*LIB_META_FUN_TRAMPOLINE_H*/
