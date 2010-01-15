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


#ifndef LIB_THREADWRAPPER_H
#define LIB_THREADWRAPPER_H


#include "include/logging.h"
#include "lib/sync.hpp"

extern "C" {
#include "backend/threads.h"
}

#include <tr1/functional>
#include <boost/noncopyable.hpp>


namespace backend {
  
  using std::tr1::bind;
  using std::tr1::function;
  using lib::Literal;
  using lib::Sync;
  using lib::RecursiveLock_Waitable;
  using lib::NonrecursiveLock_Waitable;
  
  typedef struct nobug_flag* NoBugFlag;
  
  class Thread;
  
  /**
   * Brainstorming-in-code: how I would like to shape the API for joining threads.
   * Intended use: This non-copyable handle has to be created within the thread which
   * wants to wait-blocking on the termination of another thread. You then pass it
   * into the ctor of the Thread starting wrapper class (see below), which causes
   * the embedded lock/condition var to be used to sync on the end of the newly
   * created thread. Note, after ending the execution, the newly created thread
   * will be on hold until either the #join() function is called or this handle
   * goes out of scope altogether. Explanation: this is implemented by locking
   * the embedded monitor immediately in the ctor. Thus, unless entering the
   * wait state, the contained mutex remains locked and prevents the thread
   * manager from invoking the broadcast() on the condition var. 
   * 
   * @see thread-wrapper-join-test.cpp 
   */
  class JoinHandle
    : public Sync<RecursiveLock_Waitable>
    , Sync<RecursiveLock_Waitable>::Lock   // noncopyable, immediately acquire the lock
    {
      typedef Sync<RecursiveLock_Waitable> SyncBase;
      
      bool isWaiting_;
      volatile bool armed_;
      
      friend class Thread;
      
      LumieraReccondition
      accessLockedCondition()
        {
          ASSERT (!armed_, "Lifecycle error, JoinHandle used for several threads.");
          armed_ = true;
          return accessMonitor().accessCond();
        }
      
      bool
      wakeupCheck()
        {
          if (!armed_)
            throw lumiera::error::Logic ("no thread created blocking on this JoinHandle");
          
          if (!isWaiting_)
            {
              isWaiting_ = true;
              return false; // causes entering the blocking wait
            }
          return true;    //   causes end of blocking wait
        }
      
      
    public:
      /** Create a promise, that the current thread will or may
       *  wait-blocking on another not-yet existing thread to terminate.
       *  When passed in on creation of the other thread, as long as this
       *  handle lives, the other thread will be on hold after termination. 
       */
      JoinHandle()
        : SyncBase::Lock(this)
        , isWaiting_(false)
        , armed_(false)
        { }
      
      /** put the current thread into a blocking wait until another thread
       *  has terminated. This other thread needs to be created by the Thread
       *  wrapper, passing this JoinHandle as ctor parameter.
       *  @throws error::Logic if no thread has been registered to block on this
       */ 
      void
      join()
        {
          accessMonitor().wait (&handle, *this, &JoinHandle::wakeupCheck);
        }
    };
  
  
  
  
  /****************************************************************************
   * A thin convenience wrapper for dealing with threads,
   * as implemented by the backend (on top of pthread).
   * Using this wrapper...
   * - helps with passing data to the function executed in the new thread
   * - allows to bind to various kinds of functions including member functions
   * - supports integrating with an existing object monitor based lock (planned)
   * The new thread starts immediately within the ctor; after returning, the new
   * thread has already copied the arguments and indeed actively started to run.
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
  class Thread
    : public Sync<NonrecursiveLock_Waitable>
    , boost::noncopyable
    {
      volatile bool started_;
      
      typedef function<void(void)> Operation;
      Operation const& operation_;
      
      static void
      run (void* arg)
        { 
          REQUIRE (arg);
          Thread* startingWrapper = reinterpret_cast<Thread*>(arg);
          Operation _doIt_(startingWrapper->operation_);
          {
            Lock sync(startingWrapper);
            startingWrapper->started_ = true;
            sync.notify(); // handshake signalling we've gotten the parameter
          }
          
          _doIt_(); // execute the actual operation in the new thread
        }
      
      
      void
      start_thread (lumiera_thread_class kind, Literal& purpose, NoBugFlag logging_flag, LumieraReccondition joinCond=0)
        {
          Lock sync(this);
          LumieraThread res = 
          lumiera_thread_run ( kind
                             , &run         // invoking the run helper and..
                             , this         // passing this start context as parameter
                             , joinCond     // maybe wait-blocking for the thread to terminate
                             , purpose.c()
                             , logging_flag
                             );
          
          if (!res)
            throw lumiera::error::State("failed to create new thread.");
          
          // make sure the new thread had the opportunity to take the Operation
          // prior to leaving and thereby possibly destroying this local context
          sync.wait (started_);
        }
        
    public:
      /** Create a new thread to execute the given operation.
       *  The new thread starts up synchronously, i.e. when the ctor returns, the new thread
       *  has started running and taken over (copied) the operation functor passed in. The
       *  thread will be created by lumiera_thread_run (declared in threads.h), it can't 
       *  be cancelled and it can't be joined.
       *  @param purpose fixed char string used to denote the thread for diagnostics
       *  @param logging_flag NoBug flag to receive diagnostics regarding the new thread
       *  @param operation defining what to execute within the new thread. Any functor
       *         which can be bound to function<void(void)>. Note this functor will be
       *         copied onto the stack of the new thread, thus it can be transient.
       * 
       */
      Thread (Literal purpose, Operation const& operation, NoBugFlag logging_flag = &NOBUG_FLAG(thread))
        : started_(false),
          operation_(operation)
        {
          start_thread (LUMIERA_THREADCLASS_INTERACTIVE, purpose, logging_flag);
        }
      
      /** Variant of the standard case, used to register a JoinHandle in addition to starting a thread.
       *  @param join ref to a JoinHandle, which needs to be created in the thread which plans
       *         to wait-blocking on the termination of this newly created thread 
       * 
       */
      Thread (Literal purpose, Operation const& operation, 
              JoinHandle& join, NoBugFlag logging_flag = &NOBUG_FLAG(thread))
        : started_(false),
          operation_(operation)
        {
          start_thread (LUMIERA_THREADCLASS_INTERACTIVE, purpose, logging_flag, 
                        join.accessLockedCondition());
        }
    };
  
  
  
 } // namespace backend
#endif
