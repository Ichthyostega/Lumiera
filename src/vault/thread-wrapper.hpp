/*
  THREADWRAPPER.hpp  -  thin convenience wrapper for starting lumiera threads

  Copyright (C)         Lumiera.org
    2008, 2010          Hermann Vosseler <Ichthyostega@web.de>
                        Christian Thaeter <ct@pipapo.org>

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


/** @file thread-wrapper.hpp
 ** Convenience front-end for basic thread handling needs.
 ** The Lumiera vault contains a dedicated low-level thread handling framework,
 ** which is relevant for scheduling render activities to make best use of parallelisation
 ** abilities of the given system. Typically, the upper layers should not have to deal much
 ** with thread handling, yet at some point there is the need to implement a self contained
 ** action running within a dedicated thread. The vault::Thread class is a wrapper to
 ** represent such an parallel action conveniently and safely; together with the object
 ** monitor, this allows to abstract away intricacies into self contained objects.
 ** 
 ** @deprecated will be replaced by a thin wrapper on top of C++17 threads     //////////////////////////////TICKET #1279 : consolidate to C++17 features 
 */


#ifndef LIB_THREADWRAPPER_H
#define LIB_THREADWRAPPER_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "include/logging.h"
#include "lib/meta/function.hpp"
#include "lib/result.hpp"

extern "C" {
#include "vault/threads.h"
}
#include "vault/threadpool-init.hpp"

#include <type_traits>
#include <utility>


namespace vault {
  
  using lib::Literal;
  namespace error = lumiera::error;
  using error::LERR_(STATE);
  using error::LERR_(EXTERNAL);
  
  typedef struct nobug_flag* NoBugFlag;
  
  
  
