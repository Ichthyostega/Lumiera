/*
  SCOPEDHOLDER.hpp  -  general purpose wrapper for dealing with ownership problems 
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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

/** @file scopedholder.hpp
 ** Some wrappers for coping with ownership problems.
 ** Working with collections of objects, especially in conjunction with
 ** polymorphism, can be challenging when we are bound to care for lifecycle
 ** and ownership for the contained classes. There are several solutions,
 ** including the boost::ptr_container library, the use of shared_ptr
 ** or even a garbage collector. Sometimes the circumstances rather call
 ** for a very simple or lightweight solution though.
 ** 
 ** ScopedPtrHolder is a simple extension to boost::scoped_ptr, enabling
 ** to use it within STL containers if we stick to a specific protocol.
 ** The idea is to permit copying as long as the scoped_ptr is empty.
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
 ** @see scopedholdertest.cpp
 ** @see scopedholdertransfer.hpp use in std::vector
 ** @see AllocationCluster usage example
 */


#ifndef LIB_SCOPEDHOLDER_H
#define LIB_SCOPEDHOLDER_H

#include <boost/scoped_ptr.hpp>

#include "lib/error.hpp"



namespace lib {
  
  
  
  
  /**
   * Extension to boost::scoped_ptr, allowing copy operations
   * on empty pointers (i.e. contained pointer is null).
   * @throw error::Logic on attempt to copy otherwise
   */
  template<class B>
  class ScopedPtrHolder
    : public boost::scoped_ptr<B>
    {
      typedef boost::scoped_ptr<B> _Parent;
      
      static B* must_be_null (_Parent const& ptr)
      {
        if (ptr)
          throw lumiera::error::Logic("ScopedPtrHolder protocol violation: "
                                      "attempt to copy from non-null.");
        return 0;
      }
      
    public:
      ScopedPtrHolder ()
        : _Parent(0)
        { }
      
      template<class SU>
      explicit ScopedPtrHolder (SU * p)  // never throws
        : _Parent(p)
        { }
      
      template<class SU>
      explicit ScopedPtrHolder (std::auto_ptr<SU> p)  // never throws
        : _Parent(p.release())
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
        TRACE (test, "transfer_control<ScopedPtrHolder>... from=%x to=%x",&from, &to);
        must_be_null (to);
        to.swap(from);
      }
    };
  
  
  
  
  /**
   * Inline buffer holding and owning an object similar to scoped_ptr.
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
      
      typedef ScopedHolder<TY> _ThisType;
      
      
      static char must_be_empty (_ThisType const& ref)
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
      
      void
      clear ()
        {
          if (created_)
            get()->~TY();
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
      operator* ()  const  // never throws
        {
          ASSERT (created_);
          return (TY&) content_;
        }
      
      TY* 
      operator-> ()  const // never throws
        {
          ASSERT (created_);
          return (TY*) &content_;
        }
      
      TY* get() const     // never throws
        { 
          return (TY*) &content_; 
        }
      
      
      typedef char _ThisType::*unspecified_bool_type;
      
      /** implicit conversion to "bool" */
      operator unspecified_bool_type()  const // never throws
        {
          return created_?  &_ThisType::created_ : 0;
        }
      
      bool operator! ()  const { return !created_; }
      
      
      friend void
      transfer_control (ScopedHolder& from, ScopedHolder& to)
      {
        if (!from) return;
        TRACE (test, "transfer_control<ScopedHolder>... from=%x to=%x",&from, &to);
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
