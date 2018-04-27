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

extern "C" {
#include "lib/lockerror.h"
}

#include <pthread.h>
#include <cerrno>
#include <ctime>



namespace lib {
  
  /** Helpers and building blocks for Monitor based synchronisation */
  namespace sync {
    
    
    
    
    /* ========== adaptation layer for accessing backend/system level code ============== */
    
    struct Wrapped_ExclusiveMutex
      {
        pthread_mutex_t mutex_;
        
      protected:
        Wrapped_ExclusiveMutex()
          {
            pthread_mutex_init (&mutex_, NULL);
          }
       ~Wrapped_ExclusiveMutex()
          {
            if (pthread_mutex_destroy (&mutex_))
              ERROR (sync, "Failure destroying mutex.");
          }               // shouldn't happen in a correct program
        
        void
        lock()
          {
            if (pthread_mutex_lock (&mutex_))
              throw lumiera::error::Fatal ("Mutex acquire failed");
          }               // shouldn't happen in a correct program
        
        void
        unlock()
          {
            if (pthread_mutex_unlock (&mutex_))
              ERROR (sync, "Failure unlocking mutex.");
          }               // shouldn't happen in a correct program
      };
    
    
    
    struct Wrapped_RecursiveMutex
      {
        pthread_mutex_t mutex_;
        
      protected:
        Wrapped_RecursiveMutex();
       ~Wrapped_RecursiveMutex()
          {
            if (pthread_mutex_destroy (&mutex_))
              ERROR (sync, "Failure destroying (rec)mutex.");
          }               // shouldn't happen in a correct program
        
        void
        lock()
          {
            if (pthread_mutex_lock (&mutex_))
              throw lumiera::error::Fatal ("(rec)Mutex acquire failed");
          }               // shouldn't happen in a correct program
        
        void
        unlock()
          {
            if (pthread_mutex_unlock (&mutex_))
              ERROR (sync, "Failure unlocking (rec)mutex.");
          }               // shouldn't happen in a correct program
      };
    
    
    template<class MTX>
    struct Wrapped_Condition
      : MTX
      {
         pthread_cond_t cond_;
        
      protected:
        Wrapped_Condition()
          {
            pthread_cond_init (&cond_, NULL);
          }
       ~Wrapped_Condition()
          {
            if (pthread_cond_destroy (&cond_))
              ERROR (sync, "Failure destroying condition variable.");
          }               // shouldn't happen in a correct program
        
        bool
        wait()
          {
            int err;
            do { err = pthread_cond_wait (&this->cond_, &this->mutex_);
            } while(err == EINTR);
            
            if (err) lumiera_lockerror_set (err, &NOBUG_FLAG(sync), NOBUG_CONTEXT_NOFUNC);
            return not err;
          }
        
        bool
        timedwait (const struct timespec* timeout)
          {
            int err;
            do { err = pthread_cond_timedwait (&this->cond_, &this->mutex_, timeout);
            } while(err == EINTR);
              
            if (err) lumiera_lockerror_set (err, &NOBUG_FLAG(sync), NOBUG_CONTEXT_NOFUNC);
            return not err;
          }
        
        
        void signal()    { pthread_cond_signal (&cond_); }
        void broadcast() { pthread_cond_broadcast (&cond_); }
      };
    
    
    
    
    
    
    
    /* ========== abstractions defining the usable synchronisation primitives ============== */
    
    template<class MTX>
    class Mutex
      : protected MTX
      {
      protected:
       ~Mutex () { }
        Mutex () { }
        Mutex (const Mutex&); ///< noncopyable...
        const Mutex& operator= (const Mutex&);
        
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
    
    
    
    /**
     * helper for specifying an optional timeout for an timed wait.
     * Wrapping a timespec-struct, it allows for easy initialisation
     * by a given relative offset.
     * @todo integrate with std::chrono                                //////////////////////////TICKET #1055
     */
    struct Timeout
      : timespec
      {
        Timeout() { tv_sec=tv_nsec=0; }
        
        /** initialise to NOW() + offset (in milliseconds) */
        Timeout&
        setOffset (ulong offs)
          {
            if (offs)
              {
                clock_gettime(CLOCK_REALTIME, this);                   //////////////////////////TICKET #886
                tv_sec   += offs / 1000;
                tv_nsec  += 1000000 * (offs % 1000);
                if (tv_nsec >= 1000000000)
                  {
                    tv_sec += tv_nsec / 1000000000;
                    tv_nsec %= 1000000000;
              }   }
            return *this;
          }
        
        operator bool() { return 0 != tv_sec; } // allows if (timeout_)....
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
        
        
        template<class BF>
        bool
        wait (BF& predicate, Timeout& waitEndTime)
          {
            bool ok = true;
            while (ok and !predicate())
              if (waitEndTime)
                ok = Cond::timedwait (&waitEndTime);
              else
                ok = Cond::wait ();
            
            if (not ok and lumiera_error_expect(LUMIERA_ERROR_LOCK_TIMEOUT)) return false;
            lumiera::throwOnError();     // any other error throws
            
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
        Timeout timeout_;
        
      public:
        Monitor() {}
        ~Monitor() {}
        
        /** allow copy, without interfering with the identity of IMPL */
        Monitor (Monitor const& ref) : IMPL(), timeout_(ref.timeout_) { }
        const Monitor& operator= (Monitor const& ref) { timeout_ = ref.timeout_; return *this; }
        
        
        void acquireLock() { IMPL::acquire(); }
        void releaseLock() { IMPL::release(); }
        
        void signal(bool a){ IMPL::signal(a); }
        
        bool
        wait (Flag flag, ulong timedwait=0)
          {
            BoolFlagPredicate checkFlag(flag);
            return IMPL::wait(checkFlag, timeout_.setOffset(timedwait));
          }
        
        template<class X>
        bool
        wait (X& instance, bool (X::*method)(void), ulong timedwait=0)    ///////////////////////TICKET #1051 : add support for lambdas
          {
            BoolMethodPredicate<X> invokeMethod(instance, method);        ///////////////////////TICKET #1057 : const correctness, allow use of const member functions
            return IMPL::wait(invokeMethod, timeout_.setOffset(timedwait));
          }
        
        void setTimeout(ulong relative) {timeout_.setOffset(relative);}
        bool isTimedWait()              {return (timeout_);}
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
   * ## Usage
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
          void setTimeout(ulong time)       { mon_.setTimeout(time); }
          bool isTimedWait()                { return mon_.isTimedWait(); }
          
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
           *  observing a condition defined as member function. */
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
