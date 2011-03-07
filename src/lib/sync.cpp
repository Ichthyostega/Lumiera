/*
  Sync  -  generic helper for object based locking and synchronisation

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/

/** @file sync.cpp
 ** This compilation unit holds the static attribute struct
 ** for initialising pthread's recursive mutex.
 ** 
 */


#include "lib/sync.hpp"


namespace lib {
namespace sync {
  
  
  namespace { // private pthread attributes
    
    pthread_mutexattr_t attribute_;
    pthread_once_t is_init_ = PTHREAD_ONCE_INIT;
    
    void
    initAttribute()
    {
      pthread_mutexattr_init    (&attribute_);
      pthread_mutexattr_settype (&attribute_, PTHREAD_MUTEX_RECURSIVE);
    }
    
    
    inline pthread_mutexattr_t*
    recursive_flag()
    {
      pthread_once (&is_init_, initAttribute);
      return &attribute_;
    }
  }
  
  
  
  /** 
   * @internal creating a recursive mutex.
   * Defined here in a separate compilation unit,
   * so it can refer to a single mutex attribute flag. 
   */
  Wrapped_RecursiveMutex::Wrapped_RecursiveMutex()
  {
    pthread_mutex_init (&mutex_, recursive_flag());
  }
  
  
  
}}// lib::sync
