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
 ** While the implementation of threading and concurrency support is based on the
 ** C++ standard library, using in-project wrappers as front-end allows to codify some
 ** references and provide simplifications for the prevalent use case. Notably, threads
 ** which must be _joined_ are qualified as special case, while the standard case will
 ** just `detach()` at thread end. The main-level of each thread catches exceptions, which
 ** are typically ignored to keep the application running. Moreover, similar convenience
 ** wrappers are provided to implement [N-fold synchronisation](\ref lib::SyncBarrier)
 ** and to organise global [locking and waiting](\ref lib::Sync) in accordance with the
 ** _Object Monitor_ pattern. In concert, these allow to package concurrency facilities
 ** into self-contained RAII-style objects.
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
 ** such a thread is not meant to be managed further, beyond possibly detecting the live-ness state
 ** through `bool`-check. Exceptions propagating to top level within the new thread will be coughed
 ** and ignored, terminating and discarding the thread. Note however, since especially derived
 ** classes can be used to create a safe anchor and working space for the launched operations,
 ** it must be avoided to destroy the Thread object while still operational; as a matter of
 ** design, it should be assured the instance object outlives the enclosed chain of activity.
 ** As a convenience, the destructor blocks for a short timespan of 20ms; a thread running
 ** beyond that grace period will kill the whole application by `std::terminate`.
 ** 
 ** For the exceptional case when a supervising thread need to await the termination of
 ** launched threads, a different front-end \ref lib::ThreadJoinable is provided, exposing
 ** the `join()` operation. This operation returns a [»Either« wrapper](\ref lib::Result),
 ** to transport the return value and possible exceptions from the thread function to the
 ** caller. Such threads *must* be joined however, and thus the destructor immediately
 ** terminates the application in case the thread is still running.
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
 **
 ** [syncs-with definition] : https://en.cppreference.com/w/cpp/atomic/memory_order#Synchronizes_with
 */


#ifndef LIB_THREAD_H
#define LIB_THREAD_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "include/logging.h"
//#include "lib/builder-qualifier-support.hpp"//////////////////////////TODO
#include "lib/format-util.hpp"
#include "lib/result.hpp"

#include <thread>
#include <string>
#include <utility>


namespace util {
  std::string sanitise (std::string const&);
}
namespace lib {

  using std::string;
  
  
  
  namespace thread {// Thread-wrapper base implementation...
    
    using lib::meta::typeSymbol;
    using std::forward;
    using std::decay_t;
    using std::invoke_result_t;
    using std::is_constructible;
    using std::is_same;
    using std::__or_;
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////OOO extract -> lib/meta
    /**
     * Metaprogramming helper to mark some arbitrary base type by subclassing.
     * In most respects the _specially marked type_ behaves like the base; this
     * can be used to mark some element at compile time, e.g. to direct it into
     * a specialisation or let it pick some special overload.
     */
    template<class BAS, size_t m=0>
    struct Marked
      : BAS
      {
        using BAS::BAS;
      };
    ////////////////////////////////////////////////////////////////////////////////////////////////////OOO extract(End)
    /** @internal wraps the C++ thread handle
     *   and provides some implementation details,
     *   which are then combined by the _policy template_
     */
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
          , threadImpl_{forward<ARGS> (args)... }
          { }
        
        /** detect if the currently executing code runs within this thread */
        bool invokedWithinThread()  const;
        
        void markThreadStart(string);
        void markThreadEnd  (string);
        void setThreadName  ();
        void waitGracePeriod()  noexcept;
      };
    
    
    
