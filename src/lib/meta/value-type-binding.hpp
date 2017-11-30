/*
  VALUE-TYPE-BINDING.hpp  -  control type variations for IterAdapter

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

/** @file value-type-binding.hpp
 ** Type re-binding helper template for IterAdapter and friends.
 ** This header defines a trait template which is used by the Iterator
 ** adapters to figure out the value-, pointer- and reference types
 ** when wrapping iterators or containers. For extending the use of
 ** Iterator adapters and Iter-tools to situations involving const
 ** iterators or custom containers, explicit specialisations
 ** might be injected prior to instantiating the Iterator adapter
 ** template.
 ** 
 ** @see iter-adapter.hpp
 ** @see scope-path.hpp usage example (explicit specialisation)
 */


#ifndef LIB_META_VALUE_TYPE_BINDING_H
#define LIB_META_VALUE_TYPE_BINDING_H


#include "lib/error.hpp"
#include "lib/meta/duck-detector.hpp"  ////////TODO
#include "lib/meta/util.hpp"           ////////TODO



namespace std {
  template<typename _Tp>
  class shared_ptr;
}

namespace lib {
namespace meta {
  
  META_DETECT_NESTED (value_type);
  
  /** 
   * Type re-binding helper template for creating nested typedefs
   * for use by IterAdapter or similar "Lumiera Forward Iterators".
   * This trait provides a value-, reference- and pointer type,
   * similar to what the STL does.
   * @note client code might define specialisations
   *       to handle tricky situations (like const_reverse_iter)
   */
  template<typename TY, typename SEL =void>
  struct TypeBinding
    {
      typedef TY value_type;
      typedef TY& reference;
      typedef TY* pointer;
    };
  
  template<typename TY>
  struct TypeBinding<TY *>
    {
      typedef TY value_type;
      typedef TY& reference;
      typedef TY* pointer;
    };
  
  template<typename TY>
  struct TypeBinding<const TY *>
    {
      typedef TY value_type;
      typedef const TY& reference;
      typedef const TY* pointer;
    };
  
  template<typename TY>
  struct TypeBinding<TY &>
    {
      typedef TY value_type;
      typedef TY& reference;
      typedef TY* pointer;
    };
  
  template<typename TY>
  struct TypeBinding<TY const&>
    {
      typedef TY value_type;
      typedef const TY& reference;
      typedef const TY* pointer;
    };
  
  /**
   * specialisation for classes providing
   * STL style type binding definitions themselves
   */
  template<typename TY>
  struct TypeBinding<TY,                enable_if<HasNested_value_type<TY>> >
    {
      typedef typename TY::pointer pointer;
      typedef typename TY::reference reference;
      typedef typename TY::value_type value_type;
    };
  
  
  
}} // namespace lib::meta
#endif /*LIB_META_VALUE_TYPE_BINDING_H*/
