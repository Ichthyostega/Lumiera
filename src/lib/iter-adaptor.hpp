/*
  ITER-ADAPTOR.hpp  -  helpers for building simple forward iterators 
 
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

/** @file iter-adaptor.hpp
 ** Helper template(s) for creating <b>lumiera forward iterators</b>.
 ** This denotes a concept similar to STL's  "forward iterator", with
 ** the addition of an bool check to detect iteration end. The latter
 ** is inspired by the \c hasNext() function found in many current
 ** languages supporting iterators. In a similar vein (inspired from
 ** functional programming), we deliberately don't support the various
 ** extended iterator concepts from STL and boost (random access iterators,
 ** output iterators and the like). According to this concept, <i>an iterator
 ** is a promise for pulling values</i> -- and nothing beyond that.
 ** 
 ** @todo WIP WIP WIP
 ** @todo see Ticket #182
 ** 
 ** @see scoped-ptrvect.hpp
 */


#ifndef LIB_ITER_ADAPTOR_H
#define LIB_ITER_ADAPTOR_H


//#include "include/logging.h"
//#include "lib/error.hpp"
//#include "lib/util.hpp"

//#include <vector>
//#include <boost/noncopyable.hpp>


namespace lib {
  
//  using util::for_each;
  
  
  
  /**
   */
  template<class T, class CON>
  class PtrIter
    {
    };
  
  
  /**
   */
  template<class T, class CON>
  class PtrDerefIter
    {
    };
  
  
  
  
} // namespace lib
#endif
