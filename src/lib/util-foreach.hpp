/*
  UTIL-FOREACH.hpp  -  helpers for doing something for each element
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/


#ifndef UTIL_FOREACH_H
#define UTIL_FOREACH_H

#include "lib/util.hpp"
#include "lib/meta/trait.hpp"

#include <boost/utility/enable_if.hpp>
#include <algorithm>



namespace util {
  
  using boost::enable_if;
  using boost::disable_if;
  
  using lib::meta::can_STL_ForEach;
  using lib::meta::can_IterForEach;
  
  
  
  /** All quantification: check if all elements of a collection
   *  satisfy the given predicate. Actually a short-circuit
   *  evaluation is performed.
   */
  template <typename IT, typename FUN>
  inline bool
  and_all (IT i, IT end, FUN predicate)
  {
    for ( ; i!=end; ++i )
      if (!predicate(*i))
        return false;
    
    return true;
  }
  
  
  /** Existential quantification: check if any element
   *  of a collection satisfies the given predicate.
   *  Actually, a short-circuit evaluation is performed.
   */
  template <typename IT, typename FUN>
  inline bool
  has_any (IT i, IT end, FUN predicate)
  {
    for ( ; i!=end; ++i )
      if (predicate(*i))
        return true;
    
    return false;
  }
  
  
  /* === specialisations for STL containers and Lumiera Forward Iterators === */
  
  /** shortcut for operating on all elements of a STL container. */
  template <typename Container
           ,typename FUN
           >
  inline   typename disable_if< can_IterForEach<Container>,
  FUN      >::type
  for_each (Container& c, FUN doIt)
  {
    return std::for_each (c.begin(),c.end(), doIt);
  }
  
  
  
  template <typename Container
           ,typename FUN
           >
  inline   typename enable_if< can_STL_ForEach<Container>,
  bool     >::type                                        
  and_all (Container& coll, FUN predicate)
  {
    return and_all (coll.begin(),coll.end(), predicate);
  }
  
  
  template <typename Container
           ,typename FUN
           >
  inline   typename enable_if< can_STL_ForEach<Container>,
  bool     >::type                                        
  has_any (Container& coll, FUN predicate)
  {
    return has_any (coll.begin(),coll.end(), predicate);
  }
  
  
} // namespace util
#endif /*UTIL_FOREACH_H*/
