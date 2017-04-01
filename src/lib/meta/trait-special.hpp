/*
  TRAIT-SPECIAL.hpp  -  supplement, not so commonly used definitions and specialisations

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


/** @file trait-special.hpp
 ** Rarely used specialisations of otherwise heavily used generic code.
 ** Over time, we've established a set of tiny (meta)programming helpers, used
 ** pervasively all over the code base. Such widely used (header) includes incur
 ** the danger to increase compilation time and code size, and thus we spent some
 ** effort on optimising the dependencies of these headers. Typically, such
 ** analyse reveals some obscure and rare extension, which, while used occasionally,
 ** maybe even only once, would cause some increase in the "include size profile".
 ** 
 ** The obvious remedy is to move those into a separate header.
 */


#ifndef LIB_META_TRAIT_SPECIAL_H
#define LIB_META_TRAIT_SPECIAL_H


#include "lib/meta/trait.hpp"

#include <unordered_map>


namespace lib {
namespace meta {
  
  
  /** the std hashtable(s) are notoriously difficult to handle
   *  when it comes to detecting capabilities by metaprogramming,
   *  because the're built up from a generic baseclass and especially
   *  inherit their iterator-types as template specialisations.
   *  
   *  Thus we provide fixed diagnostics to bypass the traits check.
   */
  template<typename KEY, typename VAL, typename HASH>
  struct can_STL_ForEach<std::unordered_map<KEY,VAL,HASH>>
    {
      enum { value = true };
    };
  
  
  
}} // namespace lib::meta
#endif
