/*
  CONCURRENCY.hpp  -  generic helper for object based locking and synchronisation
 
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

/** @file concurrency.hpp
 ** Collection of helpers and wrappers to support dealing with concurrency issues.
 ** Actually, everything is implemented either by the Lumiera backend, or directly
 ** by pthread. The purpose is to support and automate the most common use cases
 ** in object oriented style.
 **
 ** @see mutex.h
 ** @see concurrency-locking-test.cpp
 ** @see asset::AssetManager::reg() usage example
 ** @see subsystemrunner.hpp usage example
 */


#ifndef LIB_CONCURRENCY_H
#define LIB_CONCURRENCY_H

#include "include/nobugcfg.h"
#include "lib/util.hpp"


namespace lib {
      
    /**
     * Facility for monitor object based locking. 
     * To be attached either on a per class base or per object base.
     * Typically, the client class will inherit from this template (but it
     * is possible to use it stand-alone, if inheriting isn't an option).
     * The interface for clients to access the functionality is the embedded
     * Lock template, which should be instantiated as an automatic variable
     * within the scope to be protected.
     * 
     * @todo actually implement this facility using the Lumiera backend.
     */
    struct Concurrency
      {
        struct Monitor
          {
            void acquireLock() { TODO ("acquire Thread Lock"); }
            void releaseLock() { TODO ("release Thread Lock"); }
          };
        
        template<class X>
        static Monitor& getMonitor(X* forThis);
        
        template<class X>
        class Lock
          {
            Monitor& mon_;
          public:
            Lock(X* it=0)
              : mon_(getMonitor<X> (it)) 
              { 
                mon_.acquireLock(); 
              }
            
            ~Lock()
              { 
                mon_.releaseLock(); 
              }
          };
      };
    
    
    
    template<class X>
    Concurrency::Monitor&
    Concurrency::getMonitor(X* forThis)
    {
      UNIMPLEMENTED ("get moni");
    }
    
  
} // namespace lumiera
#endif
