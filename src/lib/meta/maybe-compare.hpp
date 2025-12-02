/*
  MAYBE-COMPARE.hpp  -  guarded invocation of comparisons

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file maybe-compare.hpp
 ** helpers for fail-safe invocation of comparison operations from generic code.
 */


#ifndef LIB_META_MAYBE_COMPARE_H
#define LIB_META_MAYBE_COMPARE_H




namespace lib {
namespace meta{
  
  
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
  
  /* == add specialisations here == */
  
  
  template<typename X>
  inline bool
  equals_safeInvoke (X const& x1, X const& x2)
  {
    return Comparator<X>::equals(x1,x2);
  }
  
  
}} // namespace lib::meta
#endif
