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
 ** 
 ** # Usage
 ** Based on experience, there seem to be two fundamentally different usage patterns for
 ** thread-like entities: In most cases, they are just launched to participate in interactions
 ** elsewhere defined. However, sometimes dedicated sub-processing is established and supervised,
 ** finally to join results. And while the underlying implementation supports both usage styles,
 ** a decision was made to reflect this dichotomy by casting two largely distinct front-ends.
 ** 
 ** The »just launch it« scheme is considered the default and embodied into lib::Thread.
 ** Immediately launched on construction using the given _Invokable Functor_ and binding arguments,
 ** it is not meant to be managed further, beyond possibly detecting the live-ness state through
 ** `bool`-check. Exceptions propagating to top level within the new thread will be catched and
 ** ignored, terminating and discarding the thread. Note however, since especially derived
 ** classes can be used to create a safe anchor and working space for the launched operations,
 ** it must be avoided to discard the Thread object while still operational; as a matter of
 ** design, it should be assured the instance object outlives the enclosed chain of activity.
 ** As a convenience, the destructor blocks for a short timespan of 20ms; a thread running
 ** beyond that grace period will kill the whole application by `std::terminate`.
 ** 
 ** For the exceptional case when a supervising thread need to await the termination of
 ** launched threads, a different front-end \ref lib::ThreadJoinable is provided, exposing
 ** the `join()` operation. Such threads *must* be joined however, and thus the destructor
 ** immediately terminates the application in case the thread is still running.
 ** 
 ** ## Synchronisation
 ** The C++ standard provides that the end of the `std::thread` constructor _syncs-with_ the
 ** start of the new thread function, and likewise the end of the thread activity _syncs-with_
 ** the return from `join()`. According to the [syncs-with definition], this implies the
 ** _happens before_ relation and thus precludes a data race. In practice thus
 ** - the new thread function can access all data defined prior to ctor invocation
 ** - the caller of `join()` is guaranteed to see all effects of the terminated thread.
 ** Note however, that these guarantees do not extend into the initialisations performed
 ** in a derived class's constructor, which start only after leaving the ctor of Thread.
 ** So in theory there is a possible race between the extended setup in derived classes,
 ** and the use of these facilities from within the thread function. In practice the new
 ** thread, while already marked as live, still must be scheduled by the OS to commence,
 ** which does not completely remove the possibility of undefined behaviour however. So
 ** in cases where a race could be critical, additional means must be implemented; a
 ** possible solution would be to use a [N-fold synchronisation barrier](\ref lib::SyncBarrier)
 ** explicitly, or otherwise to ensure there is sufficient delay in the starting thread function.
 ** 
 ** @remarks Historical design evolution:
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
 ** [syncs-with definition] : https://en.cppreference.com/w/cpp/atomic/memory_order#Synchronizes_with
 ** @todo WIP 9/23 about to be replaced by a thin wrapper on top of C++17 threads     ///////////////////////TICKET #1279 : consolidate to C++17 features 
 */


#ifndef LIB_THREAD_H
#define LIB_THREAD_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "include/logging.h"
#include "lib/meta/function.hpp"

#include <thread>
#include <string>
#include <utility>


namespace util {
  std::string sanitise (std::string const&);
}
namespace lib {

  using std::string;
  
  
  
  namespace thread {// Thread-wrapper base implementation...
    
    struct ThreadWrapper
      : util::MoveOnly
      {
        const string threadID_;
        std::thread threadImpl_;
        
        bool isLive()  const { return threadImpl_.joinable(); }
        
        
        /** @internal derived classes may create an inactive thread */
        ThreadWrapper()
          : threadID_{util::BOTTOM_INDICATOR}
          , threadImpl_{}
          { }
        
        template<typename...ARGS>
        ThreadWrapper (string const& threadID, ARGS&& ...args)
          : threadID_{util::sanitise (threadID)}
          , threadImpl_{std::forward<ARGS> (args)... }
          { }
        
        /** determine if the currently executing code runs within this thread */
        bool invokedWithinThread()  const;
        
        void markThreadStart();
        void markThreadEnd  ();
        void setThreadName  ();
        void waitGracePeriod()  noexcept;
      };
    
    
    template<class BAS>
    struct PolicyTODO
      : BAS
      {
        using BAS::BAS;
        
        void
        handle_thread_still_running()
          {
            BAS::waitGracePeriod();
          }
      };
    
