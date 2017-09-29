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
 ** @remark the choice of implementation layout is arbitrary and not based on evidence.
 **         A recursive structure with fixed inline storage looked like an interesting programming challenge.
 **         Using just a heap based array storage would have been viable likewise.
 ** @todo when UICoord is in widespread use, collect performance statistics and revisit this design.
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
#include <memory>
#include <array>


namespace lib {
  
//  using std::unique_ptr;
  using std::string;
  using lib::Literal;
  
  namespace { // Implementation helper: flexible heap based extension storage....
    
    /**
     * Heap-allocated extension storage for an immutable sequence of literal strings.
     * The size of the allocation is determined and fixed once, at construction time,
     * derived from the number of initialisers. The first slot within the allocation
     * stores this length. Extension can be _empty_ (default constructed),
     * in which case no heap allocation is performed.
     */
    class Extension
      {
        using PStorage = const char**;
        
        PStorage storage_;
        
        
        static size_t&
        size (PStorage& p)
          {
            REQUIRE (p);
            return reinterpret_cast<size_t&> (p[0]);
          }
        
      public:
        Extension()
          : storage_{nullptr}
          { }
        
        template<typename...ELMS>
        explicit
        Extension (ELMS ...elms)
          : storage_{new const char* [1 + sizeof...(ELMS)]}
          {
            size(storage_) = sizeof...(ELMS);
            storage_[1] = {elms...};
          }
       ~Extension()
          {
            if (storage_)
              delete[] storage_;
          }
      };
  }//(End)Implementation helper
  
  
  
  /**
   * Abstraction for path-like topological coordinates.
   * A sequence of Literal strings, with array-like access and
   * standard iteration. Implemented as fixed size inline tuple
   * with heap allocated unlimited extension space.
   */
  template<size_t chunk_size>
  class PathArray
    {
      using LitArray = std::array<const char*, chunk_size>;
      
      LitArray elms_;
      Extension tail_;
      
    public:
      template<typename...ARGS>
      explicit
      PathArray (ARGS&& ...args)
        : elms_{}
        {
          UNIMPLEMENTED ("initialise path array components");
        }
      
      PathArray (PathArray const& o)
        : elms_(o.elms_)
        {
          UNIMPLEMENTED ("copy construct path array components");
        }
      
      PathArray&
      operator= (PathArray const& r)
        {
          UNIMPLEMENTED ("path array copy assignment");
        }
      
      
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
  
  
  template<size_t chunk_size>
  inline
  PathArray<chunk_size>::operator string()  const
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
