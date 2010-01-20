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
#include "lib/error.hpp"

extern "C" {
#include "backend/threads.h"
}

#include <tr1/functional>
#include <boost/noncopyable.hpp>


namespace backend {
  
  using std::tr1::bind;
  using std::tr1::function;
  using lib::Literal;
  
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
    : boost::noncopyable
  {
    friend class Thread;
    LumieraThread       handle;

    void
    attach (LumieraThread handle_)
    {
      REQUIRE (!handle, "JoinHandle used for several threads.");
      handle = handle_;
    }

  public:
      /** Create a promise, that the current thread will or may
       *  wait-blocking on another not-yet existing thread to terminate.
       *  When passed in on creation of the other thread, as long as this
       *  handle lives, the other thread will be on hold after termination. 
       */
    JoinHandle(LumieraThread handle_ = 0) : handle(handle_) {}

    ~JoinHandle() { if(handle) throw "not joined";}     /* TODO decide if we want auto-joining here? (any way, throwing in dtor isnt very nice) */
    
    /** put the current thread into a blocking wait until another thread
       *  has terminated. This other thread needs to be created by the Thread
       *  wrapper, passing this JoinHandle as ctor parameter.
       *  @throws error::Logic if no thread has been registered to block on this
       */
    void
    join()
    {
      if (!handle)
        throw lumiera::error::Logic ("no thread created blocking on this JoinHandle");

      lumiera_err err = lumiera_thread_join (handle);
      lumiera_error_set(err, 0);

      handle = 0;
      if (err)          /* dont throw here when there is already an error pending (ymmv)*/
        lumiera::throwOnError();
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
    : boost::noncopyable
  {
    LumieraThread       handle;

    typedef function<void(void)> Operation;
    Operation const& operation_;

    static void
    run (void* arg)
    {
      REQUIRE (arg);
      Thread* startingWrapper = reinterpret_cast<Thread*>(arg);
      Operation _doIt_(startingWrapper->operation_);

      lumiera_thread_sync ();   // thread syncronizization point, see lumiera_thread_sync_other() below

      _doIt_(); // execute the actual operation in the new thread
    }

    void
    start_thread (lumiera_thread_class kind, Literal& purpose, NoBugFlag logging_flag, JoinHandle* jh = 0)
    {
      handle =
        lumiera_thread_run ( kind | (jh?LUMIERA_THREAD_JOINABLE:0)      // joinable thread when a join handle is passed
                             , &run                                     // invoking the run helper and..
                             , this                                     // passing this start context as parameter
                             , purpose.c()
                             , logging_flag
                             );

      if (!handle)      /* we dont want to throw here when there was already an error pending */
        lumiera::throwOnError();

      if (jh)
        jh->attach (handle);

      // make sure the new thread had the opportunity to take the Operation
      // prior to leaving and thereby possibly destroying this local context
      lumiera_thread_sync_other (handle);
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
      : operation_(operation)
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
      : operation_(operation)
    {
      start_thread (LUMIERA_THREADCLASS_INTERACTIVE, purpose, logging_flag,
                    &join);
    }

    /**
     * syncronization barrier, the user supplied operation must have a matching lumiera_thread_sync() call
     */
    void
    sync ()
    {
      if (!lumiera_thread_sync_other (handle))
        lumiera::throwOnError();
    }

  };

} // namespace backend
#endif
