/*
  LIVE-MARK.hpp  -  safety-tag to prevent invoking a deceased partner

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file live-mark.hpp
 ** Memory marker tag to prevent segfault in communication with a defunct receiver.
 ** The tag comprises an unique hash value, that can be embedded into another object
 ** and tied to the lifecycle. This setup allows to build a safe-handle, that offers
 ** a liveness check of the communication partner and prevents dereferencing and
 ** invoking into the stale memory of an expired receiver. Notably this allows
 ** to safeguard asynchronous communication between thread-local entities,
 ** even when some of the participating threads terminates unexpectedly.
 ** 
 ** @todo WIP 5/2026 developing a thread-local allocation pool
 ** 
 ** @see engine-buffer-manager.hpp usage example
 ** @see LiveMark_test
 ** 
 */

#ifndef LIB_LIVE_MARK_H_
#define LIB_LIVE_MARK_H_

#include "lib/error.hpp"
#include "lib/hash-combine.hpp"
#include "lib/util.hpp"

#include <atomic>
#include <concepts>

namespace lib {
  namespace err = lumiera::error;
  
  using lib::HashVal;
  
  
  class LiveMark;
  
  template<class SUBJ>
  concept alive_marked = requires (SUBJ const& subject)
    {
      { subject.getLiveMark() } -> std::convertible_to<LiveMark const&>;
    };
  
  
  /**
   * Liveness marker tag to be incorporated into another object,
   * so that a communication link can be severed automatically when
   * the receiver object goes out of scope.
   * @see AllocReceiver
   * @see LiveMark_test
   */
  class LiveMark
    {
      std::atomic<HashVal> id_;

    public:
      LiveMark()  = default;
     ~LiveMark() { disengage(); }  ///< @note the actual protection happens here
      
      /**
       * Create a LiveMark that is connected
       * to the Subject to be protected.
       */
      LiveMark (alive_marked auto& subject)
        : id_{generateID (subject)}
        { }
      
      // using default copy/assignment
      
      /** deactivate and mark as dead (irreversible) */
      void disengage() { id_.store (0, std::memory_order_relaxed); }
      
      
      /**
       * A smart-handle for safe protected access to some object
       * that might go out of scope eventually. This works under the
       * assumption that the target object embeds a LiveMark, which
       * is expressed through the concept alive_marked. The marker
       * HashVal is unique for each instance and distinct memory
       * location, and thus even access to a new object instance
       * with the same type, placed into the same memory location
       * will be rejected, after the original target was destroyed.
       */
      template<alive_marked SUBJ>
      class SafeHandle
        {
          SUBJ * target_{nullptr};
          HashVal check_{0};
          
        public:
          SafeHandle()  = default;
          // default copy operations
          
          /** liveness check */
          operator bool()  const
            {
              return bool(target_)
                 and check_ == retrieveID (*target_);
            }
          
          /* ==== subject access ==== */
          
          SUBJ&
          operator*()  const
            {
              if (not bool(*this))
                throw err::State{"Accessing defunct target"
                                , LERR_(BOTTOM_VALUE)};
              return * target_;
            }
          
          SUBJ*
          operator->()  const
            {
              return & SafeHandle::operator*();
            }
          
        private:
          SafeHandle (SUBJ& target)
            : target_{& target}
            , check_{retrieveID(target)}
            { }
          
          friend class LiveMark;
          
          /** The heart of the implementation: peek into the
           *  target location to see if a valid LiveMark is embedded.
           */
          static HashVal
          retrieveID (SUBJ& target)
            {
              return target.getLiveMark().id_.load (std::memory_order_relaxed);
            }
        };
      
      /**
       * Create a safe smart-handle to refer to the given subject,
       * similar to a raw pointer. Yet the handle will bool-check
       * to `false` when the target object was destroyed
       */
      template<alive_marked SUBJ>
      SafeHandle<SUBJ>
      makeHandle (SUBJ& subject)
        {
          return SafeHandle{subject};
        }
      
    private:
      /** Generate unique HashVal.
       * @remark seeded by a nonce (counter) and the memory address,
       *  which makes it impossible to get the same hash at the same
       *  target address more than once (unless there is a collision
       *  in the 64bit hash space, which is extremely rare)
       */
      template<alive_marked X>
      static HashVal
      generateID (X const& targetAddress)
        {
          static std::atomic_size_t nonce;
          HashVal mark = nonce.fetch_add (1, std::memory_order_relaxed);
          
          hash::combine (mark, util::addrID (targetAddress));
          return mark;
        }
    };
  
  
} // namespace lib
#endif /*LIB_LIVE_MARK_H_*/
