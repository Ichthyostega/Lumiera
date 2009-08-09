/*
  SINGLETON-SUBCLASS.hpp  -  variant of the singleton (factory) creating a subclass
 
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
 
*/

/** @file singleton-subclass.hpp
 ** Specialised SingletonFactory creating subclasses of the nominal type.
 ** The rationale is to be able to defer the decision what type to create
 ** down to the point where the singleton factory is actually created.
 ** Thus the code using the singleton need not know the implementation
 ** class, but nevertheless gets an non-virtual access function to the
 ** singleton instance (which can be inlined), and the compiler is
 ** still able to spot type errors. Maybe someone knows a less 
 ** contrived solution fulfilling the same criteria....?
 ** 
 ** @see configrules.cpp usage example
 ** @see SingletonSubclass_test
 */


#ifndef LUMIERA_SINGLETON_SUBCLASS_H
#define LUMIERA_SINGLETON_SUBCLASS_H


#include "lib/singleton.hpp"

#include <boost/scoped_ptr.hpp>
#include <typeinfo>


namespace lumiera {
  
  using boost::scoped_ptr;
  
  
  namespace singleton {
    
      /** 
       * Helper template to use the general policy classes of the lumiera::Singleton,
       * but change the way they are parametrised on-the-fly. 
       */
      template<template<class> class POL, class I>
      struct Adapter
        {
          
          struct Link
            {
              virtual ~Link() {}
              virtual I* create ()          = 0;   ///< @note compiler will check if the actual type is assignable...
              virtual void destroy (I* pSi) = 0;
            };
          
          template<class S>
          struct TypedLink : Link
            {
              virtual S* create ()           { return POL<S>::create (); }                 // covariance checked!
              virtual void destroy (I* pSi)  { POL<S>::destroy (static_cast<S*> (pSi)); }
            };
          
          
          struct My_scoped_ptr : scoped_ptr<Link> ///< implementation detail: defeat static initialisation
            {
              using scoped_ptr<Link>::get;
              My_scoped_ptr() : scoped_ptr<Link> (get()? get() : 0) {}   ///< bypass if already configured
            };
          
          /** we configure this link \e later, when the singleton factory
           *  is actually created, to point at the desired implementation subclass.
           */
          static My_scoped_ptr link;
          
          /** Forwarding Template used to configure the basic SingletonFactory */
          template<class II>
          struct Adapted
            {
              static II* create ()           { return link->create (); }
              static void destroy (II* pSi)  { link->destroy (pSi); }
            };
        };
      
      template<template<class> class A, class I>
      typename Adapter<A,I>::My_scoped_ptr Adapter<A,I>::link;    // note: use special ctor (due to static init order!)
      
      
      /** type-information used to configure the factory instance
       *  with the concrete implementation type to be created. */
      template<class SU>
      struct UseSubclass
      { };
      
  } // namespace singleton
  
  
  
  
  /**
   * Special variant of the SingletonFactory with the option of actually creating
   * a subclass or wrap the product in some way. For the user code, it should behave
   * exactly like the standard SingletonFactory. The configuration of the product
   * actually to be created is delayed until the ctor call, so it can be hidden
   * away to the implementation of a class using the SingletonFactory.
   * 
   * @see configrules.cpp usage example
   */
  template
    < class SI  // the class to use as Interface for the Singleton
    , template <class> class Create    = singleton::StaticCreate   // how to create/destroy the instance
    , template <class> class Life      = singleton::AutoDestroy   // how to manage Singleton Lifecycle
    >
  class SingletonSubclassFactory
    : public SingletonFactory< SI
                             , singleton::Adapter<Create,SI>::template Adapted
                             , Life
                             >
    {
    public:
      /** The singleton-factory ctor configures what concrete type to create.
       *  It takes type information passed as dummy parameter and installs
       *  a trampoline object in the static field of class Adapter to perform
       *  the necessary up/downcasts. This allows to use whatever policy
       *  class is desired, but parametrises this policy template with
       *  the concrete type to be created. (only the  "create" policy
       *  needs to know the actual class, because it allocates storage)
       */
      template<class SU>
      SingletonSubclassFactory (singleton::UseSubclass<SU>&)
        {
          typedef typename singleton::Adapter<Create,SI>   Adapter;
          typedef typename Adapter::template TypedLink<SU> TypedLink;
          
          if (!Adapter::link)
            Adapter::link.reset (new TypedLink);
          
#ifdef DEBUG
          else
            REQUIRE ( typeid(*Adapter::link) == typeid(new TypedLink),
                      "If using several instances of the sub-class-creating "
                      "singleton factory, all *must* be configured to create "
                      "objects of exactly the same implementation type!");
#endif
        }
    };
  
  
  /**
   * Default Singleton configuration (subclass creating factory)
   * @note all Policy template parameters taking default values
   */
  template <class SI>
  struct SingletonSub 
    : public SingletonSubclassFactory<SI>
    { 
      template<typename TY>
      SingletonSub (TY ref) : SingletonSubclassFactory<SI>(ref) {}
    };


} // namespace lumiera

#endif
