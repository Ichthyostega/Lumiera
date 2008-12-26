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
 ** @see singletonfactory.hpp usage example
 */


#ifndef LIB_SYNC_CLASSLOCK_H
#define LIB_SYNC_CLASSLOCK_H

#include "lib/sync.hpp"


namespace lib {
  
  template<class X, class CONF = NonrecursiveLock_NoWait>
  class ClassLock 
    : public Sync<CONF>::Lock
    {
      typedef typename Sync<CONF>::Lock Lock;
      typedef typename Sync<CONF>::Monitor Monitor;
      
      
      static Monitor&
      getMonitor()
        {
          //TODO: a rather obscure race condition is hidden here:
          //TODO: depending on the build order, the dtor of this static variable may be called, while another thread is still holding an ClassLock.
          //TODO: An possible solution would be to use an shared_ptr to the Monitor in case of a ClassLock and to protect access with another Mutex.
          //TODO. But I am really questioning if we can't ignore this case and state: "don't hold a ClassLock when your code maybe still running in shutdown phase!"
          //TODO: probably best Idea is to detect this situation in DEBUG or ALPHA mode
          
          static scoped_ptr<Monitor> classMonitor_ (0);
          if (!classMonitor_) classMonitor_.reset (new Monitor ());
          return *classMonitor_;
        }
      
      
    public:
      ClassLock() : Lock (getMonitor()) {}
    };
  
  
} // namespace lib
#endif
