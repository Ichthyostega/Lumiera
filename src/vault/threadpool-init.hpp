/*
  THREADPOOL-INIT.hpp  -  pull up the Thread management automagically at application initialisation 
 

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

*/

/** @file threadpool-init.hpp
 ** Automatically bring up the threading management and threadpool in the vault layer.
 ** This works by registering a lifecycle callback, which is activated at the start
 ** of main or when running the testsuite. Similarly, a shutdown hook is registered.
 ** 
 ** @todo as of 1/10 it is not clear if this will be the final solution.
 **       Alternatively, we may rely on a subsystem "vault" or "threadpool"
 ** 
 ** @see threads.h
 ** @see thread-wrapper.hpp
 ** 
 */


#ifndef VAULT_THREADPOOL_INIT_H
#define VAULT_THREADPOOL_INIT_H

#include "include/lifecycle.h"

extern "C" {
#include "vault/threads.h"
}



namespace lumiera { 
  void initialise_Threadpool ();
  void shutdown_Threadpool ();
  
  namespace {
    LifecycleHook trigger_1_ (ON_GLOBAL_INIT,   &initialise_Threadpool);
    LifecycleHook trigger_2_ (ON_GLOBAL_SHUTDOWN, &shutdown_Threadpool);
} }


#endif /* VAULT_THREADPOOL_INIT_H */
