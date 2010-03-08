/*
  SCOPED-PTRVECT.hpp  -  simple noncopyable lifecycle managing collection of pointers 
 
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

/** @file scoped-ptrvect.hpp
 ** Managing lifecycle for a collection of objects. Sometimes we need to
 ** build and own a number of objects, including lifecycle management. 
 ** For example, a service provider may need to maintain a number of individual
 ** process handles. The solution here is deliberately kept simple, it is
 ** similar to using a STL container with shared_ptr(s), but behaves rather
 ** like boost::scoped_ptr. It provides the same basic functionality as
 ** boost::ptr_vector, but doesn't require us to depend on boost-serialisation.
 ** 
 ** Some details to note:
 ** - contained objects accessed by reference, never NULL.
 ** - the exposed iterator automatically dereferences
 ** - TODO: detaching of objects...
 ** - TODO: retro-fit with RefArray interface
 ** 
 ** @see scoped-ptrvect-test.cpp
 ** @see scoped-holder.hpp
 ** @see gui::DisplayService usage example
 */


#ifndef LIB_SCOPED_PTRVECT_H
#define LIB_SCOPED_PTRVECT_H


#include "include/logging.h"
#include "lib/iter-adapter.hpp"
#include "lib/error.hpp"

#include <vector>
#include <boost/noncopyable.hpp>


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
      boost::noncopyable
    {
      typedef std::vector<T*> _Vec;
      typedef typename _Vec::iterator VIter;
      
      typedef RangeIter<VIter> RIter;
      typedef PtrDerefIter<RIter> IterType;
      
      typedef typename IterType::ConstIterType ConstIterType;
      typedef typename IterType::WrappedConstIterType RcIter;
      
      
    public:
      typedef size_t   size_type;
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
              push_back (obj);
              return *obj;
            }
          catch(...)
            {
              delete obj;
              throw;
        }   }
      
      
      void
      clear()
        { 
          VIter e = _Vec::end();
          for (VIter i = _Vec::begin(); i!=e; ++i)
            {
              if (*i)
                try
                  {
                    delete *i;
                    *i = 0;
                  }
                catch(std::exception& ex)
                  {
                    WARN (library, "Problem while deallocating ScopedPtrVect: %s", ex.what());
            }     }
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
      const_iterator begin()  const { return const_iterator (allPtrs()); }
      iterator       end()          { return       iterator ( RIter() ); }
      const_iterator end()    const { return const_iterator (RcIter() ); }
      
      
      
      
      /* ====== proxied vector functions ==================== */
      
      size_type  size ()      const  { return _Vec::size();     }
      size_type  max_size ()  const  { return _Vec::max_size(); }
      size_type  capacity ()  const  { return _Vec::capacity(); }
      bool       empty ()     const  { return _Vec::empty();    }
      
      
    private:
      /** @internal element access, including null check */
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
    };
  
  
  
  
} // namespace lib
#endif
