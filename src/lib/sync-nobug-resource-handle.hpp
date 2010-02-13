/*
  SYNC-NOBUG-RESOURCE-HANDLE.hpp  -  supplement: manage storage for NoBug resource handles
 
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

/** @file sync-nobug-resource-handle.hpp
 ** Supplement to sync.hpp: manage the storage for NoBug resource handles.
 ** For resource tracking we need storage for each \em usage of a resource, in order
 ** to provide a user handle for this usage situation. I consider this an internal
 ** detail and want to keep it away from the code concerned with the resource itself
 ** (here the object monitor).
 ** 
 ** @todo experimental solution
 **/


#ifndef LIB_SYNC_NOBUG_RESOURCE_HANDLE_H
#define LIB_SYNC_NOBUG_RESOURCE_HANDLE_H


#include "lib/error.hpp"
#include "lib/bool-checkable.hpp"

#include <boost/noncopyable.hpp>
#include <pthread.h>



namespace lib {
namespace sync{
  
  
  /**
   * Thread local pointer without ownership management.
   * This (noncopyable) smart-ptr class cares for registering and
   * deregistering the per-instance access key, but besides that
   * behaves passively, like a normal pointer. When first accessed,
   * the pointer is NIL in each new thread; it may be set by assignment.
   */
  template<typename TAR>
  class ThreadLocalPtr
    : public BoolCheckable< ThreadLocalPtr<TAR>
    , boost::noncopyable  >
    {
      pthread_key_t key_;
      
    public:
      ThreadLocalPtr()
        {
          if (pthread_key_create (&key_, NULL))
            throw lumiera::error::External ("unable to create key for thread local data");
        }
      
     ~ThreadLocalPtr()
        {
          WARN_IF (pthread_key_delete (key_), sync, "failure to drop thread-local data key");
        }
      
      
      bool isValid()    const { return get(); }
      TAR& operator* ()  const { return *accessChecked(); }
      TAR* operator-> ()  const { return accessChecked(); }
      
      void operator= (TAR& tar) { set(&tar); }
      
      
      TAR*
      get()  const
        {
          return static_cast<TAR*> (pthread_getspecific (key_));
        }
      
      void
      set (TAR* pointee)
        {
          if (pthread_setspecific (key_, pointee))
            throw lumiera::error::External ("failed to store thread local data");
        }
      
    private:
      TAR*
      accessChecked()
        {
          TAR *p(get());
          if (!p)
            throw lumiera::error::State ("dereferencing a thread local NULL pointer"
                                        ,lumiera::error::LUMIERA_ERROR_BOTTOM_VALUE);
          return p;
        }

    };
  
  
    
    
//#ifdef NOBUG_MODE_ALPHA     /////////TODO don't we need the handle in BETA builds for resource logging?
  
  /** 
   * Diagnostic context, housing the NoBug resource tracker handle.
   * Instances of this class should be created on the stack at appropriate scopes.
   * When used in nested scopes, a chain (stack) of contexts is maintained automatically.
   * Client code can access the innermost handle via static API.
   * @warning never store this into global data structures.
   */
  class NobugResourceHandle
    : boost::noncopyable
    {
      typedef nobug_resource_user* Handle;
      typedef ThreadLocalPtr<NobugResourceHandle> ThreadLocalAccess;
      
      Handle handle_;
      NobugResourceHandle * const prev_;
      
      /** embedded thread local pointer
       *  to the innermost context encountered */
      static ThreadLocalAccess&
      current()
        {
          static ThreadLocalAccess accessPoint;
          return accessPoint;
        }
      
      
    public:
      NobugResourceHandle()
        : handle_(0)
        , prev_(current().get())
        {
          current().set (this);
        }
      
     ~NobugResourceHandle()
        {
          ASSERT (this == current().get());
          current().set (prev_);
        }
      
      
      operator Handle* () ///< payload: NoBug resource tracker user handle
        {
          return &handle_;
        }
      
      /** accessing the innermost diagnostic context created */
      static NobugResourceHandle&
      access ()
        {
          NobugResourceHandle* innermost = current().get();
          if (!innermost)
            throw lumiera::error::Logic ("Accessing Diagnostic context out of order; "
                                         "an instance should have been created in "
                                         "an enclosing scope");
          return *innermost;
        }
    };
//#endif
  
  
  
}} // namespace lib::sync
#endif
