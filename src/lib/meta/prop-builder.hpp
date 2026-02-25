/*
  PROP-BUILDER.hpp  -  generate data record with arbitrary named fields

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file prop-builder.hpp
 ** A builder for compile-time generation of data records with arbitrary named fields.
 */


#ifndef LIB_META_PROP_BUILDER_H
#define LIB_META_PROP_BUILDER_H


#include "lib/meta/trait.hpp"

#include <utility>


namespace lib {
namespace meta{
  
  using std::move;
  using std::forward;
  
  
  /**
   * Anchor record and builder mechanics for generating data records.
   * @see PropBuilder_test::simpleUsage
   */
  struct PropBuilder
    {
      /**
       * Core builder operation: invoke a builder-λ to extend »this«
       * @param layerBilder a specially crafted invokable, that takes _this object_
       *   and the given property value and creates an instance of a derived class.
       * @note the macro \ref PROP_FIELD below will generate a suitable argument.
       * @param val an arbitrary (movable) data element to be stored as property.
       * @remark C++20 introduced the syntax for _explicit object member functions_,
       *   which allow to bind the "self" object instance to an explicitly defined
       *   first function parameter. Since this builder operation will be invoked
       *   on the generated compound object, the template parameter \a BAS will
       *   denote this compound, and the next builder-λ will derive from it.
       */
      template<class BAS, class LAY, typename VAL>
      auto
      define (this BAS&& chain, LAY const& layerBuilder, VAL&& val)
        {
          return layerBuilder (move(chain), forward<VAL>(val));
        }
    };
  
  
#define PROP_FIELD(FIELD_NAME)        \
  []<class BAS, typename VAL>          \
    (BAS&& bas, VAL&& val)              \
    {                                    \
      using BaseLayer = std::decay_t<BAS>;\
      using Property  = std::decay_t<VAL>; \
                                            \
        struct DataLayer_ ## FIELD_NAME      \
          : BaseLayer                         \
          {                                    \
            Property FIELD_NAME;                \
                                                 \
            DataLayer_ ## FIELD_NAME (BaseLayer b \
                                     ,Property v)  \
              : BaseLayer{move(b)}                  \
              ,FIELD_NAME{move(v)}                   \
              { }                                     \
          };                                           \
      return DataLayer_ ## FIELD_NAME {forward<BAS>(bas)\
                                      ,forward<VAL>(val) \
                                      };                  \
    }
  
  
  
}} // namespace lib::meta
#endif /*LIB_META_PROP_BUILDER_H*/