    /**
     * Policy-based configuration of thread lifecycle
     */
    template<template<class> class POL>
    class ThreadLifecycle
      : protected POL<ThreadWrapper>
      {
        using Policy = POL<ThreadWrapper>;
        
        template<class FUN, typename...ARGS>
        void
        main (FUN&& threadFunction, ARGS&& ...args)
          {
            Policy::markThreadStart();
            try {
                 //  execute the actual operation in this new thread
                std::invoke (std::forward<FUN> (threadFunction), std::forward<ARGS> (args)...);
              }
            ERROR_LOG_AND_IGNORE (thread, "Thread function")
            //
            Policy::markThreadEnd();
//            if (autoTerm)
              Policy::threadImpl_.detach();
          }
        
        
      protected:
       ~ThreadLifecycle()
          {
            if (Policy::isLive())
              Policy::handle_thread_still_running();
          }
        
      public:
        /**
         * Is this thread »active« and thus tied to OS resources?
         * @note this implies some statefulness, which may contradict the RAII pattern.
         *       - especially note the possibly for derived classes to create an _empty_ Thread.
         *       - moreover note that ThreadJoinable may have terminated, but still awaits `join()`.
         */
        explicit
        operator bool()  const
          {
            return Policy::isLive();
          }
        
        
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
        ThreadLifecycle (string const& threadID, FUN&& threadFunction, ARGS&& ...args)
          : Policy{threadID
                  , &ThreadLifecycle::main<FUN,ARGS...>, this
                  , std::forward<FUN> (threadFunction)
                  , std::forward<ARGS> (args)... }
          { }
        
        
      };
    
  }//(End)base implementation.
  
  
  /************************************************************************//**
   * A thin convenience wrapper to simplify thread-handling. The implementation
   * is backed by the C++ standard library.
   * Using this wrapper...
   * - removes the need to join() threads, catches and ignores exceptions.
   * - allows to bind to various kinds of functions including member functions
   * The new thread starts immediately within the ctor; after returning, the new
   * thread has already copied the arguments and indeed actively started to run.
   * @warning The destructor waits for a short grace period of 20ms, but calls
   *          `std::terminate` afterwards, should the thread still be active then.
   */
  class Thread
    : public thread::ThreadLifecycle<thread::PolicyTODO>
    {
      
    public:
      using ThreadLifecycle::ThreadLifecycle;
    };
  
  
  
  
  
  
  /************************************************************************//**
   * Variant of the [standard case](\ref Thread), requiring to wait and `join()`
   * on the termination of this thread. Useful to collect results calculated
   * by multiple threads. Note however that the system resources of the thread
   * are kept around until the `join()` call, and thus also the `bool` conversion
   * yields `true`, even while the actual operation has already terminated.
   * @warning Thread must be joined prior to destructor invocation, otherwise
   *          the application is shut down immediately via `std::terminate`.
   */
  class ThreadJoinable
    : public thread::ThreadLifecycle<thread::PolicyTODO>
    {
    public:
      using ThreadLifecycle::ThreadLifecycle;
      
      /** put the caller into a blocking wait until this thread has terminated */
      void
      join ()
        {
          if (not threadImpl_.joinable())
            throw lumiera::error::Logic ("joining on an already terminated thread");
          
          threadImpl_.join();
        }
    };
  
  
  
} // namespace lib
#endif /*LIB_THREAD_H*/
