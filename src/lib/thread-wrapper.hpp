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


#include "backend/threads.h"
#include "include/nobugcfg.h"

#include <tr1/functional>
#include <boost/noncopyable.hpp>


namespace lumiera {

  using std::tr1::bind;
  using std::tr1::function;
  using std::tr1::placeholders::_1;
  
  
  /**
   * A thin convenience wrapper for dealing with threads,
   * as implemented by the backend (on top of pthread).
   * Using this wrapper...
   * - helps with passing data to the function executed in the new thread
   * - allows to bind to various kinds of functions
   * - supports integrating with an existing object monitor based lock (planned)
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
  class Thread : boost::noncopyable
    {
      typedef function<void(void)> Operation;
      
      static void
      run (void* arg)
        { 
          ASSERT (arg);
          Operation doIt_ = *reinterpret_cast<Operation*>(arg);
          
          doIt_();
        }
      
      
    public:
      Thread (Literal& purpose, Operation operation, struct nobug_flag *logging_flag = &NOBUG_FLAG(operate))
        {
          lumiera_thread_run ( LUMIERA_THREAD_INTERACTIVE
                             , &run         // invoking the run helper and..
                             , &operation   // passing the actual operation as parameter
                             , 0            // no condition variable provided (for now...)
                             , purpose.c_str()
                             , logging_flag
                             );
        }
    };
    
  

   
} // namespace lumiera
#endif
