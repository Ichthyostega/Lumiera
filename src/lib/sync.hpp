/*
  SYNC.hpp  -  generic helper for object based locking and synchronisation
 
  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>
                        Hermann Vosseler <Ichthyostega@web.de>
 
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

/** @file sync.hpp
 ** Collection of OO-wrappers to support dealing with concurrency issues.
 ** Actually, everything is implemented either by the Lumiera backend, or directly
 ** by pthread. The purpose is to support and automate the most common use cases
 ** in object oriented style; especially we build upon the monitor object pattern.
 ** 
 ** A class becomes \em lockable by inheriting from lib::Sync with the appropriate
 ** parametrisation. This causes any instance to inherit a monitor member (object),
 ** managing a mutex and (optionally) a condition variable for waiting. The actual
 ** synchronisation is achieved by placing a guard object as local (stack) variable
 ** into a given scope (typically a member function body). This guard object of
 ** class lib::Sync::Lock accesses the enclosing object's monitor and automatically
 ** manages the locking and unlocking; optionally it may also be used for waiting
 ** on a condition.
 ** 
 ** Please note:
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
 ** @todo fully implement the NoBug resource tracking calls 
 **
 ** @see mutex.h
 ** @see sync-locking-test.cpp
 ** @see sync-waiting-test.cpp
 ** @see asset::AssetManager::reg() usage example
 ** @see subsystemrunner.hpp usage example
 */


#ifndef LIB_SYNC_H
#define LIB_SYNC_H

#include "include/logging.h"
#include "lib/error.hpp"
#include "lib/util.hpp"

extern "C" {
#include "lib/mutex.h"
#include "lib/condition.h"
#include "lib/reccondition.h"
}

#include <boost/noncopyable.hpp>
#include <pthread.h>
#include <cerrno>
#include <ctime>

using boost::noncopyable;


namespace lib {
    
    
    /** Helpers and building blocks for Monitor based synchronisation */
    namespace sync {
      
      
      struct Wrapped_LumieraExcMutex
        : public lumiera_mutex
        {
        protected:
          Wrapped_LumieraExcMutex() { lumiera_mutex_init    (this, "Obj.Monitor ExclMutex", &NOBUG_FLAG(sync)); }
         ~Wrapped_LumieraExcMutex() { lumiera_mutex_destroy (this, &NOBUG_FLAG(sync)); }
         
         //------------------Resource-Tracking------
         void __may_block() { TODO ("Record we may block on mutex"); }
         void __enter()     { TODO ("Record we successfully acquired the mutex"); }
         void __leave()     { TODO ("Record we are releasing the mutex"); }
        };
      
      
      struct Wrapped_LumieraRecMutex
        : public lumiera_mutex
        {
        protected:
          Wrapped_LumieraRecMutex() { lumiera_recmutex_init (this, "Obj.Monitor RecMutex", &NOBUG_FLAG(sync)); }
         ~Wrapped_LumieraRecMutex() { lumiera_mutex_destroy (this, &NOBUG_FLAG(sync)); }
         
         //------------------Resource-Tracking------
         void __may_block() { TODO ("Record we may block on mutex"); }
         void __enter()     { TODO ("Record we successfully acquired the mutex"); }
         void __leave()     { TODO ("Record we are releasing the mutex"); }
        };
      
      
      struct Wrapped_LumieraExcCond
        : public lumiera_condition
        {
        protected:
          Wrapped_LumieraExcCond() { lumiera_condition_init    (this, "Obj.Monitor ExclCondition", &NOBUG_FLAG(sync) ); }
         ~Wrapped_LumieraExcCond() { lumiera_condition_destroy (this, &NOBUG_FLAG(sync) ); }
         
         //------------------Resource-Tracking------
         void __may_block() { TODO ("Record we may block on mutex"); }
         void __enter()     { TODO ("Record we successfully acquired the mutex"); }
         void __leave()     { TODO ("Record we are releasing the mutex"); }
        };
      
      
      struct Wrapped_LumieraRecCond
        : public lumiera_reccondition
        {
        protected:
          Wrapped_LumieraRecCond() { lumiera_reccondition_init    (this, "Obj.Monitor RecCondition", &NOBUG_FLAG(sync) ); } ////////TODO
         ~Wrapped_LumieraRecCond() { lumiera_reccondition_destroy (this, &NOBUG_FLAG(sync) ); }
         
         //------------------Resource-Tracking------
         void __may_block() { TODO ("Record we may block on mutex"); }
         void __enter()     { TODO ("Record we successfully acquired the mutex"); }
         void __leave()     { TODO ("Record we are releasing the mutex"); }
        };
      
      
      
