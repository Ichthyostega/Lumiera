/*
  SYNC-BARRIER.hpp  -  N-fold synchronisation point with yield-wait

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file sync-barrier.hpp
 ** A N-fold synchronisation latch using yield-wait until fulfilment.
 ** Semantically, this barrier is similar to std::latch (introduced with C++20).
 ** It is intended for one-time use and blocks any invocation until N invocations occurred.
 ** The »wait« however is implemented as a spinning loop with `std::this_thread::yield`,
 ** which places this implementation into a middle ground between a full-fledged blocking
 ** lock and a busy spinning log. The `yield()` call is expected to de-prioritise the current
 ** thread behind all other threads currently amenable for active execution, without actually
 ** placing the thread into sleeping state. The coordination of the latch relies on Atomics.
 ** @see SyncBarrier_test
 ** @see steam::control::DispatcherLoop
 ** @todo as of 9/2023 it remains to be seen if this facility is just a pre-C++20 workaround;
 **       otherwise it may present distinct performance characteristics than std::latch,
 **       possibly also a slightly more abstracted (and thus clearer) usage API.
 */


#ifndef LIB_SYNC_BARRIER_H
#define LIB_SYNC_BARRIER_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
//#include "include/logging.h"
//#include "lib/meta/function.hpp"
//#include "lib/result.hpp"

#include <thread>
#include <atomic>


namespace lib {
  
//  using lib::Literal;
//  namespace error = lumiera::error;
//  using error::LERR_(STATE);
//  using error::LERR_(EXTERNAL);
  
  
  
  /**
   * A one time N-fold mutual synchronisation barrier.
   * Calls to #sync() will block until N such calls occurred.
   * @note The blocking wait is implemented by a check-and-`yield()` loop,
   *       increasing load at the OS scheduler, possibly starving the system
   *       when stretched out over extended time.
   * @remark intended use is to allow all participants to catch up and reach
   *       a well defined point with initialisation or implementation logic.
   */
  class SyncBarrier
    : util::NonCopyable
    {
      std::atomic_int latch_;
      
    public:
      /** @param nFold the number of participants to sync */
      explicit
      SyncBarrier (uint nFold =2)
        : latch_{int(nFold)}
        {
          REQUIRE (nFold >= 2, "Pointless to sync less than two participants.");
        }
      
      void
      sync()
        {
          size_t level = latch_.fetch_add(-1, std::memory_order_acq_rel);
          if (1 < level)
            do std::this_thread::yield();
            while (0 < latch_.load (std::memory_order_relaxed));
          else
            latch_.store (0, std::memory_order_relaxed);
        }
    };
  
  
  
} // namespace lib
#endif /*LIB_SYNC_BARRIER_H*/
