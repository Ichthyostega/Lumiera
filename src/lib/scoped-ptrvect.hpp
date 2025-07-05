/*
  SCOPED-PTRVECT.hpp  -  simple noncopyable lifecycle managing collection of pointers

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file scoped-ptrvect.hpp
 ** Managing lifecycle for a collection of objects. Sometimes we need to
 ** build and own a number of objects, including lifecycle management.
 ** For example, a service provider may need to maintain a number of individual
 ** process handles. The solution here is deliberately kept simple, it is
 ** similar to using a STL container with shared_ptr(s), but behaves rather
 ** like std::unique_ptr.
 ** 
 ** Some details to note:
 ** - contained objects accessed by reference, never NULL.
 ** - the exposed iterator automatically dereferences
 ** - can be move-assigned and move constructed, like `unique_ptr`
 ** 
 ** @warning deliberately **not threadsafe**
 ** @remark This library class provides the same basic functionality as
 **   `boost::ptr_vector`, but doesn't require us to depend on boost-serialisation.
 **   Furthermore, Boost provides several variants of containers and a much more
 **   feature-rich API, also with the ability to clone / move containers;
 **   yet we prefer to leave out most functionality, in favour of adding
 **   a dedicated API with #manage() and #detach() to indicated semantics.
 **   And we value the ability to integrate with Lumiera Forward Iterators.
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
   * Implemented as a non-copyable object, based
   * on a vector of bare pointers.
   */
  template<class T>
  class ScopedPtrVect
    : util::MoveAssign
    {
      using _Vec  = std::vector<T*>;
      using VIter = _Vec::iterator;
      
      using RIter    = RangeIter<VIter>;
      using IterType = PtrDerefIter<RIter>;
      
      using ConstIterType = IterType::ConstIterType;
      using RcIter        = IterType::WrappedConstIterType;
      
      _Vec vec_;
      
    public:
      typedef size_t   size_type;
      typedef T        value_type;
      typedef T &      reference;
      typedef T const& const_reference;
      
      
      
     ~ScopedPtrVect()
        {
          clear();
        }
      
      ScopedPtrVect()
        : vec_{}
        { }
      
      explicit
      ScopedPtrVect (size_type capacity)
        : vec_{}
        {
          vec_.reserve (capacity);
        }
      
      ScopedPtrVect (ScopedPtrVect && src)
        : vec_{std::move (src.vec_)}
        { }
      
      friend void
      swap (ScopedPtrVect& left, ScopedPtrVect& right)
        {
          swap (left.vec_, right.vec_);
        }
      
      ScopedPtrVect&
      operator= (ScopedPtrVect && other)
        {
          swap (*this, other);
          return *this;
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
              vec_.push_back (obj);
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
       */
      T*
      detach (void* objAddress)
        {
          T* extracted = static_cast<T*> (objAddress);
          VIter pos = std::find (vec_.begin(),vec_.end(), extracted);
          if (pos != vec_.end() and *pos != nullptr)
            {
              extracted = *pos;
              vec_.erase(pos);  // EX_STRONG
              return extracted;
            }
          return nullptr;
        }
      
      
      void
      clear()
        {
          for (T*& p : vec_)
            if (p)
              try {
                  delete p;
                  p = nullptr;
                }
              ERROR_LOG_AND_IGNORE (library, "Clean-up of ScopedPtrVect");
          vec_.clear();
        }
      
      
      /* === Element access and iteration === */
      
      T&
      operator[] (size_type i)
        {
          return * get(i);
        }
      
      using iterator       = IterType;
      using const_iterator = ConstIterType;
      
      iterator       begin()       { return       iterator (allPtrs()); }
      iterator       end()         { return       iterator ( RIter() ); }
      const_iterator begin() const { return const_iterator::build_by_cast (allPtrs()); }
      const_iterator end()   const { return const_iterator::nil();      }
      
      
      
      
      /* ====== proxied vector functions ==================== */
      
      size_type  size()      const { return vec_.size();     }
      size_type  max_size()  const { return vec_.max_size(); }
      size_type  capacity()  const { return vec_.capacity(); }
      bool       empty()     const { return vec_.empty();    }
      
      
    private:
      /** @internal element access, including range and null check */
      T*
      get (size_type i)
        {
          T* p (vec_.at (i));
          if (!p)
            throw lumiera::error::Invalid("no valid object at this index");
          else
            return p;
        }
      
      /** @internal access sequence of all managed pointers */
      RIter
      allPtrs()
        {
          return RIter{vec_.begin(), vec_.end()};
        }
      RIter
      allPtrs()  const
        {
          _Vec& elements = util::unConst(this)->vec_;
          return RIter{elements.begin(), elements.end()};
        }
    };
  
  
  
  
} // namespace lib
#endif
