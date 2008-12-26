/*
  SYNC-CLASSLOCK.hpp  -  special case of object based locking tied directly to a type
 
  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>
                        Hermann Vosseler <Ichthyostega@web.de>
 
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

/** @file sync-classlock.hpp
 ** A special implementation of lib::Sync, where the storage of the object monitor
 ** is associated directly to a type rather then to a single object instance. While
 ** being problematic in conjunction with static startup/shutdown, doing so is sometimes
 ** necessary to setup type based dispatcher tables, managing singleton creation etc.
 **
 ** @note simply using the ClassLock may cause a Monitor object (with a mutex) to be
 **       created at static initialisation and destroyed on application shutdown.
 ** @see singletonfactory.hpp usage example
 */


#ifndef LIB_SYNC_CLASSLOCK_H
#define LIB_SYNC_CLASSLOCK_H

#include "lib/sync.hpp"


namespace lib {
  
  namespace { // implementation details
    
    template<class X>
    struct NiftyHolder 
      {
        static uint accessed_;
        static char content_[sizeof(X)];
        
        NiftyHolder() 
          {
            if (!accessed_)
              new(content_) X();
            ++accessed_; 
          }
        
       ~NiftyHolder() 
          {
            --accessed_; 
            if (0==accessed_)
              get().~X();
          }
       
       X&
       get()
        { 
          X* obj = reinterpret_cast<X*> (&content_);
          ASSERT (obj, "Logic of Schwartz counter broken.");
          return *obj;
        }
      };
    
    template<class X>
    uint NiftyHolder<X>::accessed_;
    
    template<class X>
    char NiftyHolder<X>::content_[sizeof(X)];
    
  } // (End) implementation details
  
  
  
  /**
   * A synchronisation protection guard employing a lock scoped
   * to the parameter type as a whole, not an individual instance.
   * After creating an instance, every other access specifying the same
   * type is blocked.
   * @warn beware of recursion when using a nonrecursive Mutex
   * @see Sync::Lock the usual simple instance-bound variant
   */
  template<class X, class CONF = NonrecursiveLock_NoWait>
  class ClassLock 
    : public Sync<CONF>::Lock
    {
      typedef typename Sync<CONF>::Lock Lock;
      typedef typename sync::Monitor<CONF> Monitor;
      
      Monitor&
      getPerClassMonitor()
        {
          static NiftyHolder<Monitor> __used_here;
          return __used_here.get();
        }
      
    public:
      ClassLock() : Lock (getPerClassMonitor()) {}
      
      uint use_count() { return NiftyHolder<Monitor>::accessed_; }
    };
  
  
} // namespace lib
#endif