  /************************************************************************//**
   * A thin convenience wrapper for dealing with threads,
   * as implemented by the threadpool in the vault (based on pthread).
   * Using this wrapper...
   * - helps with passing data to the function executed in the new thread
   * - allows to bind to various kinds of functions including member functions
   * The new thread starts immediately within the ctor; after returning, the new
   * thread has already copied the arguments and indeed actively started to run.
   * 
   * # Joining, cancellation and memory management
   * In the basic version (class Thread), the created thread is completely detached
   * and not further controllable. There is no way to find out its execution state,
   * wait on termination or even cancel it. Client code needs to implement such
   * facilities explicitly, if needed. Care has to be taken with memory management,
   * as there are no guarantees beyond the existence of the arguments bound into
   * the operation functor. If the operation in the started thread needs additional
   * storage, it has to manage it actively.
   * 
   * There is an extended version (class ThreadJoinable) to allow at least to wait
   * on the started thread's termination (joining). Building on this it is possible
   * to create a self-contained "thread in an object"; the dtor of such an class
   * must join to prevent pulling away member variables the thread function will
   * continue to use.
   * 
   * # failures in the thread function
   * The operation started in the new thread is protected by a top-level catch block.
   * Error states or caught exceptions can be propagated through the lumiera_error
   * state flag, when using ThreadJoinable::join(). By invoking `join().maybeThrow()`
   * on a join-able thread, exceptions can be propagated.
   * @note any errorstate or caught exception detected on termination of a standard
   * async Thread is considered a violation of policy and will result in emergency
   * shutdown of the whole application.
   * 
   * # synchronisation barriers
   * Lumiera threads provide a low-level synchronisation mechanism, which is used
   * to secure the hand-over of additional arguments to the thread function. It
   * can be used by client code, but care has to be taken to avoid getting out
   * of sync. When invoking the #sync and #syncPoint functions, the caller will
   * block until the counterpart has also invoked the corresponding function.
   * If this doesn't happen, you'll block forever.
   * @deprecated will be replaced by a thin wrapper on top of C++17 threads     /////////////////////////////TICKET #1279 : consolidate to C++17 features 
   */
  class Thread
    : util::MoveOnly
    {
      /** @internal perfect forwarding through a C-style `void*` */
      template<class FUN>
      static FUN&&
      forwardInitialiser (void* rawPtr) noexcept
        {
          REQUIRE (rawPtr);
          FUN& initialiser = *reinterpret_cast<FUN*> (rawPtr);
          return static_cast<FUN&&> (initialiser);
        }
      
      
      template<class FUN>
      static void
      threadMain (void* arg)
        {
          using Fun= typename lib::meta::_Fun<FUN>::Functor;
          Fun _doIt_{forwardInitialiser<FUN> (arg)};
          
          lumiera_thread_sync (); // sync point: arguments handed over
          
          try {
              _doIt_(); // execute the actual operation in the new thread
            }
          
          catch (std::exception& failure)
            {
              if (!lumiera_error_peek())
                LUMIERA_ERROR_SET (sync, STATE
                                  ,failure.what());
            }
          catch (...)
            {
              LUMIERA_ERROR_SET_ALERT (sync, EXTERNAL
                                      , "Thread terminated abnormally");
            }
        }
      
      
    protected:
      LumieraThread threadHandle_;
      
      /** @internal derived classes may create an inactive thread */
      Thread() : threadHandle_(0) { }
      
      
      /** @internal use the Lumiera thread manager to start a new thread and hand over the operation */
      template<class FUN>
      void
      launchThread (Literal purpose, FUN&& operation, NoBugFlag logging_flag, uint additionalFlags =0)
        {
          REQUIRE (!lumiera_error(), "Error pending at thread start");
          using Functor = typename std::remove_reference<FUN>::type;
          threadHandle_ =
            lumiera_thread_run ( LUMIERA_THREADCLASS_INTERACTIVE | additionalFlags
                               , &threadMain<Functor>
                               , reinterpret_cast<void*> (&operation)
                               , purpose.c()
                               , logging_flag
                               );
          if (!threadHandle_)
            throw error::State ("Failed to start a new Thread for \"+purpose+\""
                               , lumiera_error());
          
          // make sure the new thread had the opportunity to take the Operation
          // prior to leaving and thereby possibly destroying this local context
          lumiera_thread_sync_other (threadHandle_);
        }
      
      
      
    public:
      /** Create a new thread to execute the given operation.
       *  The new thread starts up synchronously, can't be cancelled and it can't be joined.
       *  @param purpose fixed char string used to denote the thread for diagnostics
       *  @param logging_flag NoBug flag to receive diagnostics regarding the new thread
       *  @param operation a functor holding the code to execute within the new thread.
       *         Any function-like entity with signature `void(void)` is acceptable.
       *  @warning The operation functor will be forwarded to create a copy residing
       *         on the stack of the new thread; thus it can be transient, however
       *         anything referred through a lambda closure here must stay alive
       *         until the new thread terminates.
       */
      template<class FUN>
      Thread (Literal purpose, FUN&& operation, NoBugFlag logging_flag = &NOBUG_FLAG(thread))
        : threadHandle_{nullptr}
        {
          launchThread (purpose, std::forward<FUN> (operation), logging_flag);
        }
      
      
      /** @note by design there is no possibility to find out
       *  just based on the thread handle if some thread is alive.
       *  We define our own accounting here based on the internals
       *  of the thread wrapper. This will break down, if you mix
       *  uses of the C++ wrapper with the raw C functions. */
      bool
      isValid()  const
        {
          return threadHandle_;
        }
      
      
      /** Synchronisation barrier. In the function executing in this thread
       *  needs to be a corresponding Thread::syncPoint() call. Blocking until
       *  both the caller and the thread have reached the barrier.
       */
      void
      sync()
        {
          REQUIRE (isValid(), "Thread not running");
          if (!lumiera_thread_sync_other (threadHandle_))
            lumiera::throwOnError();
        }
      
      /** counterpart of the synchronisation barrier, to be called from
       *  within the thread to be synchronised. Will block until both
       *  this thread and the outward partner reached the barrier.
       * @warning blocks on the _current_ thread's condition var
       */
      static void
      syncPoint ()
        {
          lumiera_thread_sync ();
        }
      
    protected:
      /** determine if the currently executing code runs within this thread */
      bool
      invokedWithinThread()  const
        {
          REQUIRE (isValid(), "Thread not running");
          LumieraThread current = lumiera_thread_self ();
          return current
             and current == this->threadHandle_;
        }
    };
  
  
  
  
  
  
  /**
   * Variant of the standard case, allowing additionally
   * to join on the termination of this thread.
   * @deprecated will be replaced by a thin wrapper on top of C++17 threads     /////////////////////////////TICKET #1279 : consolidate to C++17 features 
   */
  class ThreadJoinable
    : public Thread
    {
    public:
      template<class FUN>
      ThreadJoinable (Literal purpose, FUN&& operation,
                      NoBugFlag logging_flag = &NOBUG_FLAG(thread))
        : Thread{}
        {
          launchThread<FUN> (purpose, std::forward<FUN> (operation), logging_flag,
                             LUMIERA_THREAD_JOINABLE);
        }
      
      
      /** put the caller into a blocking wait until this thread has terminated.
       *  @return token signalling either success or failure.
       *          The caller can find out by invoking `isValid()`
       *          or `maybeThrow()` on this result token
       */
      lib::Result<void>
      join ()
        {
          if (!isValid())
            throw error::Logic ("joining on an already terminated thread");
          
          lumiera_err errorInOtherThread =
              lumiera_thread_join (threadHandle_);
          threadHandle_ = 0;
          
          if (errorInOtherThread)
            return error::State ("Thread terminated with error", errorInOtherThread);
          else
            return true;
        }
    };
  
  
  
} // namespace vault
#endif /*LIB_THREADWRAPPER_H*/