    /**
     * Thread Lifecycle Policy:
     * - launch thread without further control
     * - errors in thread function will only be logged
     * - thread detaches before terminating
     * - »grace period« for thread to terminate on shutdown
     */
    template<class BAS, typename>
    struct PolicyLaunchOnly
      : BAS
      {
        using BAS::BAS;
        
        template<class FUN, typename...ARGS>
        void
        perform_thread_function(FUN&& callable, ARGS&& ...args)
          {
            try {
                 //  execute the actual operation in this new thread
                std::invoke (forward<FUN> (callable), forward<ARGS> (args)...);
              }
            ERROR_LOG_AND_IGNORE (thread, "Thread function")
          }
        
        void
        handle_end_of_thread()
          {
            if (BAS::isLive())
              BAS::threadImpl_.detach();
          }
        
        void
        handle_thread_still_running()
          {
            BAS::waitGracePeriod();
          }
      };
    
    
    /**
     * Thread Lifecycle Policy:
     * - thread with the ability to publish results
     * - return value from the thread function will be stored
     * - errors in thread function will likewise be captured and retained
     * - thread *must* be joined after termination of the thread function
     * @warning unjoined thread on dtor call will be a fatal error (std::terminate)
     */
    template<class BAS, typename RES>
    struct PolicyResultJoin
      : BAS
      {
        using BAS::BAS;
        
        /** Wrapper to capture a success/failure indicator and possibly a computation result */
        lib::Result<RES> result_{error::Logic{"No result yet, thread still running; need to join() first."}};
        
        
        template<class FUN, typename...ARGS>
        void
        perform_thread_function(FUN&& callable, ARGS&& ...args)
          {
            static_assert (__or_<is_same<RES,void>
                                ,is_constructible<RES, invoke_result_t<FUN,ARGS...>>>());
            
            // perform the given operation (failsafe) within this thread and capture result...
            result_ = std::move (
                        lib::Result{forward<FUN>(callable)
                                   ,forward<ARGS>(args)...});
          }
        
        void
        handle_end_of_thread()
          {
            /* do nothing -- thread must be joined manually */;
          }
        
        void
        handle_thread_still_running()
          {
            ALERT (thread, "Thread '%s' was not joined. Abort.", BAS::threadID_.c_str());
          }
      };
    
    
    /**
     * Policy-based configuration of thread lifecycle
     */
    template<template<class,class> class POL, typename RES =void>
    class ThreadLifecycle
      : protected POL<ThreadWrapper, RES>
//      , public lib::BuilderQualifierSupport<ThreadLifecycle<POL,RES>>
      {
        using Policy = POL<ThreadWrapper,RES>;
//        using Qualifier = typename lib::BuilderQualifierSupport<ThreadLifecycle<POL,RES>>::Qualifier;
        
        template<typename...ARGS>
        void
        invokeThreadFunction (ARGS&& ...args)
          {
            string id{Policy::threadID_}; // local copy
            Policy::markThreadStart(id);
            Policy::perform_thread_function (forward<ARGS> (args)...);
            Policy::markThreadEnd(id);
            Policy::handle_end_of_thread();
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
        
        /** @return does this call happen from within this thread? */
        using Policy::invokedWithinThread;
        
        
        /** Create a new thread to execute the given operation.
         *  The new thread starts up synchronously, can't be cancelled and it can't be joined.
         *  @param threadID human readable descriptor to identify the thread for diagnostics
         *  @param threadFunction a functor holding the code to execute within the new thread.
         *         Any function-like entity or callable is acceptable; arguments can be given.
         *  @warning The operation functor and all arguments will be copied into the new thread.
         *         The return from this constructor _syncs-with_ the launch of the operation.
         */
        template<class FUN, typename...ARGS>
        ThreadLifecycle (string const& threadID, FUN&& threadFunction, ARGS&& ...args)
          : Policy{threadID
                  , &ThreadLifecycle::invokeThreadFunction<FUN, decay_t<ARGS>...>, this
                  , forward<FUN> (threadFunction)
                  , decay_t<ARGS> (args)... }                                        // Note: need to decay the argument types
          { }                                                                       //        (arguments must be copied)
        
        template<class SUB, typename...ARGS>
        ThreadLifecycle (RES (SUB::*memFun) (ARGS...), ARGS ...args)
          : ThreadLifecycle{util::joinDash (typeSymbol<SUB>(), args...)
                           ,std::move (memFun)
                           ,static_cast<SUB*> (this)
                           ,forward<ARGS> (args)... }
          { }
        
        struct ConfigBuilder
          : util::MoveOnly
          {
            using Launcher = std::function<void(ThreadLifecycle&)>;
            Launcher launch;
            
            template<class FUN, typename...ARGS>
            ConfigBuilder (FUN&& threadFunction, ARGS&& ...args)
              : launch{[=]
                       (ThreadLifecycle& wrapper)
                          {
                            wrapper.threadImpl_
                              = std::thread{&ThreadLifecycle::invokeThreadFunction<FUN, decay_t<ARGS>...>
                                           , &wrapper
                                           , std::move(threadFunction)
                                           , std::move(args)... };
                          }}
              { }
            
            ConfigBuilder&&
            threadID (string const& id)
              {
                launch = [=, chain=std::move(launch)]
                         (ThreadLifecycle& wrapper)
                            {
                              util::unConst(wrapper.threadID_) = id;
                              chain (wrapper);
                            };
                return move(*this);
              }
          };
        
        ThreadLifecycle (ConfigBuilder launcher)
          : Policy{}
          { 
            launcher.launch (*this);
          }
      };
    
  }//(End)base implementation.
  
  
  
