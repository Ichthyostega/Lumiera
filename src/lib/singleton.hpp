/*
  SINGLETON.hpp  -  configuration header for singleton factory

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file singleton.hpp
 ** Factory for creating Singleton instances.
 ** The <b>Singleton Pattern</b> provides a single access point to a class or
 ** service and exploits this ubiquitous access point to limit the number of objects
 ** of this type to a single shared instance. Within Lumiera, we mostly employ a
 ** factory template; the intention is to use on-demand initialisation and a
 ** standardised lifecycle.
 ** 
 ** This configuration header just pulls in some other implementation headers in 
 ** the right order. The basic class template for creating singletons resides in
 ** singleton-factory.hpp, besides we need policy classes defining how to create
 ** the singleton objects and how to manage singleton lifecycle. Finally,
 ** we want to preconfigure singleton factories for some important facilities;
 ** e.g. sometimes we want to include a hook for injecting Test Mock instances.
 ** 
 ** You'll find the default Policies in singleton-factory.hpp and the default
 ** definition of type lumiera::singleton in singleton-preconfigure.hpp
 ** 
 ** \par Why Singletons? Inversion-of-Control and Dependency Injection
 ** 
 ** Singletons are frequently over-used, and often they serve as disguised
 ** global variables to support a procedural programming style. As a remedy, typically
 ** the use of a »Dependency Injection Container« is promoted. And -- again typically --
 ** these DI containers tend to evolve into heavyweight universal tools and substitute
 ** the original problem by metadata hell.
 ** 
 ** Thus, for Lumiera, the choice to use Singletons was deliberate: we understand the
 ** Inversion-of-Control principle, yet we want to stay just below the level of building
 ** a central application manager core. At the usage site, we access a factory for some
 ** service <i>by name</i>, where the »name« is actually the type name of an interface
 ** or facade. Singleton is used as an implementation of this factory, when the service
 ** is self-contained and can be brought up lazily.
 ** 
 ** \par Conventions, Lifecycle and Unit Testing
 ** 
 ** Usually we place an instance of the singleton factory (or some other kind of factory)
 ** as a static variable within the interface class describing the service or facade.
 ** As a rule, everything accessible as Singleton is sufficiently self-contained to come
 ** up any time -- even prior to \c main(). But at shutdown, any deregistration must be
 ** done explicitly using a lifecycle hook. Destructors aren't allowed to do any significant
 ** work besides releasing references, and we acknowledge that singletons can be released
 ** in \em arbitrary order.
 ** 
 ** @see SingletonFactory
 ** @see singleton::StaticCreate
 ** @see singleton::AutoDestroy 
 ** @see singletontest.hpp
 ** @see SingletonTestMock_test
 */


#ifndef LIB_SINGLETON_H
#define LIB_SINGLETON_H


#include "lib/singleton-policies.hpp"
#include "lib/singleton-factory.hpp"
#include "lib/singleton-preconfigure.hpp"


#endif
