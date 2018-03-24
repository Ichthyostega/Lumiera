/*
  THREAD-LOCAL.hpp  -  support using thread local data

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file thread-local.hpp
 ** Helpers for working with thread local data.
 ** As we don't want to depend on boost.threads, we'll provide some simple
 ** support facilities for dealing with thread local data in RAII fashion.
 ** Draft as of 2/10, to be extended on demand.
 ** 
 ** @todo care for unit test coverage
 ** @todo WIP-WIP. Maybe add facilities similar to boost::specific_ptr
 ** @deprecated C++11 has a `thread_local` storage class...
 **/


#ifndef LIB_THREAD_LOCAL_H
#define LIB_THREAD_LOCAL_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"

#include <pthread.h>



namespace lib {
  
  
  /**
   * Thread local pointer without ownership management.
   * This (noncopyable) smart-ptr class cares for registering and
   * deregistering the per-instance access key, but besides that
   * behaves passively, like a normal pointer. When first accessed,
   * the pointer is NIL in each new thread; it may be set by assignment.
   */
  template<typename TAR>
  class ThreadLocalPtr
    : util::NonCopyable
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
      
      explicit operator bool()  const { return isValid(); }
      
      
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
      accessChecked()  const
        {
          TAR *p(get());
          if (!p)
            throw lumiera::error::State ("dereferencing a thread local NULL pointer"
                                        ,lumiera::error::LUMIERA_ERROR_BOTTOM_VALUE);
          return p;
        }
      
    };
  
  
  
} // namespace lib
#endif
