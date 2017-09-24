/*
  PATH-ARRAY.hpp  -  sequence of path-like component-IDs in fixed storage

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file path-array.hpp
 ** Foundation abstraction to implement path-like component sequences.
 ** This library class can be used to build a path abstraction for data structure access
 ** or some similar topological coordinate system, like e.g. [UI coordinates](\ref ui-coord.hpp)
 ** A PathArray is an iterable sequence of literal component IDs, implemented as tuple of `Literal*`
 ** held in fixed inline storage with possible heap allocated (and thus unlimited) extension storage.
 ** It offers range checks, standard iteration and array-like indexed component access; as a whole
 ** it is copyable, while actual components are immutable after construction.
 ** 
 ** @todo WIP 9/2017 first draft ////////////////////////////////////////////////////////////////////////////TICKET #1106 generic UI coordinate system
 ** 
 ** @see PathArray_test
 ** @see UICoord_test
 ** @see gui::interact::UICoord
 ** @see view-spec-dsl.hpp
 */


#ifndef LIB_PATH_ARRAY_H
#define LIB_PATH_ARRAY_H

#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/iter-adapter.hpp"

//#include <boost/noncopyable.hpp>
#include <string>
//#include <memory>


namespace lib {
  
//  using std::unique_ptr;
  using std::string;
  using lib::Literal;
  
//  class GlobalCtx;
  
  
  
  /**
   * @internal Base abstraction for path-like topological coordinates.
   */
  class PathArray
    {
    public:
      template<typename...ARGS>
      explicit
      PathArray (ARGS&& ...args)
        {
          UNIMPLEMENTED ("initialise path array components");
        }
      
      // standard copy operations acceptable
      
      
      size_t
      size()  const
        {
          UNIMPLEMENTED ("path implementation storage");
        }
      
      bool
      empty()  const
        {
          UNIMPLEMENTED ("path implementation storage");
        }
      
      operator string()  const;
      
      
      Literal
      operator[] (size_t idx)
        {
          UNIMPLEMENTED ("path implementation storage");
        }
      
      
    protected:  /* ==== Iteration control API for IterAdapter ==== */
      
      /** Implementation of Iteration-logic: pull next element. */
      friend void
      iterNext (const PathArray* src, Literal* pos)
        {
          ++pos;
          checkPoint (src,pos);
        }
      
      /** Implementation of Iteration-logic: detect iteration end. */
      friend bool
      checkPoint (const PathArray* src, Literal* pos)
        {
          REQUIRE (src);
          if ((pos != nullptr) && (pos != src->storage_end()))
            return true;
          else
            {
              pos = nullptr;
              return false;
        }   }
      
    public:
      using iterator = lib::IterAdapter<Literal*, PathArray*>;
      using const_iterator = iterator;
      
      iterator begin()  const { UNIMPLEMENTED ("content iteration"); }
      iterator end()    const { UNIMPLEMENTED ("content iteration"); }
      
      friend iterator begin(PathArray const& pa) { return pa.begin();}
      friend iterator end  (PathArray const& pa) { return pa.end();  }
      
    private:
      Literal*
      storage_end()  const
        {
          UNIMPLEMENTED ("path implementation storage");
        }
    };
  
  
  inline
  PathArray::operator string()  const
  {
    if (this->empty()) return "";
    
    string buff;
    size_t expectedLen = this->size() * 10;
    buff.reserve (expectedLen);
    for (Literal elm : *this)
      buff += elm + "/";
    
    // chop off last delimiter
    size_t len = buff.length();
    ASSERT (len >= 1);
    buff.resize(len-1);
    return buff;
  }
  
  
  
}// namespace lib
#endif /*LIB_PATH_ARRAY_H*/
