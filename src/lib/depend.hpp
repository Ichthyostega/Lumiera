/*
  DEPENDENCY.hpp  -  access point to singletons and dependencies

  Copyright (C)         Lumiera.org
    2013,               Hermann Vosseler <Ichthyostega@web.de>

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

====================================================================
This code is heavily inspired by
 The Loki Library (loki-lib/trunk/include/loki/Singleton.h)
    Copyright (c) 2001 by Andrei Alexandrescu
    Loki code accompanies the book:
    Alexandrescu, Andrei. "Modern C++ Design: Generic Programming
        and Design Patterns Applied".
        Copyright (c) 2001. Addison-Wesley. ISBN 0201704315

*/



#ifndef LIB_DEPEND_H
#define LIB_DEPEND_H


#include "lib/nobug-init.hpp"
#include "lib/sync-classlock.hpp"

namespace lib {
  
  /**
   * Access point to singletons and other kinds of dependencies.
   * Actually this is a Factory object, which is typically placed into a static field
   * of the Singleton (target) class or some otherwise suitable interface.
   * @note uses static fields internally, so all factory instances share pInstance_
   * @remark there is an ongoing discussion regarding the viability of the
   *   Double Checked Locking pattern, which requires either the context of a clearly defined
   *   language memory model (as in Java), or needs to be supplemented by memory barriers.
   *   In our case, this debate boils down to the question: does \c pthread_mutex_lock/unlock
   *   constitute a memory barrier, such as to force any memory writes happening \em within
   *   the singleton ctor to be flushed and visible to other threads when releasing the lock?
   *   To my understanding, the answer is yes. See
   *   [posix](http://www.opengroup.org/onlinepubs/000095399/basedefs/xbd_chap04.html#tag_04_10)
   * @param SI the class of the Singleton instance
   * @param Create policy defining how to create/destroy the instance
   * @param Life policy defining how to manage Singleton Lifecycle
   */
  template<class SI>
  class Depend
    {
      typedef ClassLock<SI> ThreadLock;
      typedef DependencyFactory<SI> Factory;
      
      static SI* volatile instance;
      static Factory       factory;
      
      
    public:
      /** Interface to be used by SingletonFactory's clients.
       *  Manages internally the instance creation, lifecycle
       *  and access handling in a multithreaded context.
       *  @return "the" single instance of class S
       */
      SI&
      operator() ()
        {
          if (!instance)
            {
              ThreadLock guard;
              
              if (!instance)
                instance = factory.buildInstance();
            }
          ENSURE (instance);
          return *instance;
        }
      
      void
      shutdown()
        {
          ThreadLock guard;
          
          factory.deconfigure (instance);
          instance = NULL;
        }
      
      void
      injectReplacement (SI* mock)
        {
          if (mock)
            {
              factory.takeOwnership (mock);   // EX_SANE
              
              ThreadLock guard;
              factory.shaddow (instance);
              instance = mock;
            }
          else
            {
              factory.discardMock (instance); // EX_FREE
              
              ThreadLock guard;
              instance = factory.restore ();  // EX_SANE
            }
        }
      
      Depend ();
      Depend (typename Factory::Constructor ctor);
      
    private:
    };
  
  
  // Storage for SingletonFactory's static fields...
  template<class SI>
    SI* volatile Depend<SI>::instance;
  
  
  
  
} // namespace lib
#endif
