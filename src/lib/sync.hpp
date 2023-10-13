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
    class Condition
      : public Mutex<Wrapped_Condition<MTX>>
      {
        typedef Wrapped_Condition<MTX> Cond;
        
      public:
        void
        signal (bool wakeAll=false)
          {
            if (wakeAll)
              Cond::broadcast ();
            else
              Cond::signal ();
          }
        
        
        /** @return `false` in case of timeout */
        template<class BF>
        bool
        wait (BF& predicate, uint timeout_ms =0)
          {
            while (not predicate())
              if (timeout_ms != 0)
                {
                  if (not Cond::timedwait (std::chrono::milliseconds (timeout_ms)))
                    return false;
                }
              else
                Cond::wait ();
            return true;
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
      {
      public:
        Monitor() {}
        ~Monitor() {}
        
        /** allow copy, without interfering with the identity of IMPL */
        Monitor (Monitor const& ref) : IMPL() { }
        const Monitor& operator= (Monitor const& ref) { /*prevent assignment to base*/ return *this; }
        
        
        void acquireLock() { IMPL::acquire(); }
        void releaseLock() { IMPL::release(); }
        
        void signal(bool a){ IMPL::signal(a); }
        
        bool
        wait (Flag flag, ulong timedwait_ms=0)
          {
            BoolFlagPredicate checkFlag(flag);
            return IMPL::wait(checkFlag, timedwait_ms);
          }
        
        template<class X>
        bool
        wait (X& instance, bool (X::*method)(void), ulong timedwait_ms=0)   /////////////////////TICKET #1051 : add support for lambdas
          {
            BoolMethodPredicate<X> invokeMethod(instance, method);        ///////////////////////TICKET #1057 : const correctness, allow use of const member functions
            return IMPL::wait(invokeMethod, timedwait_ms);
          }
      };
    
    typedef Mutex<Wrapped_ExclusiveMutex> NonrecursiveLock_NoWait;
    typedef Mutex<Wrapped_RecursiveMutex> RecursiveLock_NoWait;
    typedef Condition<Wrapped_ExclusiveMutex>  NonrecursiveLock_Waitable;
    typedef Condition<Wrapped_RecursiveMutex>  RecursiveLock_Waitable;
    
    
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
      typedef sync::Monitor<CONF> Monitor;
      mutable Monitor objectMonitor_;
      
      static Monitor&
      getMonitor(const Sync* forThis)
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
          Lock(X* it) : mon_(getMonitor(it)){ mon_.acquireLock(); }
          ~Lock()                           { mon_.releaseLock(); }
          
          void notify()                     { mon_.signal(false);}
          void notifyAll()                  { mon_.signal(true); }
          
          template<typename C>
          bool
          wait (C& cond, ulong timeout=0)                     //////////////////////////////////////TICKET #1055 : accept std::chrono values here
            {
              return mon_.wait(cond,timeout);
            }
          
          template<typename X>
          bool
          wait  (X& instance, bool (X::*predicate)(void), ulong timeout=0)    //////////////////////TICKET #1051 : enable use of lambdas
            {
              return mon_.wait(instance,predicate,timeout);
            }
          
          /** convenience shortcut:
           *  Locks and immediately enters wait state,
           *  observing a condition defined as member function.
           * @deprecated WARNING this function is not correct!          ////////////////////////////TICKET #1051
           *             Lock is not released on error from within wait()
           */
          template<class X>
          Lock(X* it, bool (X::*method)(void))
            : mon_(getMonitor(it))
            {
              mon_.acquireLock();
              mon_.wait(*it,method);
            }
          
        protected:
          /** for creating a ClassLock */
          Lock(Monitor& m) : mon_(m)
            { mon_.acquireLock(); }
          
          /** for controlled access to the
           * underlying sync primitives */
          Monitor&
          accessMonitor() { return mon_; }
        };
    };
  
  
  
  
} // namespace lumiera
#endif
