/*
  SCOPED-COLLECTION.hpp  -  managing a fixed collection of noncopyable polymorphic objects 

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file scoped-collection.hpp
 ** Managing a collection of noncopyable polymorphic objects in compact storage.
 ** This helper supports the frequently encountered situation where a service
 ** implementation internally manages a collection of implementation related
 ** sub-components with reference semantics. Typically, those objects are
 ** being used polymorphically, and often they are also added step by step.
 ** The storage holding all those child objects is allocated in one chunk
 ** and never adjusted.
 ** 
 ** - TODO: retro-fit with RefArray interface
 ** 
 ** @see ScopedCollection_test
 ** @see scoped-ptrvect.hpp quite similar, but using individual heap pointers
 */


#ifndef LIB_SCOPED_COLLECTION_H
#define LIB_SCOPED_COLLECTION_H


//#include "include/logging.h"
//#include "lib/iter-adapter.hpp"
#include "lib/error.hpp"
//#include "lib/util.hpp"

//#include <vector>
//#include <algorithm>
#include <boost/noncopyable.hpp>


namespace lib {
  
  
  
  /**
   * A fixed collection of noncopyable polymorphic objects.
   * All child objects reside in a common chunk of storage
   * and are owned and managed by this collection holder.
   * Array style access and iteration.
   */
  template<class T>
  class ScopedCollection
    : boost::noncopyable
    {
//    typedef std::vector<T*> _Vec;
//    typedef typename _Vec::iterator VIter;
//    
//    typedef RangeIter<VIter> RIter;
//    typedef PtrDerefIter<RIter> IterType;
//    
//    typedef typename IterType::ConstIterType ConstIterType;
//    typedef typename IterType::WrappedConstIterType RcIter;
      
      
    public:
      typedef size_t   size_type;
//    typedef T &      reference;
//    typedef T const& const_reference;
      
      
      
      ScopedCollection ()
        { }
      
      explicit
      ScopedCollection (size_type capacity)
        {
          UNIMPLEMENTED ("prepare scoped collection storage");
        }
      
     ~ScopedCollection ()
        { 
          clear();
        }
      
      
      
      
      void
      clear()
        {
          UNIMPLEMENTED ("drop all content objects");
        }
      
      
      /* === Element access and iteration === */
      
      T&
      operator[] (size_type i)
        {
          return *get(i);
        }
      
//    typedef IterType      iterator;
//    typedef ConstIterType const_iterator;
//    
//    iterator       begin()        { return       iterator (allPtrs()); }
//    iterator       end()          { return       iterator ( RIter() ); }
//    const_iterator begin()  const { return const_iterator::build_by_cast (allPtrs()); }
//    const_iterator end()    const { return const_iterator::nil();      }
      
      
      
      
      /* ====== proxied vector functions ==================== */
      
//    size_type  size ()      const  { return _Vec::size();     }
//    size_type  max_size ()  const  { return _Vec::max_size(); }
//    size_type  capacity ()  const  { return _Vec::capacity(); }
//    bool       empty ()     const  { return _Vec::empty();    }
      
      
    private:
      /** @internal element access, including range and null check */
      T*
      get (size_type i)
        {
          UNIMPLEMENTED("raw element access");
        }
      
//    /** @internal access sequence of all managed pointers */
//    RIter
//    allPtrs ()
//      {
//        return RIter (_Vec::begin(), _Vec::end());
//      }
//    RIter
//    allPtrs ()  const
//      {
//        _Vec& elements = util::unConst(*this);
//        return RIter (elements.begin(), elements.end());
//      }
    };
  
  
  
  
} // namespace lib
#endif
