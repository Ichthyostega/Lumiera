/*
  CALL-QUEUE.hpp  -  a queue to dispatch bound function invocations into another thread

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
 ** @see stage::NotificationService usage example
 */


#ifndef LIB_CALL_QUEUE_H
#define LIB_CALL_QUEUE_H


#include "lib/error.hpp"
#include "lib/sync.hpp"
#include "lib/iter-stack.hpp"
#include "lib/nocopy.hpp"

#include <functional>


namespace lib {
  namespace error = lumiera::error;
  
  
  
  /*********************************************************//**
   * A threadsafe queue for bound `void(void)` functors.
   * Typically used to dispatch function invocations together with
   * their concrete parameters into another thread for invocation.
   */
  class CallQueue
    : util::NonCopyable
    , public Sync<>
    {
    public:
      using Operation = std::function<void(void)>;
      
    private:
      lib::IterQueue<Operation> queue_;
      
    public:
      CallQueue() { }
      
      
      CallQueue&
      feed (Operation&& op)
        {
          if (not op)
            throw error::Logic( "Unbound Functor fed to dispatcher CallQueue"
                              , error::LUMIERA_ERROR_BOTTOM_VALUE);
          {
            Lock sync{this};
            queue_.feed (move(op));
          }
          return *this;
        }
      
      CallQueue&
      invoke()
        {
          if (not empty())
            {
              Operation operate;
              {
                Lock sync{this};
                operate = move (*queue_);
                ++queue_;
              }
              ASSERT (operate);
              operate();
            }
          return *this;
        }
      
      
      /* == diagnostics == */
      
      size_t
      size()  const
        {
          Lock sync{this};
          return queue_.size();
        }
      
      bool
      empty() const
        {
          return 0 == size();
        }
    };
  
  
  
} // namespace lib
#endif /*LIB_CALL_QUEUE_H*/
