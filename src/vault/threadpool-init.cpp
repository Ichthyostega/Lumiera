/*
  ThreadpoolInit  -  pull up the Thread management automagically at application initialisation 

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file threadpool-init.cpp
 ** Implementation of automatic initialisation of the low-level thread handling framework.
 */


#include "backend/threadpool-init.hpp"



namespace lumiera {
  
  void
  initialise_Threadpool ()
  {
    lumiera_threadpool_init();
    
////////////////////////////////////////////////////////////////////////TODO: a better way to detect Alpha/beta builds
#ifdef DEBUG
    static uint callCount = 0;
    ASSERT ( 0 == callCount++ );
#endif
  }
  
  
  void
  shutdown_Threadpool ()
  {
    lumiera_threadpool_destroy();
  }
}

