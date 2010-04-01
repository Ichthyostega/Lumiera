/*
  ITER-ADAPTER-STL.hpp  -  helpers for building simple forward iterators 
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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

/** @file iter-adapter-stl.hpp
 ** Preconfigured adapters for some STL container standard usage situations.
 ** Especially, definitions for accessing views on common STL containers
 ** repackaged as <b>lumiera forward iterators</b>. Mostly the purpose
 ** is ease of use, we don't create an abstraction barrier or try to
 ** hide implementation details. (see iter-source.hpp for such an
 ** abstraction facility). As a benefit, these adapters can be 
 ** considered low overhead.
 ** 
 ** @see iter-adapter-stl-test.cpp
 ** @see iter-adapter.hpp
 ** @see iter-source.happ
 ** @see intertools.hpp
 ** 
 */


#ifndef LIB_ITER_ADAPTER_STL_H
#define LIB_ITER_ADAPTER_STL_H


#include "lib/iter-adapter.hpp"

/////////////////////////TODO remove
#include "lib/itertools.hpp"
/////////////////////////TODO remove



namespace lib {
namespace iter_stl {
  
  
  namespace { // traits and helpers...
        
    template<class MAP>
    struct _MapT
      {
        typedef typename MAP::key_type Key;
        typedef typename MAP::value_type::second_type Val;
          typedef typename MAP::iterator    MIter;         ///////////////////////////TODO
          typedef RangeIter<MIter>         SrcRange;
          typedef TransformIter<SrcRange,Key> KeyIter;        ///////////////////////////TODO
          typedef TransformIter<SrcRange,Val> ValIter;        ///////////////////////////TODO
      };
  }
  
  
  /**
   */
  template<class MAP>
  inline typename _MapT<MAP>::KeyIter
  eachKey (MAP)
  {
    UNIMPLEMENTED ("each key of a map as Lumiera Forward Iterator");
  }
  
  
  /**
   */
  template<class MAP>
  inline typename _MapT<MAP>::ValIter
  eachVal (MAP)
  {
    UNIMPLEMENTED ("each value of a map as Lumiera Forward Iterator");
  }
  
  
  /**
   */
  template<class MAP>
  inline typename _MapT<MAP>::KeyIter
  eachDistinctKey (MAP)
  {
    UNIMPLEMENTED ("each distinct key a multimap as Lumiera Forward Iterator");
  }
  
  
}} // namespace lib::iter_stl
#endif
