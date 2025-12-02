/*
  SYNC.hpp  -  generic helper for object based locking and synchronisation

   Copyright (C)
     2007,            Christian Thaeter <ct@pipapo.org>
     2008,            Hermann Vosseler <Ichthyostega@web.de>
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file sync.hpp
 ** Object Monitor based synchronisation.
 ** The actual locking, signalling and waiting is implemented by delegating to the
 ** a _mutex_ and for waiting also to a _condition variable_ as provided by the C++
 ** standard library. The purpose of the Sync baseclass is to provide a clear and simple
 ** „everyday“ concurrency coordination based on the *Object Monitor Pattern*. This
 ** pattern describes a way of dealing with synchronisation known to play well with
 ** scoping, encapsulation and responsibility for a single purpose. For performance
 ** critical code, other solutions (e.g. Atomics) might be preferable.
 ** 
 ** # Usage
 ** 
 ** A class becomes _lockable_ by inheriting from lib::Sync with the appropriate
 ** parametrisation. This causes any instance to inherit a monitor member (object),
 ** maintaining a dedicated a mutex and (optionally) a condition variable for waiting.
 ** The actual synchronisation is achieved by placing a guard object as local (stack)
 ** variable into a given scope (typically a member function body). This guard object
 ** of class lib::Sync::Lock accesses the enclosing object's monitor and automatically
 ** manages the locking and unlocking; optionally it may also be used to perform a
 ** [wait-on-condition] — the call to `Lock::wait(predicate)` will first check the
 ** predicate, and if it does not yield `true`, the thread will be put to sleep.
 ** It must be awakened from another thread by invoking `notify_one|all` and will
 ** then re-check the condition predicate. The `wait_for` variant allows to set
 ** a timeout to limit the sleep state, which implies however that the call may
 ** possibly return `false` in case the condition predicate is not (yet) fulfilled.
 ** @note
 ** - It is important to select a suitable parametrisation of the monitor.
 **   This is done by specifying one of the defined policy classes.
 ** - Be sure to pick the recursive mutex implementation when recursive calls
 **   of synchronised functions can't be avoided. (performance penalty)
 ** - You can't use the Lock#wait and Lock#notify functions unless you pick
 **   a parametrisation including a condition variable.
 ** - The "this" pointer is fed to the ctor of the Lock guard object. Thus
 **   you may use any object's monitor as appropriate, especially in cases
 **   when adding the monitor to a given class may cause size problems.
 ** - There is a special variant of the Lock guard called ClassLock, which
 **   can be used to lock based on a type, not an instance.
 **
 ** [wait-on-condition]: https://en.cppreference.com/w/cpp/thread/condition_variable_any/wait
 ** @see mutex.h
 ** @see sync-locking-test.cpp
 ** @see sync-waiting-test.cpp
 ** @see \ref steam::control::DispatcherLoop "usage example: receiving session coomands"
 ** @see \ref subsystem-runner.hpp "usage example: subsystem start/shutdown"
 */


#ifndef LIB_SYNC_H
#define LIB_SYNC_H

#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <mutex>
#include <condition_variable>
#include <chrono>


namespace lib {
  
  /** Helpers and building blocks for Monitor based synchronisation */
  namespace sync {
    
    /* ========== policies to define required degree of locking functionality ============== */
    
    template<class MTX>
    struct Condition
      : MTX
      ,  std::condition_variable_any
      {
        
        template<class PRED>
        void
        wait (PRED&& predicate)
          {
            condVar().wait (mutex(), std::forward<PRED> (predicate));
          }
        
        /** @return `false` in case of timeout,
         *          `true` if predicate is fulfilled at return
         */
        template<class REPR, class PERI, class PRED>
        bool
        wait_for (std::chrono::duration<REPR, PERI> const& timeout, PRED&& predicate)
          {
            return condVar().wait_for (mutex(), timeout, std::forward<PRED> (predicate));
          }
        
        MTX&
        mutex()
          {
            return static_cast<MTX&> (*this);
          }
        std::condition_variable_any&
        condVar()
          {
            return static_cast<std::condition_variable_any&> (*this);
          }
      };
    
    struct NoLocking
      {
        void lock()            { /* boo */ }
        void unlock() noexcept { /* hoo */ }
      };
    
    
    
    /* ========== Object Monitor ============== */
    
