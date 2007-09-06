/*
  Singleton  -  template for implementing the singleton pattern 
 
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
    Alexandrescu, Andrei. "Modern C++ Design: 
        Generic Programming and Design Patterns Applied". 
        Copyright (c) 2001. Addison-Wesley. ISBN 0201704315
 
* *****************************************************/



#include "common/singleton.hpp"
#include "common/util.hpp"
#include "nobugcfg.h"

namespace cinelerra
  {
  
  /** Interface to be used by Singleton's clients.
   *  Manages internally the instance creation, lifecycle 
   *  and access handling in a multithreaded context.
   *  @return "the" single instance of class S 
   */
  template
    < class SI,                           // the class to make Singleton
      template <class> class Create,     // how to create/destroy the instance
      template <class> class Life,      // how to manage Singleton Lifecycle
      template <class> class Threading
    >
  SI&
  Singleton<SI,Create,Life,Threading>::instance()
  {
    if (!pInstance_)
      {
        ThreadLock guard;
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
  
  
  /** @internal helper used to delegate destroying the single instance
   *  to the Create policy, at the same time allowing the Life policy
   *  to control the point in the Application lifecycle when the 
   *  destruction of this instance occures.
   */
  template
    < class SI,
      template <class> class Create,
      template <class> class Life,
      template <class> class Threading
    >
  void
  Singleton<SI,Create,Life,Threading>::destroy()
  {
    REQUIRE (!isDead_);
    Create<SI>::destroy (pInstance_);
    pInstance_ = 0;
    isDead_ = true;
  }
  
  
  
  
  namespace singleton
    {
    
  } // namespace singleton
  
} // namespace cinelerra
