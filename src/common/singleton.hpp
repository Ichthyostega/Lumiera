/*
  SINGLETON.hpp  -  configuration header for singleton factory
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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

/** @file singleton.hpp
 ** Factory for creating Singleton instances.
 ** This configuration header just pulls in some other implementation headers in 
 ** the right order. The basic class template for creating singletons resides in
 ** singletonfactory.hpp, besides we need policy classes defining how to create
 ** the singleton objects, how to manage lifecycle and multithreading. Finally,
 ** we want to preconfigure singleton factories for some important facilities;
 ** e.g. sometimes we want to include a hook for injecting Test Mock instances.
 **
 ** You'll find the default Policies in singletonfactory.hpp and the default
 ** definition of type cinelerra::singleton in singletonpreconfigure.hpp
 ** 
 ** @see SingletonFactory
 ** @see singleton::StaticCreate
 ** @see singleton::AutoDestroy 
 ** @see singletontest.hpp
 ** @see singletontestmocktest.hpp
 */


#ifndef CINELERRA_SINGLETON_H
#define CINELERRA_SINGLETON_H


#include "common/singletonpolicies.hpp"
#include "common/singletonfactory.hpp"
#include "common/singletonpreconfigure.hpp"


#endif
