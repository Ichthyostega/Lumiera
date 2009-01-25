/*
  SINGLETONFACTORY.hpp  -  template for implementing the singleton pattern
 
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

====================================================================
This code is heavily inspired by  
 The Loki Library (loki-lib/trunk/include/loki/Singleton.h)
    Copyright (c) 2001 by Andrei Alexandrescu
    This Loki code accompanies the book:
    Alexandrescu, Andrei. "Modern C++ Design: Generic Programming
        and Design Patterns Applied". 
        Copyright (c) 2001. Addison-Wesley. ISBN 0201704315
 
*/



#ifndef LUMIERA_SINGLETONFACTORY_H
#define LUMIERA_SINGLETONFACTORY_H


#include "lib/singletonpolicies.hpp"  // several Policies usable together with SingletonFactory

#include "include/logging.h"
#include "lib/util.hpp"
#include "lib/sync-classlock.hpp"

namespace lumiera {
  
  /**
   * A configurable Template for implementing Singletons. 
   * Actually this is a Factory object, which could be placed into a static field
   * of the Singleton (target) class or used directly. 
   * @note internally uses static fields, so all factory instances share pInstance_
   * @note there is an ongoing discussion regarding Double Checked Locking pattern,
   *       which in this case boils down to the question: does \c pthread_mutex_lock/unlock
   *       constitute a memory barrier, such as to force any memory writes done within
   *       the singleton ctor to be flushed and visible to other threads when releasing
   *       the lock? To my understanding, the answer is yes. See
   *       http://www.opengroup.org/onlinepubs/000095399/basedefs/xbd_chap04.html#tag_04_10
   * @param SI the class of the Singleton instance
   * @param Create policy defining how to create/destroy the instance
   * @param Life policy defining how to manage Singleton Lifecycle
   */
  template
    < class SI
    , template <class> class Create    = singleton::StaticCreate 
    , template <class> class Life      = singleton::AutoDestroy
    >
  class SingletonFactory
    {
      typedef SI* volatile  PType;
      typedef lib::ClassLock<SI> ThreadLock;
      
      static PType pInstance_;
      static bool isDead_;

      
    public:
      /** Interface to be used by SingletonFactory's clients.
       *  Manages internally the instance creation, lifecycle 
       *  and access handling in a multithreaded context.
       *  @return "the" single instance of class S 
       */
      SI& operator() ()
        {
          if (!pInstance_)
            {
              NOBUG_INIT;
              FIXME ("NoBug wasnt even basically initialized here, the old ON_BASIC_INIT did not initialize it in proper order"
                     " so I explicitly init it here (calling init multiple times does not harm) --cehteh"
                     " nb maybe as exception we can use an explicit mutex here since pthread mutexes can be statically initialized");
              ThreadLock guard  SIDEEFFECT;

              if (!pInstance_)
                {
                  if (isDead_)
                    {
                      Life<SI>::onDeadReference();
                      isDead_ = false;
                    }
                  pInstance_ = Create<SI>::create();
                  Life<SI>::scheduleDelete (&destroy);
            }   }
          ENSURE (pInstance_);
          ENSURE (!isDead_);
          return *pInstance_;
        }
      
    private:
      /** @internal helper used to delegate destroying the single instance
       *  to the Create policy, at the same time allowing the Life policy
       *  to control the point in the Application lifecycle when the 
       *  destruction of this instance occurs.
       */
      static void destroy()
        {
          REQUIRE (!isDead_);
          Create<SI>::destroy (pInstance_);
          pInstance_ = 0;
          isDead_ = true;
        }
    };
    
  
  // Storage for SingletonFactory's static fields...  
  template
    < class SI,
      template <class> class C,
      template <class> class L
    >
    typename SingletonFactory<SI,C,L>::PType 
    SingletonFactory<SI,C,L>::pInstance_;
  
  template
    < class SI,
      template <class> class C,
      template <class> class L
    >
    bool SingletonFactory<SI,C,L>::isDead_;
  
  
  
///// Question: can we get rid of the static fields?
/////       this is tricky because of invoking the destructors. If we rely on instance vars,
/////       the object may already have been released when the runtime system calls the
/////       destructors of static objects at shutdown.
/////       It seems this would either cost us much of the flexibility or get complicated
/////       to a point where we could as well implement our own Dependency Injection Manager.
/////       But the whole point of my pervasive use of this singleton template is to remain
/////       below this borderline of integration ("IoC yes, but avoid DI").
  
} // namespace lumiera
#endif
/*
// Local Variables:
// mode: C++
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
