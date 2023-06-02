/*
  HANDLE.hpp  -  opaque handle to an implementation entity, automatically managing lifecycle

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

/** @file handle.hpp
 ** A generic opaque handle to an implementation entity, including lifecycle management.
 ** The intended use is for some public interface to return such a handle to track the
 ** lifecycle or registration of a dedicated service created for this call. The handle
 ** is implemented as refcounting smart-ptr on top of shared_ptr, which especially
 ** means for the client code that the handle has value semantics, can be copied and
 ** stored, while the referred service will stay alive as long as there is still an
 ** handle in use. A handle may be empty ("null handle") or closed; the latter
 ** also decreases the ref count and can be used to close a service explicitly.
 ** 
 ** Possibly a service may use an extension of the handle template carrying dedicated
 ** API functions. In this case, the handle acts like a PImpl; the actual definition
 ** of the implementation class the handle points at is necessary only in the
 ** translation unit implementing such an extended handle.
 **
 ** @see steam::control::Command usage example
 ** 
 */


#ifndef LIB_HANDLE_H
#define LIB_HANDLE_H

#include "lib/nobug-init.hpp"

#include <memory>
#include <utility>


namespace lib {
  
  using std::shared_ptr;
  using std::unique_ptr;
  using std::weak_ptr;
  using std::move;
  
  
  
  /**
   * Generic opaque reference counting handle, for accessing a service
   * and managing its lifecycle. Usually such a handle is created by
   * an service interface and \link #activate activated \endlink by
   * setting up the link to a suitable hidden implementation object.
   * This setup can only be done by a friend or derived class,       //////////////////////////TODO: that was the intention. Why didn't this work out as expected?
   * while client code is free to copy and store handle objects.
   * Finally, any handle can be closed, thereby decrementing
   * the use count.
   */
  template<class IMP>
  class Handle
    {
    protected:
      typedef std::shared_ptr<IMP> SmPtr;
      
      SmPtr smPtr_;
      
    public:
      
      /** by default create an Null handle.
       *  Typically this is followed by activating
       *  the handle by the managing service.
       */
      Handle ( )
      : smPtr_()
      { }
      
                                 Handle (Handle const& r)          = default;
                                 Handle (Handle && rr)             = default;
      template<class Y> explicit Handle (shared_ptr<Y> const& r)   : smPtr_{r}         { }
      template<class Y> explicit Handle (shared_ptr<Y> && srr)     : smPtr_{move(srr)} { }
      template<class Y> explicit Handle (weak_ptr<Y> const& wr)    : smPtr_{wr}        { }
      template<class Y> explicit Handle (unique_ptr<Y> && urr)     : smPtr_{move(urr)} { }
      
                        Handle& operator=(Handle const& r)         = default;
                        Handle& operator=(Handle && rr)            = default;
      template<class Y> Handle& operator=(shared_ptr<Y> const& sr) { smPtr_ = sr;        return *this; }
      template<class Y> Handle& operator=(shared_ptr<Y> && srr)    { smPtr_ = move(srr); return *this; }
      template<class Y> Handle& operator=(unique_ptr<Y> && urr)    { smPtr_ = move(urr); return *this; }
      
      
      explicit operator bool()  const { return bool(smPtr_); }
      bool isValid()            const { return bool(smPtr_); }
      
      
      
      /** Activation of the handle by the managing service.
       *  @param impl the implementation object this handle is tied to
       *  @param whenDead functor to be invoked when reaching end-of-life
       *  @throw std::bad_alloc, in which case \c whenDead(impl) is invoked
       */
      template<typename DEL>
      Handle&
      activate (IMP* impl, DEL whenDead)
        {
          smPtr_.reset (impl, whenDead);
          return *this;
        }
      
      /** another way of activating a handle by sharing ownership
       *  with an existing smart-ptr */
      Handle&
      activate(shared_ptr<IMP> const& impl)
        {
          smPtr_ = impl;
          return *this;
        }
      
      Handle&
      activate(shared_ptr<IMP> && impl)
        {
          smPtr_ = move (impl);
          return *this;
        }
      
      /** deactivate this handle, so it isn't tied any longer
       *  to the associated implementation or service object.
       *  When all handles have either been deactivated or
       *  went out of scope, the associated implementation
       *  reaches end-of-life.
       */
      void close ()
        {
          smPtr_.reset();
        }
      
      
      
    protected:
      IMP&
      impl()  const
        {
          REQUIRE (smPtr_.get(), "Lifecycle-Error");
          return *(smPtr_.get());
        }
    };
  
  
} // namespace lib
#endif