  /************************************************************************//**
   * A thin convenience wrapper to simplify thread-handling.
   * The implementation is backed by the C++ standard library.
   * Using this wrapper...
   * - removes the need to join() threads, catches and ignores exceptions.
   * - allows to bind to various kinds of functions including member functions
   * The new thread starts immediately within the ctor; after returning, the new
   * thread has already copied the arguments and indeed actively started to run.
   * @warning The destructor waits for a short grace period of 20ms, but calls
   *          `std::terminate` afterwards, should the thread still be active then.
   */
  class Thread
    : public thread::ThreadLifecycle<thread::PolicyLaunchOnly>
    {
      
    public:
      using ThreadLifecycle::ThreadLifecycle;
      
      /** allow to detach explicitly — independent from thread-function's state
       * @warning this function is borderline dangerous; it might be acceptable
       *          in a situation where the thread totally manages itself and the
       *          thread object is maintained in a unique_ptr. You must ensure that
       *          the thread function only uses storage within its own scope.
       * @deprecated can't sleep well while this function is exposed;
       *          need a prime solution to address this relevant use case ////////////////////////////////////////OOO allow for a thread with explicit lifecycle
       */
      void detach() { ThreadLifecycle::handle_end_of_thread(); }
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
  template<typename RES =void>
  class ThreadJoinable
    : public thread::ThreadLifecycle<thread::PolicyResultJoin, RES>
    {
      using Impl = thread::ThreadLifecycle<thread::PolicyResultJoin, RES>;
      
    public:
      using Impl::Impl;
      
      /**
       * put the caller into a blocking wait until this thread has terminated
       * @return intermediary token signalling either success or failure.
       *     The caller can find out by invoking `isValid()` or `maybeThrow()`
       *     on this result token. Moreover, if the _thread function_ yields a
       *     result value, this value is copied into the token and can be retrieved
       *     either by type conversion, or with `get<TY>()`, `value_or(default)`
       *     or even with an alternative producer `or_else(λ)`.
       */
      lib::Result<RES>
      join ()
        {
          if (not Impl::threadImpl_.joinable())
            throw lumiera::error::Logic ("joining on an already terminated thread");
          
          Impl::threadImpl_.join();
          
          return Impl::result_;
        }
    };

  /** deduction guide: find out about result value to capture from a generic callable. */
  template<typename FUN, typename...ARGS>
  ThreadJoinable (string const&, FUN&&, ARGS&&...) -> ThreadJoinable<std::invoke_result_t<FUN,ARGS...>>;
  
  
  
} // namespace lib
#endif /*LIB_THREAD_H*/
