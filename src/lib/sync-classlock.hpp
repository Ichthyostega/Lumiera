/*
  SYNC-CLASSLOCK.hpp  -  special case of object based locking tied directly to a type

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>
                        Hermann Vosseler <Ichthyostega@web.de>

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

/** @file sync-classlock.hpp
 ** A special implementation of lib::Sync, where the storage of the object monitor
 ** is associated directly to a type rather then to a single object instance. While
 ** being problematic in conjunction with static startup/shutdown, doing so is sometimes
 ** necessary to setup type based dispatcher tables, managing singleton creation etc.
 **
 ** @note simply using the ClassLock may cause a Monitor object (with a mutex) to be
 **       created at static initialisation and destroyed on application shutdown.
 ** @see depend.hpp usage example
 ** @see SyncClasslock_test
 */


#ifndef LIB_SYNC_CLASSLOCK_H
#define LIB_SYNC_CLASSLOCK_H

#include "lib/nobug-init.hpp"
#include "lib/zombie-check.hpp"
#include "lib/meta/util.hpp"
#include "lib/sync.hpp"


namespace lib {
  
  
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
  class ClassLock 
    : public Sync<CONF>::Lock
    {
      typedef typename Sync<CONF>::Lock Lock;
      typedef typename sync::Monitor<CONF> Monitor;
      
      struct PerClassMonitor : Monitor {};
      
      Monitor&
      getPerClassMonitor()
        {
          static PerClassMonitor classMonitor;
          static ZombieCheck zombieCheck{util::typeStr(this)};
          
          zombieCheck();
          return classMonitor;
        }
      
    public:
      ClassLock() : Lock (getPerClassMonitor()) { }
    };
  
  
} // namespace lib
#endif /*LIB_SYNC_CLASSLOCK_H*/
