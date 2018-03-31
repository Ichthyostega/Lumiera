/*
  DEPENDABLE-BASE.hpp  -  fundamental structures with extended lifespan

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file dependable-base.hpp
 ** Static container to hold basic entities needed during static init and shutdown.
 ** A special implementation of lib::Sync, where the storage of the object monitor
 ** is associated directly to a type rather then to a single object instance. While
 ** being problematic in conjunction with static startup/shutdown, doing so is sometimes
 ** necessary to setup type based dispatcher tables, managing singleton creation etc.
 **
 ** @note simply using the ClassLock may cause a Monitor object (with a mutex) to be
 **       created at static initialisation and destroyed on application shutdown.
 ** @see singleton-factory.hpp usage example
 */


#ifndef LIB_DEPENDABLE_BASE_H
#define LIB_DEPENDABLE_BASE_H

#include "lib/nobug-init.hpp"
#include "lib/sync.hpp"


namespace lib {
  
  namespace nifty { // implementation details
    
    template<class X>
    struct Holder 
      {
        static uint accessed_;
        static char content_[sizeof(X)];
        
        Holder() 
          {
            if (!accessed_)
              new(content_) X();
            ++accessed_; 
          }
        
       ~Holder() 
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
    uint Holder<X>::accessed_;
    
    template<class X>
    char Holder<X>::content_[sizeof(X)];
    
  } // (End) nifty implementation details
  
  
  
  /**
   * A synchronisation protection guard employing a lock scoped
   * to the parameter type as a whole, not an individual instance.
   * After creating an instance, every other access specifying the same
   * type is blocked.
   * @note the Lock is recursive, because several instances within the same
   *       thread may want to acquire it at the same time without deadlock.
   * @note there is a design sloppiness, as two instantiations of the
   *       ClassLock template with differing CONF count as different type.
   *       Actually using two different configurations within for a single
   *       class X should be detected and flagged as error, but actually
   *       just two non-shared lock instances get created silently. Beware! 
   * @see Sync::Lock the usual simple instance-bound variant
   */
  template<class X, class CONF = RecursiveLock_NoWait>
  class ClassLock_WIP 
    : public Sync<CONF>::Lock
    {
      typedef typename Sync<CONF>::Lock Lock;
      typedef typename sync::Monitor<CONF> Monitor;
      
      struct PerClassMonitor : Monitor {};
      
      Monitor&
      getPerClassMonitor()
        {
          static nifty::Holder<PerClassMonitor> __used_here;
          if (1 != use_count())
            {
              ERROR (progress, "AUA %d", use_count()); 
            }
          ASSERT (1==use_count(), "static init broken");
          
          return __used_here.get();
        }
      
    public:
      ClassLock_WIP() : Lock (getPerClassMonitor()) { }
      
      uint use_count() { return nifty::Holder<PerClassMonitor>::accessed_; }
    };
  
  
} // namespace lib
#endif /*LIB_DEPENDABLE_BASE_H*/
