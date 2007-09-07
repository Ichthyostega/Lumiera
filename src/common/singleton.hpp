/*
  SINGLETON.hpp  -  template for implementing the singleton pattern
 
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

====================================================================
This code is heavily inspired by  
 The Loki Library (loki-lib/trunk/include/loki/Singleton.h)
    Copyright (c) 2001 by Andrei Alexandrescu
    This Loki code accompanies the book:
    Alexandrescu, Andrei. "Modern C++ Design: Generic Programming
        and Design Patterns Applied". 
        Copyright (c) 2001. Addison-Wesley. ISBN 0201704315
 
*/



#ifndef CINELERRA_SINGLETON_H
#define CINELERRA_SINGLETON_H


#include "common/singletonpolicies.hpp"  ///< several Policies usable together with singleton



namespace cinelerra
  {
  
  /**
   * A configurable Template for implementing Singletons
   */
  template
    < class S,  // the class to make Singleton
      template <class> class Create    = singleton::Static,     // how to create/destroy the instance
      template <class> class Life      = singleton::Automatic,  // how to manage Singleton Lifecycle
      template <class> class Threading = singleton::IgnoreThreadsafety  //TODO use Multithreaded!!!
    >
  class Singleton
    {
    public:
      static S& instance();
      
    private:
      typedef typename Threading<S>::VolatileType SType;
      typedef typename Threading<S>::Lock ThreadLock;
      static SType* pInstance_;
      static bool isDead_;
      
    protected:
      Singleton () { }
      Singleton (const Singleton&) { }
      Singleton& operator= (const Singleton&) { return *this; }
      
    private:
      static void destroy();
    };
  
  
} // namespace cinelerra
#endif
