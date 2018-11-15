/*
  SCOPED-PTRVECT.hpp  -  simple noncopyable lifecycle managing collection of pointers 

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

/** @file scoped-ptrvect.hpp
 ** Managing lifecycle for a collection of objects. Sometimes we need to
 ** build and own a number of objects, including lifecycle management. 
 ** For example, a service provider may need to maintain a number of individual
 ** process handles. The solution here is deliberately kept simple, it is
 ** similar to using a STL container with shared_ptr(s), but behaves rather
 ** like std::unique_ptr. It provides the same basic functionality as
 ** boost::ptr_vector, but doesn't require us to depend on boost-serialisation.
 ** 
 ** Some details to note:
 ** - contained objects accessed by reference, never NULL.
 ** - the exposed iterator automatically dereferences
 ** - TODO: detaching of objects...
 ** - TODO: retro-fit with RefArray interface
 ** 
 ** @warning deliberately \em not threadsafe
 ** 
 ** @see scoped-ptrvect-test.cpp
 ** @see scoped-holder.hpp
 ** @see stage::DisplayService usage example
 */


#ifndef LIB_SCOPED_PTRVECT_H
#define LIB_SCOPED_PTRVECT_H


#include "include/logging.h"
#include "lib/iter-adapter-ptr-deref.hpp"
#include "lib/nocopy.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"

#include <vector>
#include <algorithm>


namespace lib {
  
  
  
  /**
   * Simple vector based collection of pointers,
   * managing lifecycle of the pointed-to objects.
   * Implemented as a non-copyable object, based on a
   * vector of bare pointers (private inheritance)
   */
  template<class T>
  class ScopedPtrVect
    : std::vector<T*>,
      util::NonCopyable
    {
      typedef std::vector<T*> _Vec;
      typedef typename _Vec::iterator VIter;
      
      typedef RangeIter<VIter> RIter;
      typedef PtrDerefIter<RIter> IterType;
      
      typedef typename IterType::ConstIterType ConstIterType;
      typedef typename IterType::WrappedConstIterType RcIter;
      
      
    public:
      typedef size_t   size_type;
      typedef T        value_type;
      typedef T &      reference;
      typedef T const& const_reference;
      
      
      
      ScopedPtrVect ()
        : _Vec()
        { }
      
      explicit
      ScopedPtrVect (size_type capacity)
        : _Vec()
        {
          _Vec::reserve (capacity);
        }
      
     ~ScopedPtrVect ()
        { 
          clear();
        }
      
      
      /** take ownership of the given object,
       *  adding it at the end of the collection
       *  @note object is deleted in case of any
       *        problem while adding it
       */
      T&
      manage (T* obj)
        {
          REQUIRE (obj);
          try 
            {
              this->push_back (obj);
              return *obj;
            }
          catch(...)
            {
              delete obj;
              throw;
        }   }
      
      
      /** withdraw responsibility for a specific object.
       *  This object will be removed form this collection
       *  and returned as-is; it won't be deleted when the
       *  ScopedPtrVect goes out of scope.
       * @param obj address of the object in question.
       * @return pointer to the object, if found.
       *         Otherwise, NULL will be returned and the
       *         collection of managed objects remains unaltered
       * @note EX_STRONG
       * @todo TICKET #856 better return a Maybe<T&> instead of a pointer?
       */
      T*
      detach (void* objAddress)
        {
          T* extracted = static_cast<T*> (objAddress);
          VIter pos = std::find (_Vec::begin(),_Vec::end(), extracted);
          if (pos != _Vec::end() && bool(*pos))
            {
              extracted = *pos;
              _Vec::erase(pos);  // EX_STRONG
              return extracted;
            }
          return NULL;
        }
      
      
      void
      clear()
        { 
          VIter e = _Vec::end();
          for (VIter i = _Vec::begin(); i!=e; ++i)
            if (*i)
              try {
                  delete *i;
                  *i = 0;
                }
              ERROR_LOG_AND_IGNORE (library, "Clean-up of ScopedPtrVect");
          _Vec::clear();
        }
      
      
      /* === Element access and iteration === */
      
      T&
      operator[] (size_type i)
        {
          return *get(i);
        }
      
      typedef IterType      iterator;
      typedef ConstIterType const_iterator;
      
      iterator       begin()        { return       iterator (allPtrs()); }
      iterator       end()          { return       iterator ( RIter() ); }
      const_iterator begin()  const { return const_iterator::build_by_cast (allPtrs()); }
      const_iterator end()    const { return const_iterator::nil();      }
      
      
      
      
      /* ====== proxied vector functions ==================== */
      
      size_type  size ()      const  { return _Vec::size();     }
      size_type  max_size ()  const  { return _Vec::max_size(); }
      size_type  capacity ()  const  { return _Vec::capacity(); }
      bool       empty ()     const  { return _Vec::empty();    }
      
      
    private:
      /** @internal element access, including range and null check */
      T*
      get (size_type i)
        {
          T* p (_Vec::at (i));
          if (!p)
            throw lumiera::error::Invalid("no valid object at this index");
          else
            return p;
        }
      
      /** @internal access sequence of all managed pointers */
      RIter
      allPtrs ()
        {
          return RIter (_Vec::begin(), _Vec::end());
        }
      RIter
      allPtrs ()  const
        {
          _Vec& elements = util::unConst(*this);
          return RIter (elements.begin(), elements.end());
        }
    };
  
  
  
  
} // namespace lib
#endif