    /**
     * Object Monitor for synchronisation and waiting.
     * Implemented by a (wrapped) set of sync primitives,
     * which are default constructible and noncopyable.
     */
    template<class IMPL>
    class Monitor
      : IMPL
      , util::NonCopyable
      {
      public:
        
        void lock()                { IMPL::lock(); }
        void unlock()     noexcept { IMPL::unlock(); }
        
        void notify_one() noexcept { IMPL::notify_one(); }
        void notify_all() noexcept { IMPL::notify_all(); }
        
        template<class PRED>
        void
        wait (PRED&& predicate)
          {
            IMPL::wait (std::forward<PRED>(predicate));
          }
        
        template<class DUR, class PRED>
        bool
        wait_for (DUR const& timeout, PRED&& predicate)
          {
            return IMPL::wait_for (timeout, std::forward<PRED> (predicate));
          }
      };
    
    using NonrecursiveLock_NoWait   = std::mutex;
    using    RecursiveLock_NoWait   = std::recursive_mutex;
    using NonrecursiveLock_Waitable = Condition<std::mutex>;
    using    RecursiveLock_Waitable = Condition<std::recursive_mutex>;
    
    
  } // namespace sync (helpers and building blocks)
  
  
  
  
  
  /* Interface to be used by client code:
   * Inherit from class Sync with a suitable Policy.
   * Then use the embedded Lock class.
   */
  
  /* =======  Policy classes  ======= */
  
  using sync::NonrecursiveLock_NoWait;
  using sync::NonrecursiveLock_Waitable;
  using sync::RecursiveLock_NoWait;
  using sync::RecursiveLock_Waitable;
  
  
  /*********************************************************************//**
   * Facility for monitor object based locking.
   * To be attached either on a per class base or per object base.
   * Typically, the client class will inherit from this template (but it
   * is possible to use it stand-alone, if inheriting isn't an option).
   * The interface for clients to access the functionality is the embedded
   * Lock template, which should be instantiated as an automatic variable
   * within the scope to be protected.
   * 
   * # Usage
   * - for *locking*, just place an instant of the embedded Lock into the local
   *   scope to be protected. All lock instances within the same object share
   *   the monitor; thus any time, only one of them gets the mutex all other
   *   instances block on construction.
   * - for *waiting* likewise place an instance of the Lock class (which gets
   *   you a mutex). Then invoke the wait method on that instance; this suspends
   *   the current thread and releases the mutex. To awake and check the condition,
   *   some other thread must invoke the Lock::notify() within the same object.
   *   The Lock::wait() call returns `true` when the condition was met, and
   *   `false` if awakened due to timeout. The call might throw in case of
   *   technical errors. In any case, when returning from the `wait()` call,
   *   the mutex has been re-acquired.
   */
  template<class CONF = NonrecursiveLock_NoWait>
  class Sync
    {
      using Monitor = sync::Monitor<CONF>;
      mutable Monitor objectMonitor_;
      
    public:
      static Monitor&
      getMonitor(Sync const* forThis)
        {
          REQUIRE (forThis);
          return forThis->objectMonitor_;
        }
      
      
      /*****************************************//**
       * scoped guard to control the actual locking.
       */
      class Lock
        : util::NonCopyable
        {
          Monitor& mon_;
          
        public:
          template<class X>
          Lock(X* it) : mon_{getMonitor(it)}{ mon_.lock(); }
         ~Lock()                            { mon_.unlock(); }
          
          void notify_one()                 { mon_.notify_one(); }
          void notify_all()                 { mon_.notify_all(); }
          
          template<class PRED>
          void
          wait (PRED&& predicate)
            {
              mon_.wait (std::forward<PRED>(predicate));
            }
          
          template<class DUR, class PRED>
          bool
          wait_for (DUR const& timeout, PRED&& predicate)
            {
              return mon_.wait_for (timeout, std::forward<PRED> (predicate));
            }
          
          /** convenience shortcut:
           *  Locks and immediately enters wait state on the given predicate
           */
          template<class X, class PRED>
          Lock(X* it, PRED&& predicate)
            : mon_(getMonitor(it))
            {
              mon_.lock();
              try {
                  mon_.wait (std::forward<PRED>(predicate));
                }
              catch(...)
                {
                  mon_.unlock();
                  throw;
                }
            }
          
        protected:
          /** for creating a ClassLock */
          Lock(Monitor& m)
            : mon_{m}
            {
              mon_.lock();
            }
          
          /** subclass access to underlying sync primitives */
          Monitor& accessMonitor() { return mon_; }
        };
    };
  
  
} // namespace lumiera
#endif
