/*
  CALL-QUEUE.hpp  -  a queue to dispatch bound function invocations into another thread

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file call-queue.hpp
 ** A Queue for function invocations, allowing them to be dispatched on demand.
 ** Typically this queue helper is used to forward lambdas into another thread, e.g.
 ** the UI thread for execution.
 ** 
 ** @todo simplistic braindead implementation with locking and heap based storage
 **       based on std::function; should use a lock-fee queue and should have an eye
 **       on the impact of managing argument storage
 ** 
 ** @see gui::NotificationService usage example
 */


#ifndef LIB_CALL_QUEUE_H
#define LIB_CALL_QUEUE_H


#include "lib/error.hpp"

#include <boost/noncopyable.hpp>


namespace lib {
  
  
  
  /*********************************************************//**
   * A threadsafe queue for bound `void(void)` functors.
   * Typically used to dispatch function invocations together with
   * their concrete parameters into another thread for invocation.
   */
  struct CallQueue
    : boost::noncopyable
    {
      
    };
  
  
  
} // namespace lib
#endif /*LIB_CALL_QUEUE_H*/