      /* ========== abstractions defining the usable synchronisation primitives ============== */
      
      template<class MTX>
      class Mutex
        : protected MTX
        {
        protected:
          using MTX::mutex;
          using MTX::__may_block;
          using MTX::__enter;
          using MTX::__leave;
          
         ~Mutex () { }
          Mutex () { }
          Mutex (const Mutex&); ///< noncopyable...
          const Mutex& operator= (const Mutex&);
          
        public:
            void
            acquire()
              {
                __may_block();
                
                if (pthread_mutex_lock (&mutex))
                  throw lumiera::error::State("Mutex acquire failed.");  ///////TODO capture the error-code
                
                __enter();
              }
            
            void
            release()
              { 
                __leave();
                
                pthread_mutex_unlock (&mutex);
              }
            
        };
      
      
      class Timeout;
      
      
      template<class CDX>
      class Condition
        : public Mutex<CDX>
        {
        protected:
          using CDX::cond;
          using CDX::mutex;
          
        public:
          void
          signal (bool wakeAll=false)
            {
              if (wakeAll)
                  pthread_cond_broadcast (&cond);
              else
                  pthread_cond_signal (&cond);
            }
          
          
          template<class BF>
          bool
          wait (BF& predicate, Timeout& waitEndTime)
            {
              int err=0;
              while (!predicate() && !err)
                if (waitEndTime)
                  err = pthread_cond_timedwait (&cond, &mutex, &waitEndTime);
                else
                  err = pthread_cond_wait (&cond, &mutex);
              
              if (!err)           return true;
              if (ETIMEDOUT==err) return false;
              
              throw lumiera::error::State ("Condition wait failed."); ///////////TODO extract error-code
            }
        };
      
      
      /** 
       * helper for specifying an optional timeout for an timed wait.
       * Wrapping a timespec-struct, it allows for easy initialisation
       * by a given relative offset.
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
                  clock_gettime(CLOCK_REALTIME, this);
                  tv_sec   += offs / 1000;
                  tv_nsec  += 1000000 * (offs % 1000);
                  if (tv_nsec > 1000000000)
                    {
                      tv_sec += tv_nsec / 1000000000;
                      tv_nsec %= 1000000000;
                }   }
              return *this;
            }
          
          operator bool() { return 0 != tv_sec; } // allows if (timeout_)....
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
          const Monitor& operator= (Monitor const& ref) { timeout_ = ref.timeout_; }
          
          
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
          wait (X& instance, bool (X::*method)(void), ulong timedwait=0)
            {
              BoolMethodPredicate<X> invokeMethod(instance, method);
              return IMPL::wait(invokeMethod, timeout_.setOffset(timedwait));                                   
            }
          
          void setTimeout(ulong relative) {timeout_.setOffset(relative);}
          bool isTimedWait()              {return (timeout_);}
          
          LumieraCondition accessCond()   {return static_cast<LumieraCondition> (this);}
        };
     
      typedef Mutex<Wrapped_LumieraExcMutex> NonrecursiveLock_NoWait;
      typedef Mutex<Wrapped_LumieraRecMutex> RecursiveLock_NoWait;
      typedef Condition<Wrapped_LumieraExcCond>  NonrecursiveLock_Waitable;
      typedef Condition<Wrapped_LumieraRecCond>  RecursiveLock_Waitable;
      
      
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
    
    
    /*************************************************************************
     * Facility for monitor object based locking. 
     * To be attached either on a per class base or per object base.
     * Typically, the client class will inherit from this template (but it
     * is possible to use it stand-alone, if inheriting isn't an option).
     * The interface for clients to access the functionality is the embedded
     * Lock template, which should be instantiated as an automatic variable
     * within the scope to be protected.
     *  
     */
    template<class CONF = NonrecursiveLock_NoWait>
    class Sync
      {
        typedef sync::Monitor<CONF> Monitor;
        Monitor objectMonitor_;
        
        static Monitor&
        getMonitor(Sync* forThis)
          {
            REQUIRE (forThis);
            return forThis->objectMonitor_;
          }
        
        
      public:
        class Lock
          : private noncopyable
          {
            Monitor& mon_;
            
          public:
            template<class X>
            Lock(X* it) : mon_(getMonitor(it)){ mon_.acquireLock(); }
            ~Lock()                           { mon_.releaseLock(); }
            
            void notify()                     { mon_.signal(false);}
            void notifyAll()                  { mon_.signal(true); }
            void setTimeout(ulong time)       { mon_.setTimeout(time); }
            
            template<typename C>
            bool wait (C& cond, ulong time=0) { return mon_.wait(cond,time);}
            bool isTimedWait()                { return mon_.isTimedWait(); }
            
            
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
