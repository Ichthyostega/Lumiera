/*
  THREADWRAPPER.hpp  -  thin convenience wrapper for starting lumiera threads
 
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


#ifndef LUMIERA_THREADWRAPPER_H
#define LUMIERA_THREADWRAPPER_H


#include "include/nobugcfg.h"
#include "lib/sync.hpp"

extern "C" {
#include "backend/threads.h"
}

#include <tr1/functional>
#include <boost/noncopyable.hpp>


namespace lumiera {
  
  using std::tr1::bind;
  using std::tr1::function;
  using std::tr1::placeholders::_1;
  
  using lib::Sync;
  using lib::NonrecursiveLock_Waitable;
  
  
  /**
   * A thin convenience wrapper for dealing with threads,
   * as implemented by the backend (on top of pthread).
   * Using this wrapper...
   * - helps with passing data to the function executed in the new thread
   * - allows to bind to various kinds of functions including member functions
   * - supports integrating with an existing object monitor based lock (planned)
   * The new thread starts immediately within the ctor; after returning, the new
   * thread has already copied the arguments and indeed actively started to run.
   * 
   * @note this class is \em not a thread handle. Within Lumiera, we do all of
   *       our thread management such as to avoid using global thread handles.
   *       If some cooperation between threads is needed, this should be done
   *       in a implementation private way, e.g. by sharing a condition var.  
   * 
   * @todo Ichthyo started this wrapper 12/08 while our own thread handling
   *       was just being shaped. It may well be possible that such a wrapper
   *       is superfluous in the final application. Re-evaluate this!
   */
  class Thread
    : public Sync<NonrecursiveLock_Waitable>,
      boost::noncopyable
    {
      volatile bool started_;
      
      typedef function<void(void)> Operation;
      Operation const& operation_;
      
      static void
      run (void* arg)
        { 
          ASSERT (arg);
          Thread* startingWrapper = reinterpret_cast<Thread*>(arg);
          Operation _doIt_(startingWrapper->operation_);
          {
            Lock sync(startingWrapper);
            startingWrapper->started_ = true;
            sync.notify(); // handshake signalling we've gotten the parameter
          }
          
          _doIt_(); // execute the actual operation in the new thread
        }
      
      
    public:
      Thread (Literal& purpose, Operation const& operation, struct nobug_flag *logging_flag = &NOBUG_FLAG(operate))
        : started_(false),
          operation_(operation)
        {
          Lock sync(this);
          LumieraThread res = 
          lumiera_thread_run ( LUMIERA_THREAD_INTERACTIVE
                             , &run         // invoking the run helper and..
                             , this         // passing this start context as parameter
                             , 0            // no condition variable provided (for now...)
                             , purpose.c_str()
                             , logging_flag
                             );
          
          if (!res)
            throw error::State("failed to create new thread.");
          
          // make sure the new thread had the opportunity to take the Operation
          // prior to leaving and thereby possibly destroying this local context
          sync.wait (started_);
        }
    };
  
  
  
 } // namespace lumiera
#endif
