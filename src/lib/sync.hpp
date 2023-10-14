/*
  SYNC.hpp  -  generic helper for object based locking and synchronisation

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>
                        Hermann Vosseler <Ichthyostega@web.de>

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

/** @file sync.hpp
 ** Object Monitor based synchronisation.
 ** The actual locking, signalling and waiting is implemented by delegating to the
 ** raw pthreads locking/sync calls. Rather, the purpose of the Sync baseclass is
 ** to support locking based on the <b>object monitor pattern</b>. This pattern
 ** describes a way of dealing with synchronisation known to play well with
 ** scoping, encapsulation and responsibility for a single purpose.
 ** 
 ** A class becomes _lockable_ by inheriting from lib::Sync with the appropriate
 ** parametrisation. This causes any instance to inherit a monitor member (object),
 ** managing a mutex and (optionally) a condition variable for waiting. The actual
 ** synchronisation is achieved by placing a guard object as local (stack) variable
 ** into a given scope (typically a member function body). This guard object of
 ** class lib::Sync::Lock accesses the enclosing object's monitor and automatically
 ** manages the locking and unlocking; optionally it may also be used for waiting
 ** on a condition.
 ** 
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
 ** - For sake of completeness, this implementation provides the ability for
 **   timed waits. But please consider that in most cases there are better
 **   solutions for running an operation with given timeout by utilising the
 **   Lumiera scheduler. Thus use of timed waits is \b discouraged.
 ** - There is a special variant of the Lock guard called ClassLock, which
 **   can be used to lock based on a type, not an instance.
 ** - in DEBUG mode, the implementation includes NoBug resource tracking.
 ** 
 ** @todo WIP-WIP 10/2023 switch from POSIX to C++14  ///////////////////////////////////////////////////////TICKET #1279 : also clean-up the Object-Monitor implementation
 ** @see mutex.h
 ** @see sync-locking-test.cpp
 ** @see sync-waiting-test.cpp
 ** @see \ref asset::AssetManager::reg() "usage example: asset registration"
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
    
    
    
    
    /* ========== adaptation layer for accessing backend/system level code ============== */
    
    struct Wrapped_ExclusiveMutex
      : util::NonCopyable
      {
        std::mutex mutex_;
        
      protected:
        Wrapped_ExclusiveMutex() = default;
        
        void
        lock()
          {
            mutex_.lock();
          }
        
        void
        unlock()
          {
            mutex_.unlock();
          }
      };
    
    
    
    struct Wrapped_RecursiveMutex
      : util::NonCopyable
      {
        std::recursive_mutex mutex_;
        
      protected:
        Wrapped_RecursiveMutex() = default;
        
        void
        lock()
          {
            mutex_.lock();
          }
        
        void
        unlock()
          {
            mutex_.unlock();
          }
      };
    
    
    template<class MTX>
    struct Wrapped_Condition
      : MTX
      {
         std::condition_variable_any cond_;
        
      protected:
        Wrapped_Condition() = default;
        
        void
        wait()
          {
            cond_.wait (this->mutex_);
          }

        template<class REPR, class PERI>
        bool
        timedwait (std::chrono::duration<REPR, PERI> const& timeout)
          {
            auto ret = cond_.wait_for (this->mutex_, timeout);
            return (std::cv_status::no_timeout == ret);
          }
        
        void signal()    { cond_.notify_one(); }
        void broadcast() { cond_.notify_all(); }
      };
    
    
    
    
    
    
    
    /* ========== abstractions defining the usable synchronisation primitives ============== */
    
    template<class MTX>
    class Mutex
      : protected MTX
      {
      protected:
        Mutex () = default;
        
      public:
          void
          acquire()
            {
              MTX::lock ();
            }
          
          void
          release()
            {
              MTX::unlock ();
            }
      };
    
    
    
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
    
    
    
    
    /* ==== functor types for defining the waiting condition ==== */
    
    typedef volatile bool& Flag;
    
    struct BoolFlagPredicate
      {
        Flag flag_;
        BoolFlagPredicate (Flag f) : flag_(f) {}
       
        bool operator() () { return flag_; }
      };
    
    
    template<class X>
    struct BoolMethodPredicate
      {
        typedef bool (X::*Method)(void);
        
        X& instance_;
        Method method_;
        BoolMethodPredicate (X& x, Method m) : instance_(x), method_(m) {}
       
        bool operator() () { return (instance_.*method_)(); }
      };
    
    
    
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
      
      static Monitor&
      getMonitor(Sync const* forThis)
        {
          REQUIRE (forThis);
          return forThis->objectMonitor_;
        }
      
      
    public:
      /*****************************************//**
       * scoped object to control the actual locking.
       */
      class Lock
        : util::NonCopyable
        {
          Monitor& mon_;
          
        public:
          template<class X>
          Lock(X* it) : mon_(getMonitor(it)){ mon_.lock(); }
         ~Lock()                            { mon_.unlock(); }
          
          void notify()                     { mon_.notify_one(); }
          void notifyAll()                  { mon_.notify_all(); }
          
          template<typename C>
          bool
          wait (C&& cond, ulong timeout_ms=0)                     //////////////////////////////////////TICKET #1055 : accept std::chrono values here
            {
              if (timeout_ms)
                return mon_.wait_for (std::chrono::milliseconds(timeout_ms)
                                     ,std::forward<C> (cond));
              else
                {
                  mon_.wait (std::forward<C> (cond));
                  return true;
                }
            }
          
          template<typename X>
          bool
          wait  (X& instance, bool (X::*predicate)(void), ulong timeout_ms=0)    //////////////////////TICKET #1051 : enable use of lambdas
            {
              return wait([&]{ return (instance.*predicate)(); }, timeout_ms);
            }
          
          /** convenience shortcut:
           *  Locks and immediately enters wait state,
           *  observing a condition defined as member function.
           * @deprecated WARNING this function is not correct!          ////////////////////////////TICKET #1051
           *             Lock is not released on error from within wait()  /////TODO is actually fixed now; retain this API??
           */
          template<class X>
          Lock(X* it, bool (X::*method)(void))
            : mon_(getMonitor(it))
            {
              mon_.lock();
              try { wait(*it,method); }
              catch(...)
                {
                  mon_.unlock();
                  throw;
                }
            }
          
        protected:
          /** for creating a ClassLock */
          Lock(Monitor& m)
            : mon_(m)
            {
              mon_.lock();
            }
          
          /** subclass access to underlying sync primitives */
          Monitor& accessMonitor() { return mon_; }
        };
    };
  
  
  
  
} // namespace lumiera
#endif
