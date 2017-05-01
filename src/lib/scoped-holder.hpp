/*
  SCOPED-HOLDER.hpp  -  general purpose wrapper for dealing with ownership problems 

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file scoped-holder.hpp
 ** Some wrappers for coping with ownership problems.
 ** Working with collections of objects, especially in conjunction with
 ** polymorphism, can be challenging when we are bound to care for lifecycle
 ** and ownership for the contained classes. There are several solutions,
 ** including the boost::ptr_container library or lib::ScopedPtrVect, the
 ** use of shared_ptr  or even a garbage collector. Sometimes circumstances
 ** rather call for a very simple or lightweight solution though.
 ** 
 ** ScopedPtrHolder is a simple extension to std::unique_ptr, enabling
 ** to use it within STL containers if we stick to a specific protocol.
 ** The idea is to permit copying as long as the unique_ptr is empty.
 ** This can be used to allow for extension of the STL container on
 ** demand, i.e. to handle the typical situation of a registry which
 ** is initialised lazily, but only released in a controlled fashion.
 ** 
 ** ScopedHolder implements a similar concept for in-place storage of
 ** noncopyable objects within STL containers.
 ** 
 ** While the added copy operations (secured with the "empty" requirement)
 ** are enough to use those holders within fixed sized STL containers,
 ** supporting dynamic growth (like in std::vector#resize() ) additionally
 ** requires a facility to transfer the lifecycle management control between
 ** holder instances. This is the purpose of the \c transfer_control
 ** friend function.
 ** 
 ** @deprecated this is a pre C++11 concept and superseded by rvalue references 
 ** 
 ** @see scoped-holder-test.cpp
 ** @see scoped-holder-transfer.hpp use in std::vector
 ** @see AllocationCluster usage example
 ** @see scoped-ptrvect.hpp simple pointer-holding collection
 */


#ifndef LIB_SCOPED_HOLDER_H
#define LIB_SCOPED_HOLDER_H

#include "lib/error.hpp"

#include <memory>


namespace lib {
  
  
  
  
  /**
   * Extension to std::unique_ptr, allowing copy operations
   * on empty pointers (i.e. contained pointer is null).
   * @throw error::Logic on attempt to copy otherwise
   */
  template<class B>
  class ScopedPtrHolder
    : public std::unique_ptr<B>
    {
      typedef std::unique_ptr<B> _Parent;
      
      static B* must_be_null (_Parent const& ptr)
      {
        if (ptr)
          throw lumiera::error::Logic("ScopedPtrHolder protocol violation: "
                                      "attempt to copy from non-null.");
        return 0;
      }
      
    public:
      ScopedPtrHolder()
        : _Parent{}
        { }
      
      template<class SU>
      explicit ScopedPtrHolder (SU * p)  // never throws
        : _Parent(p)
        { }
      
      template<class SU>
      explicit ScopedPtrHolder (std::unique_ptr<SU> pu)  // never throws
        : _Parent(pu.release())
        { }
      
      ScopedPtrHolder (ScopedPtrHolder const& ref)
        : _Parent(must_be_null (ref))
        { }
      
      ScopedPtrHolder&
      operator= (ScopedPtrHolder const& ref)
        {
          must_be_null (*this);
          must_be_null (ref);
          return *this;
        }
      
      friend void
      transfer_control (ScopedPtrHolder& from, ScopedPtrHolder& to)
      {
        if (!from) return;
        TRACE (test, "transfer_control<ScopedPtrHolder>... from=%p to=%p",&from, &to);
        must_be_null (to);
        to.swap(from);
      }
    };
  
  
  
  
  /**
   * Inline buffer holding and owning an object similar to unique_ptr.
   * Access to the contained object is similar to a smart-pointer,
   * but the object isn't heap allocated, rather placed into an
   * buffer within ScopedHolder. Initially, ScopedHolder is empty
   * and behaves like a null pointer. The contained object must be
   * created explicitly by calling #create() (using the default ctor).
   * This state change is remembered (requiring 1 char of additional 
   * storage). After the creation of the object, ScopedHolder is
   * effectively noncopyable, which is enforced by run-time checks.
   * ScopedHolder may be used to hold noncopyable objects within STL
   * containers inline without extra heap allocation.
   */
  template<class TY>
  class ScopedHolder
    {
      char content_[sizeof(TY)];
      char created_;
      
      
      static char
      must_be_empty (ScopedHolder<TY> const& ref)
      {
        if (ref)
          throw lumiera::error::Logic("ScopedHolder protocol violation: "
                                      "copy operation after having invoked create().");
        return 0;
      }
      
    public:
      ScopedHolder()
        : created_(0)
        { }
      
      ~ScopedHolder() { clear(); }
      
      
      TY& 
      create ()
        {
          ASSERT (!created_);
          TY * obj = new(content_) TY();
          ++created_;
          return *obj;
        }
      
      TY& 
      create (TY const& o)    ///< place new content object using copy ctor
        {
          ASSERT (!created_);
          TY * obj = new(content_) TY(o);
          ++created_;
          return *obj;
        }
      
      void
      clear ()
        {
          if (created_)
            get()->~TY();
          created_ = false;
        }
      
      
      ScopedHolder (ScopedHolder const& ref)
        : created_(must_be_empty (ref))
        { }
      
      ScopedHolder&
      operator= (ScopedHolder const& ref)
        {
          must_be_empty (*this);
          must_be_empty (ref);
          return *this;
        }
        
      
      TY&
      operator* ()  const
        {
          ASSERT (created_);
          return (TY&) content_;
        }
      
      TY* 
      operator-> ()  const
        {
          ASSERT (created_);
          return (TY*) &content_;
        }
      
      TY* get() const     // never throws
        { 
          return (TY*) &content_; 
        }
      
      
      
      explicit operator bool() const { return created_; }
      bool operator! ()        const { return not created_; }
      
      
      friend void
      transfer_control (ScopedHolder& from, ScopedHolder& to)
      {
        if (!from) return;
        TRACE (test, "transfer_control<ScopedHolder>... from=%p to=%p",&from, &to);
        must_be_empty (to);
        to.create();
        try
          {
            transfer_control(*from,*to);  // note: assumed to have no side-effect in case it throws
            from.clear();
            return;
          }
        catch(...)
          {
            to.clear();
            WARN (test, "transfer_control operation aborted.");
            throw;
          }
      }
    };
  
  
  
  
} // namespace lib
#endif
