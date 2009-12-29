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

#include <algorithm>



namespace util {
  
  
  
  /** shortcut for operating on all elements of a container.
   *  Isn't this already defined somewhere? It's so obvious..
   */
  template <typename Container, typename Oper>
  inline Oper
  for_each (Container& c, Oper doIt)
  {
    return std::for_each (c.begin(),c.end(), doIt);
  }
  
  
  /** All quantification: check if all elements of a collection
   *  satisfy the given predicate. Actually a short-circuit
   *  evaluation is performed.
   */
  template <typename SEQ, typename Oper>
  inline bool
  and_all (SEQ& coll, Oper predicate)
  {
    typename SEQ::const_iterator e = coll.end();
    typename SEQ::const_iterator i = coll.begin();
    
    for ( ; i!=e; ++i )
      if (!predicate(*i))
        return false;
    
    return true;
  }
  
  
  /** Existential quantification: check if any element
   *  of a collection satisfies the given predicate.
   *  Actually, a short-circuit evaluation is performed.
   */
  template <typename SEQ, typename Oper>
  inline bool
  has_any (SEQ& coll, Oper predicate)
  {
    typename SEQ::const_iterator e = coll.end();
    typename SEQ::const_iterator i = coll.begin();
    
    for ( ; i!=e; ++i )
      if (predicate(*i))
        return true;
    
    return false;
  }
  
  
} // namespace util
#endif /*UTIL_FOREACH_H*/
