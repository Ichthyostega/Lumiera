/*
  MAYBE-COMPARE.hpp  -  guarded invocation of comparisons

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file maybe-compare.hpp
 ** helpers for fail-safe invocation of comparison operations from generic code.
 */


#ifndef LIB_META_MAYBE_COMPARE_H
#define LIB_META_MAYBE_COMPARE_H


#include "lib/functor-util.hpp"

#include <functional>


namespace lib {
namespace meta{
  
  using std::function;
  
  /**
   * Trait template for invoking equality comparison.
   * This allows to treat some types specifically....
   */
  template<typename X>
  struct Comparator
    {
      static bool
      equals (X const& x1, X const& x2)
        {
          return x1 == x2;
        }
    };
  
  /** while the boost function implementation doesn't provide comparison,
   *  we'll use our private hack, which at least detects equivalence 
   *  in \em some cases... */
  template<typename SIG>
  struct Comparator<function<SIG>>
    {
      static bool
      equals (function<SIG> const& f1, function<SIG> const& f2)
        { 
          return util::rawComparison(f1,f2);
        }
        
    };
  
  
  template<typename X>
  inline bool
  equals_safeInvoke (X const& x1, X const& x2)
  {
    return Comparator<X>::equals(x1,x2);
  }
  
  
}} // namespace lib::meta
#endif
