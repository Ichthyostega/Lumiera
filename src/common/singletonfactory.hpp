/*
  SINGLETONFACTORY.hpp  -  template for implementing the singleton pattern
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
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



#ifndef CINELERRA_SINGLETONFACTORY_H
#define CINELERRA_SINGLETONFACTORY_H


#include "common/singletonpolicies.hpp"  ///< several Policies usable together with SingletonFactory

#include "common/util.hpp"
#include "nobugcfg.h"

//#include <boost/bind.hpp>


namespace cinelerra
  {
  
  /**
   * A configurable Template for implementing Singletons. 
   * Actually this is a Functor object, which could be placed into a static field
   * of the Singleton (target) class or used directly. 
   * @note internally uses static fields, so all functor instances share pInstance_
   */
  template
    < class SI,  // the class to make Singleton
      template <class> class Create    = singleton::StaticCreate,  // how to create/destroy the instance
      template <class> class Life      = singleton::AutoDestroy,  // how to manage Singleton Lifecycle
      template <class> class Threading = singleton::IgnoreThreadsafety  //TODO use Multithreaded!!!
    >
  class SingletonFactory
    {
      typedef typename Threading<SI>::VolatileType SType;
      typedef typename Threading<SI>::Lock ThreadLock;
      static SType* pInstance_;
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
       *  destruction of this instance occures.
       */
      static void destroy()
        {
          TRACE (singleton, "Singleton: triggering destruction");
          
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
      template <class> class L,
      template <class> class T
    >
    typename SingletonFactory<SI,C,L,T>::SType* 
    SingletonFactory<SI,C,L,T>::pInstance_;
  
  template
    < class SI,
      template <class> class C,
      template <class> class L,
      template <class> class T
    >
    bool SingletonFactory<SI,C,L,T>::isDead_;


  
///// TODO: get rid of the static fields?
/////       is tricky because of invoking the destructors. If we rely on instance vars,
/////       the object may already have been released when the runtime system calls the
/////       destructors of static objects at shutdown.
/////       It seems this would either cost us much of the flexibility or get complicated
/////       to a point where we could as well implement our own Depenency Injection Manager.

      /** @internal used to link together the Create policy and Life policy.
       *  @return a functor object for invoking this->destroy() */
/*      SingletonFactory::DelFunc getDeleter() 
        {
          return boost::bind (&SingletonFactory<SI,Create,Life,Threading>::destroy,
                              this);
        }
*/
  
/*      template<class T>
      class DelFunc
        {
          typedef void (T::*Fp)(void);
          T* t_;
          Fp fun_;
        public:
          DelFunc (T* t, Fp f) : t_(t), fun_(f) {}
          void operator() () { (t_->*fun_)(); }
        };
*/    
} // namespace cinelerra
#endif
