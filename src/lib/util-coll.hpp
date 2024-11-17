/*
  UTIL-COLL.hpp  -  helpers and convenience shortcuts for working with collections

   Copyright (C)
     2012,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file util-coll.hpp
 ** Some small helpers and convenience shortcuts to ease working with
 ** collections and sequences (given by iterator). Mostly, these are tiny bits of
 ** existing functionality, just packaged in a more fluent and readable way.
 ** - accessors
 **   - util::first() to get the first element
 **   - util::last() to access the last element
 ** - aggregate functions
 **   - util::max() compute the maximum of comparable numbers
 **   - util::min()
 ** 
 ** @warning some functions only available when including itertools.hpp beforehand
 ** 
 ** @see util-collection-test.cpp
 ** @see util-foreach.hpp
 ** 
 */


#ifndef UTIL_COLL_H
#define UTIL_COLL_H

#include "lib/util.hpp"
#include "lib/meta/trait.hpp"

#include <limits>



namespace util {
  
  using lib::meta::enable_if;
  using lib::meta::disable_if;
  
  namespace { // predicates to pick the right implementation
    
    using lib::meta::Yes_t;
    using lib::meta::No_t;
    
    template<typename T>
    struct treat_as_STL_Container
      {
        typedef typename lib::meta::Unwrap<T>::Type TaT;
        
        enum{ value = lib::meta::can_STL_ForEach<TaT>::value
                   &&!lib::meta::can_IterForEach<T>::value
            };
      };
    
    template<typename T>
    struct treat_as_LumieraIterator
      {
        enum{ value = lib::meta::can_IterForEach<T>::value
            };
      };
    
    template<typename T>
    struct can_direct_access_Last
      {
        typedef typename lib::meta::Unwrap<T>::Type TaT;
        
        enum{ value = lib::meta::can_STL_backIteration<TaT>::value
            };
      };
    
    
    template<typename COL>
    inline void
    __ensure_nonempty(COL const& coll)
    {
      if (util::isnil(coll))
        throw lumiera::error::Logic("attempt to access the first element of an empty collection"
                                   ,lumiera::error::LUMIERA_ERROR_BOTTOM_VALUE);
    }
  }
  
  
  
  
  /* === specialisations for STL-like containers and Lumiera Forward Iterators === */
  
  /** access the first element of a STL-like container.
   *  @note the container is taken by \c const& and
   *        the \c const is \em stripped before access.
   */
  template <typename COLL>
  inline                    enable_if< treat_as_STL_Container<COLL>,
  typename COLL::reference  >
  first (COLL const& coll)
  {
    using lib::meta::unwrap;
    
    __ensure_nonempty(coll);
    return *(unwrap(coll).begin());
  }
  
  
  /** access the last element of a STL-like container.
   *  @note the container is taken by \c const& and
   *        the \c const is \em stripped before access.
   */
  template <typename COLL>
  inline                    enable_if< can_direct_access_Last<COLL>,
  typename COLL::reference  >
  last (COLL const& coll)
  {
    using lib::meta::unwrap;
    
    __ensure_nonempty(coll);
    return *(unwrap(coll).rbegin());
  }
  
  
  
  /** extract the first element yielded by an Lumiera Forward Iterator.
   * @warning the iterator is modified.
   */
  template <typename IT>
  inline                    enable_if< treat_as_LumieraIterator<IT>,
  typename IT::reference    >
  first (IT ii)
  {
    __ensure_nonempty(ii);
    return *ii;
  }
  
  
#ifdef LIB_ITERTOOLS_H
  /** extract the last element yielded by an Lumiera Forward Iterator.
   * @warning the iterator is extracted until exhaustion (linear complexity).
   * @note returning by-value, contrary to the other tools in this suite
   * @note only available when including itertools.hpp beforehand
   */
  template <typename IT>
  inline                    enable_if< treat_as_LumieraIterator<IT>,
  typename IT::value_type   >
  last (IT ii)
  {
    __ensure_nonempty(ii);
    return lib::pull_last (ii);
  }
#endif
  
  
  
  /* === generic container helpers === */
  
  template<class IT>
  inline auto
  max (IT&& elms)
  {
    using Val = typename std::remove_reference_t<IT>::value_type;
    Val res = std::numeric_limits<Val>::min();
    for (auto const& elm : std::forward<IT> (elms))
      if (elm > res)
        res = elm;
    return res;
  }
  
  template<class CON>
  inline auto
  max (CON const& elms)
  {
    using Val = typename std::remove_reference_t<CON>::value_type;
    Val res = std::numeric_limits<Val>::min();
    for (auto const& elm : elms)
      if (elm > res)
        res = elm;
    return res;
  }
  
  
  template<class IT>
  inline auto
  min (IT&& elms)
  {
    using Val = typename std::remove_reference_t<IT>::value_type;
    Val res = std::numeric_limits<Val>::max();
    for (auto const& elm : std::forward<IT> (elms))
      if (elm < res)
        res = elm;
    return res;
  }
  
  template<class CON>
  inline auto
  min (CON const& elms)
  {
    using Val = typename std::remove_reference_t<CON>::value_type;
    Val res = std::numeric_limits<Val>::max();
    for (auto const& elm : elms)
      if (elm < res)
        res = elm;
    return res;
  }
  
  
  
} // namespace util
#endif /*UTIL_COLL_H*/
