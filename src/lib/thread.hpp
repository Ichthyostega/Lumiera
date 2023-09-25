/*
  THREAD.hpp  -  thin convenience wrapper for starting threads

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


/** @file thread.hpp
 ** Convenience front-end to simplify and codify basic thread handling.
 ** While the implementation of threading and concurrency support is based on the C++
 ** standard library, using in-project wrappers as front-end allows to codify some preferences
 ** and provide simplifications for the prevalent use case. Notably, threads which must be
 ** _joined_ are qualified as special case, while the standard case will just `detach()`
 ** at thread end. The main-level of each thread catches exceptions, which are typically
 ** ignored to keep the application running. Moreover, similar convenience wrappers are
 ** provided to implement [N-fold synchronisation](\ref lib::SyncBarrier) and to organise
 ** global locking and waiting in accordance with the _Object Monitor_ pattern. Together,
 ** these aim at packaging concurrency facilities into self-contained RAII-style objects.
 ** @remarks
 **  - Lumiera offered simplified convenience wrappers long before a similar design
 **    became part of the C++14 standard. These featured the distinction in join-able or
 **    detached threads, the ability to define the thread main-entry as functor, and a
 **    two-fold barrier between starter and new thread, which could also be used to define
 **    a second custom synchronisation point. A similar setup with wrappers was provided
 **    for locking, exposed in the form of the Object Monitor pattern.
 **  - The original Render Engine design called for an active thread-pool, which was part
 **    of a invoker service located in Vault layer; the thread-wrapper could only be used
 **    in conjunction with this pool, re-using detached and terminated threads. All features
 **    where implemented in plain-C on top of POSIX, using Mutexes and Condition Variables.
 **  - In 2023, when actually heading towards integration of the Render Engine, in-depth
 **    analysis showed that active dispatch into a thread pool would in fact complicate
 **    the scheduling of Render-Activities — leading to a design change towards _pull_
 **    of work tasks by competing _active workers._ This obsoleted the Thread-pool service
 **    and paved the way for switch-over to the threading support meanwhile part of the
 **    C++ standard library. Design and semantics were retained, while implemented
 **    using modern features, notably the new _Atomics_ synchronisation framework.
 ** @todo WIP 9/23 about to be replaced by a thin wrapper on top of C++17 threads     ///////////////////////TICKET #1279 : consolidate to C++17 features 
 */


#ifndef LIB_THREAD_H
#define LIB_THREAD_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "include/logging.h"
#include "lib/meta/function.hpp"
#include "lib/format-string.hpp" ///////////////////////////OOO RLY? or maybe into CPP file?
#include "lib/result.hpp"


#include <thread>
#include <utility>


namespace lib {
  
  using lib::Literal;
  namespace error = lumiera::error;
  using error::LERR_(STATE);
  using error::LERR_(EXTERNAL);
  
  typedef struct nobug_flag* NoBugFlag;
  
  
  
  /************************************************************************//**
   * A thin convenience wrapper to simplify thread-handling. The implementation
   * is backed by the C++ standard library.
   * Using this wrapper...
   * - removes the need to join() threads, catches and ignores exceptions.
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
   */
  class Thread
    : util::MoveOnly
    {
      
      template<class FUN, typename...ARGS>
      void
      threadMain (string threadID, FUN&& threadFunction, ARGS&& ...args)
        {
          try {
              markThreadStart (threadID);
              //  execute the actual operation in this new thread
              std::invoke (std::forward<FUN> (threadFunction), std::forward<ARGS> (args)...);
            }
          ERROR_LOG_AND_IGNORE (thread, "Thread function")
        }
      
      void
      markThreadStart (string const& threadID)
        {
          string logMsg = util::_Fmt{"Thread '%s' start..."} % threadID;
          TRACE (thread, "%s", logMsg.c_str());
          //////////////////////////////////////////////////////////////////////OOO maybe set the the Thread-ID via POSIX ??
        }
      
    protected:
      std::thread threadImpl_;
      
      /** @internal derived classes may create an inactive thread */
      Thread() : threadImpl_{} { }
      
      
    public:
      /** Create a new thread to execute the given operation.
       *  The new thread starts up synchronously, can't be cancelled and it can't be joined.
       *  @param threadID human readable descriptor to identify the thread for diagnostics
       *  @param logging_flag NoBug flag to receive diagnostics regarding the new thread
       *  @param operation a functor holding the code to execute within the new thread.
       *         Any function-like entity with signature `void(void)` is acceptable.
       *  @warning The operation functor will be forwarded to create a copy residing
       *         on the stack of the new thread; thus it can be transient, however
       *         anything referred through a lambda closure here must stay alive
       *         until the new thread terminates.
       */
      template<class FUN, typename...ARGS>
      Thread (string const& threadID, FUN&& threadFunction, ARGS&& ...args)
        : threadImpl_{&Thread::threadMain<FUN,ARGS...>, this
                     , threadID
                     , std::forward<FUN> (threadFunction)
                     , std::forward<ARGS> (args)... }
        { }
      
      
      /**
       * Is this thread »active« and thus tied to OS resources?
       * @note this implies some statefulness, which may contradict the RAII pattern.
       *       - especially note the possibly for derived classes to create an _empty_ Thread.
       *       - moreover note that ThreadJoinable may have terminated, but still awaits `join()`.
       */
      explicit
      operator bool()  const
        {
          return threadImpl_.joinable();
        }
      
      
      
    protected:
      /** determine if the currently executing code runs within this thread */
      bool
      invokedWithinThread()  const
        {
          return threadImpl_.get_id() == std::this_thread::get_id();
        }     // Note: implies get_id() != std::thread::id{} ==> it is running
    };
  
  
  
  
  
  
  /**
   * Variant of the standard case, allowing additionally
   * to join on the termination of this thread.
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
//          launchThread<FUN> (purpose, std::forward<FUN> (operation), logging_flag,
//                             LUMIERA_THREAD_JOINABLE);
        }
      
      
      /** put the caller into a blocking wait until this thread has terminated.
       *  @return token signalling either success or failure.
       *          The caller can find out by invoking `isValid()`
       *          or `maybeThrow()` on this result token
       */
      lib::Result<void>
      join ()
        {
//          if (!isValid())
//            throw error::Logic ("joining on an already terminated thread");
//          
//          lumiera_err errorInOtherThread =
//              "TODO TOD-oh";//lumiera_thread_join (threadHandle_);            //////////////////////////////////OOO
//          threadHandle_ = 0;
//          
//          if (errorInOtherThread)
//            return error::State ("Thread terminated with error", errorInOtherThread);
//          else
//            return true;
        }
    };
  
  
  
} // namespace lib
#endif /*LIB_THREAD_H*/
