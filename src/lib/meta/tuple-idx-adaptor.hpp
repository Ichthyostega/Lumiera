/*
  TUPLE-IDX-ADAPTOR.hpp  -  runtime-indexed access to tuples

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file tuple-idx-adaptor.hpp
 ** Adaptor to provide access to tuple elements by runtime subscript.
 ** Tuples are data records with distinct fields of different type; thus
 ** most of indexing, iteration and transformation of tuples need to be
 ** performed statically, at compile-time. However, under the proviso that
 ** the types in a given tuple are somehow _reconcilable_, a dispatcher
 ** table with access functors can be built, so that effectively all fields
 ** can be handled by instances of a generic functor. This setup allows to
 ** define a subscript `operator[]`, so that the suitable functor is picked
 ** with runtime-index and applied to the tuple to yield the indicated field,
 ** albeit converted into a _common type_.
 ** 
 ** While this is certainly a very specific setup, and incurs some cost,
 ** since the functor dispatcher tables must be prepared statically, such
 ** a runtime-controlled index access allows for dynamic and data-driven
 ** processing of tuple_like structures, which are fully type-safe,
 ** compact in storage and easy to nest. Furthermore, run-time
 ** processing of arguments passed to variadic functions
 ** is enabled, providing a new kind of flexible API.
 ** 
 ** @see TupleIdxAdaptor_test
 ** @see FunTrampoline_test
 */


#ifndef LIB_META_TUPLE_IDX_ADAPTOR_H
#define LIB_META_TUPLE_IDX_ADAPTOR_H


#include "include/logging.h"
#include "lib/iter-adapter.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/meta/common-result.hpp"
#include "lib/meta/fun-trampoline.hpp"

#include <string>


namespace lib {
namespace meta {
  
  template<class TUP>
  class TupleIndexAdaptor
    : public TUP
    {
      /** Helper to generate a dispatch table for
       *  this specific instance of tuple element types */
      template<typeseq SEQ>
      struct SubscriptBuilder
        {
          static_assert (reconcilable<SEQ>
                        ,"A common run-time index accessor can only be built "
                         "if all types in the tuple can be reconciled into a "
                         "common type used for the access");
          
          using Tuple   = RefTraits<TUP>::value_type;
          using AddRefs = ElmTypes<SEQ>::template Apply<std::add_lvalue_reference_t>;
          using ResType = CommonResult_t<AddRefs>;
          using Functor = ResType(*)(Tuple&);
          
          template<size_t idx>
          constexpr auto
          build()
            {
              return [](Tuple& tuple) -> ResType
                      {
                        return getElm<idx> (tuple);
                      };
            }
        };
      
      using TypeSeq = ElmTypes<TUP>::Seq;
      using Accessor = FunTrampoline<SubscriptBuilder, TypeSeq>;
      
    public:
      using Tuple   = TUP;
      using ResType  = Accessor::ResType;
      using reference = ResType;
      using value_Type = RefTraits<ResType>::value_type;
      static constexpr bool isRefAccess = isLRef_v<ResType>;
      
      
      /** construct by passing a source tuple */
      template<typename T>
      TupleIndexAdaptor(T&& dataSrc)
        : TUP{forward<T> (dataSrc)}
        { }
      
      ///   lift all constructors of the adapted tuple
      using TUP::TUP;
      
      Tuple      & tuple()       { return *this; }
      Tuple const& tuple() const { return *this; }
      
      operator std::string()  const
        {
          return "iTup" + joinTupleParen (this->tuple());
        }
      
      
      /* === subscriptable === */
      
      static constexpr size_t size() { return Accessor::size(); }
      
      ResType
      operator[] (size_t idx)
        {
          return Accessor::dispatch(idx)(*this);
        }
    };
  
  /// deduction guide
  template<class TUP>
  TupleIndexAdaptor(TUP) -> TupleIndexAdaptor<std::decay_t<TUP>>;
  
  
}}// namespace lib::meta
#endif /*LIB_META_TUPLE_IDX_ADAPTOR_H*/
