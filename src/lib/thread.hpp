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
 ** A further variant ThreadHookable allows to attach user-provided callbacks invoked from
 ** the thread lifecycle; this can be used to build a thread-object that manages itself
 ** autonomously, or a thread that opens / closes interfaces tied to its lifecycle.
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
 ** ##Caveat
 ** While these thread-wrapper building blocks aim at packaging the complexity away, there is
 ** the danger to miss a potential race, which is inherent with starting threads: the operation
 ** in the new thread contends with any initialisation done _after_ launching the thread. Even
 ** though encapsulating complex concurrent logic into an opaque component, as built on top
 ** of the thread-wrappers, is highly desirable from a code sanity angle — it is dangerously
 ** tempting to package self-contained data initialisation into a subclass, leading to the
 ** kind of _undefined behaviour,_ which „can never happen“ under normal circumstances.
 ** Even while the OS scheduler typically adds an latency of at least 100µs to the start
 ** of the new thread function, initialising anything (even subclass data members) after
 ** creating the thread-wrapper instance *is undefined behaviour*. As a remedy
 ** - it should be considered to put the thread-warpper into a _member_ (instead of inheriting)
 ** - an explicit lib::SyncBarrier can be added, to ensure the thread-function touches any
 **   extended facilities only after the initialisation is complete (as a downside, note that
 **   any hard synchronisation adds a possibility for deadlock).
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
#include "lib/meta/trait.hpp"
#include "lib/meta/function.hpp"
#include "lib/format-util.hpp"
#include "lib/result.hpp"

#include <utility>
#include <thread>
#include <string>
#include <tuple>


namespace util {
  std::string sanitise (std::string const&);
}
namespace lib {

  using std::string;
  using std::tuple;
  
  
  
  namespace thread {// Thread-wrapper base implementation...
    
    using lib::meta::typeSymbol;
    using lib::meta::_Fun;
    using util::isnil;
    using std::function;
    using std::forward;
    using std::move;
    using std::decay_t;
    using std::invoke_result_t;
    using std::is_constructible;
    using std::make_from_tuple;
    using std::tuple_cat;
    using std::is_same;
    using std::__or_;
    
    /**
     * @internal wraps the C++ thread handle
     * and provides some implementation details,
     * which are then combined by the _policy template_
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
        
        ThreadWrapper (string const& threadID)
          : threadID_{isnil(threadID)? "sub-thread" : util::sanitise (threadID)}
          , threadImpl_{} //Note: deliberately not starting the thread yet...
          { }
        
        /** @internal actually launch the new thread.
         * Deliberately the #threadImpl_ is created empty, to allow for complete
         * initialisation of all the combined policy classes stacked on top
         * @warning Start of the new thread _syncs-with_ the return from std::thread ctor.
         *          Thus -- in theory -- initialising members of derived classes after constructing
         *          a non-empty std::thread object would be *undefined behaviour*. In practice however,
         *          this is more of a „theoretical“ problem, since the OS scheduler has a considerable
         *          latency, so that the code within the new thread typically starts executing with an
         *          delay of _at least 100µs_
         * @remark non the less, the thread-wrapper framework circumvents this possible undefined behaviour,
         *          by first creating the threadImpl_ empty, and only later move-assigning the std::thread.
         * @param invocation a tuple holding some invokable and possible arguments, together forming the
         *          threadFunction to be executed in the new thread.
         */
        template<class...INVO>
        void
        launchThread (tuple<INVO...>&& invocation)
          {
            ASSERT (not isLive(), "Thread already running");
            threadImpl_ = make_from_tuple<std::thread> (invocation);
          };
        
        
        /** detect if the currently executing code runs within this thread */
        bool invokedWithinThread()  const;
        
        void markThreadStart();
        void markThreadEnd  ();
        void setThreadName  ();
        void waitGracePeriod()  noexcept;
        
        /* empty implementation for some policy methods */
        void handle_begin_thread() { }   ///< called immediately at start of thread
        void handle_after_thread() { }   ///< called immediately before end of thread
        void handle_loose_thread() { }   ///< called when destroying wrapper on still running thread
      };
    
    
    
    /**
     * Thread Lifecycle Policy:
     * - launch thread without further control
     * - errors in thread function will only be logged
     * - thread detaches before terminating
     * - »grace period« for thread to terminate on shutdown
     */
    template<class BAS, typename=void>
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
        handle_after_thread()
          {
            if (BAS::isLive())
              BAS::threadImpl_.detach();
          }
        
        void
        handle_loose_thread()
          {
            BAS::waitGracePeriod();
          }
      };
    
    
    /**
     * Thread Lifecycle Policy Extension:
     * invoke user-provided callbacks from within thread lifecycle.
     * @see ThreadLifecycle::invokeThreadFunction for invocation...
     * @see ThreadLifecycle::Launch for configuration of these hooks.
     */
    template<class BAS, class TAR>
    struct PolicyLifecycleHook
      : PolicyLaunchOnly<BAS>
      {
        using BasePol = PolicyLaunchOnly<BAS>;
        using BasePol::BasePol;
        
        using Self = PolicyLifecycleHook;
        using Hook = function<void(Self&)>;
        
        Hook hook_beginThread{};
        Hook hook_afterThread{};
        Hook hook_looseThread{};
        
        void
        handle_begin_thread()
          {
            if (hook_beginThread)
              hook_beginThread (*this);
            else
              BasePol::handle_begin_thread();
          }
        
        void
        handle_after_thread()
          {
            if (hook_afterThread)
              hook_afterThread (*this);
            if (BAS::isLive())  // Note: ensure thread is detached at end
              BAS::threadImpl_.detach();
          }
        
        void
        handle_loose_thread()
          {
            if (hook_looseThread)
              hook_looseThread (*this);
            else
              BasePol::handle_loose_thread();
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
        handle_after_thread()
          {
            /* do nothing -- thread must be joined manually */;
          }
        
        void
        handle_loose_thread()
          {
            ALERT (thread, "Thread '%s' was not joined. Abort.", BAS::threadID_.c_str());
          }
      };
    
    template<class TAR>
    struct InstancePlaceholder { };
    
    template<class W, class INVO>
    inline INVO
    lateBindInstance (W&, INVO invocation)
    {
      return invocation;
    }
    
    template<class W, class F, class TAR, typename...ARGS>
    inline auto
    lateBindInstance (W& instance, const tuple<F, InstancePlaceholder<TAR>, ARGS...> invocation)
    {
      auto splice = [&instance](auto f, auto&, auto ...args)
                      {
                        TAR* instancePtr = static_cast<TAR*> (&instance);
                        return tuple{move(f), instancePtr, move(args)...};
                      };
      return std::apply (splice, invocation);
    }
    
    /**
     * Policy-based configuration of thread lifecycle
     */
    template<template<class,class> class POL, typename RES =void>
    class ThreadLifecycle
      : protected POL<ThreadWrapper, RES>
      {
        using Policy = POL<ThreadWrapper,RES>;
        
        template<typename...ARGS>
        void
        invokeThreadFunction (ARGS&& ...args)
          {
            if (not Policy::isLive()) return;
            Policy::handle_begin_thread();
            Policy::markThreadStart();
            Policy::perform_thread_function (forward<ARGS> (args)...);
            Policy::markThreadEnd();
            Policy::handle_after_thread();
          }
        
        
      protected:
       ~ThreadLifecycle()
          {
            if (Policy::isLive())
              Policy::handle_loose_thread();
          }
        
        /** derived classes may create a disabled thread */
        ThreadLifecycle()
          : Policy{}
          { }
        
      public:
        /**
         * Build the invocation tuple, using #invokeThreadFunction
         * to delegate to the user-provided functor and arguments
         */
        template<class W, class...INVO>
        static auto
        buildInvocation (W& wrapper, tuple<INVO...>&& invocation)
        {                                           //the thread-main function
          return tuple_cat (tuple{&ThreadLifecycle::invokeThreadFunction<INVO...>
                                 , &wrapper}      //  passing the wrapper as instance-this
                           ,move (invocation));  //...invokeThreadFunction() in turn delegates
        }                                       //    to the user-provided thread-operation
        
        /**
         * Build a λ actually to launch the given thread operation later,
         * after the thread-wrapper-object is fully initialised.
         * The member function #invokeThreadFunction will be run as top-level
         * within the new thread, possibly handling errors, but delegating to
         * the user-provided actual thread-operation
         * @param args a invokable + further arguments
         * @note the invokable and the arguments will be materialised/copied
         *       thereby decaying the given type; this is necessary, because
         *       these arguments must be copied into the new thread. This will
         *       fail to compile, if the given invokable can not be invoked
         *       with these copied (and decayed) arguments.
         */
        template<class...INVO>
        static auto
        buildLauncher (INVO&& ...args)
        {
          tuple<decay_t<INVO>...> argCopy{forward<INVO> (args)...};
          return [invocation = move(argCopy)]// Note: functor+args bound by-value into the λ
                 (ThreadLifecycle& wrapper)
                    {                        // special treatment for launchDetached
                      auto boundInvocation = lateBindInstance (wrapper, move (invocation));
                      wrapper.launchThread (buildInvocation (wrapper, move(boundInvocation)));
                    };
        }
        
        
        /**
         * Configuration builder to define the operation running within the thread,
         * and possibly configure further details, depending on the actual Policy used.
         * @remark the primary ThreadLifecycle-ctor accepts such a Launch-instance
         *         and invokes a chain of λ-functions collected in the member #launch
         */
        struct Launch
          : util::MoveOnly
          {
            using Act = function<void(ThreadLifecycle&)>;
            
            Act launch;
            string id;
            
            template<class FUN, typename...ARGS>
            Launch (FUN&& threadFunction, ARGS&& ...args)
              : launch{buildLauncher (forward<FUN>(threadFunction), forward<ARGS>(args)...)}
              { }
            
            Launch&&
            threadID (string const& threadID)
              {
                id = threadID;
                return move(*this);
              }
            
            template<typename HOOK>
            Launch&&
            atStart (HOOK&& hook)
              {
                return addHook (&Policy::hook_beginThread, forward<HOOK> (hook));
              }
            
            template<typename HOOK>
            Launch&&
            atExit (HOOK&& hook)
              {
                return addHook (&Policy::hook_afterThread, forward<HOOK> (hook));
              }
            
            template<typename HOOK>
            Launch&&
            onOrphan (HOOK&& hook)
              {
                return addHook (&Policy::hook_looseThread, forward<HOOK> (hook));
              }
            
          private:
            /**
             * Helper to adapt a user provided hook to be usable as lifecycle hook.
             * @tparam HOOK type of the user provided λ or functor
             * @tparam FUN  type of the function maintained in #PolicyLifecycleHook
             * @note the user provided functor can take any type as argument, which
             *       is reachable by static cast from the thread-wrapper. Especially
             *       this allows both for low-level and userclass-internal hooks.
             */
            template<typename HOOK, class FUN>
            auto
            adaptedHook (FUN Policy::*, HOOK&& hook)
              {
                static_assert(1 == _Fun<FUN>::ARITY);
                static_assert(1 >= _Fun<HOOK>::ARITY);
                // argument type expected by the hooks down in the policy class
                using Arg = typename _Fun<FUN>::Args::List::Head;
                // distinguish if user provided functor takes zero or one argument
                if constexpr (0 == _Fun<HOOK>::ARITY)
                  return [hook = forward<HOOK>(hook)](Arg){ hook(); };
                else
                  { // instance type expected by the user-provided hook
                    using Target = typename _Fun<HOOK>::Args::List::Head;
                    return [hook = forward<HOOK>(hook)]
                           (Arg& threadWrapper)
                              { // build a two-step cast path from the low-level wrapper to user type
                                ThreadLifecycle& base = static_cast<ThreadLifecycle&> (threadWrapper);
                                Target&        target = static_cast<Target&> (base);
                                hook (target);
                              };
                  }
              }
            
            /** add a config layer to store a user-provided functor into the polic baseclass(es) */
            template<typename HOOK, class FUN>
            Launch&&
            addHook (FUN Policy::*storedHook, HOOK&& hook)
              {
                return addLayer ([storedHook, hook = adaptedHook (storedHook, forward<HOOK> (hook))]
                                 (ThreadLifecycle& wrapper)
                                    {
                                      wrapper.*storedHook = move (hook);
                                    });
              }
            
            /** generic helper to add another »onion layer« to this config builder */
            Launch&&
            addLayer (Act action)
              {
                launch = [action=move(action), chain=move(launch)]
                         (ThreadLifecycle& wrapper)
                            {
                              action(wrapper);
                              chain (wrapper);
                            };
                return move(*this);
              }
          };
        
        
        /**
         * Primary constructor: Launch the new thread with flexible configuration.
         * @param launcher a #Launch builder with a λ-chain to configure and
         *        finally trigger start of the thread
         */
        ThreadLifecycle (Launch launcher)
          : Policy{launcher.id}
          {
            launcher.launch (*this);
          }
        
        /**
         * Create a new thread to execute the given operation.
         * The new thread starts up synchronously, can't be cancelled and it can't be joined.
         * @param threadID human readable descriptor to identify the thread for diagnostics
         * @param threadFunction a functor holding the code to execute within the new thread.
         *        Any function-like entity or callable is acceptable; arguments can be given.
         * @warning The operation functor and all arguments will be copied into the new thread.
         *        The return from this constructor _syncs-with_ the launch of the operation.
         */
        template<class FUN, typename...ARGS>
        ThreadLifecycle (string const& threadID, FUN&& threadFunction, ARGS&& ...args)
          : ThreadLifecycle{
              Launch{forward<FUN> (threadFunction), forward<ARGS> (args)...}
                    .threadID(threadID)}
          { }
        
        /**
         * Special variant to bind a subclass member function as thread operation.
         * @warning potential race between thread function and subclass initialisation
         */
        template<class SUB, typename...ARGS>
        ThreadLifecycle (RES (SUB::*memFun) (ARGS...), ARGS ...args)
          : ThreadLifecycle{
              Launch{std::move (memFun)
                    ,static_cast<SUB*> (this)
                    ,forward<ARGS> (args)...
                    }
                    .threadID(util::joinDash (typeSymbol<SUB>(), args...))}
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
            return Policy::isLive();
          }
        
        /** @return does this call happen from within this thread? */
        using Policy::invokedWithinThread;
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
      void detach() { ThreadLifecycle::handle_after_thread(); }
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
  
  
  
  /************************************************************************//**
   * Extended variant of the [standard case](\ref Thread), allowing to install
   * callbacks (hook functions) to be invoked during thread lifecycle:
   * - `atStart` : invoked as first user code in the new thread
   * - `atExit` : invoked as the last user code prior to detaching and thread end
   * - `onOrphan` : invoked from the thread-wrapper destructor, when the actual
   *   thread is detected as still running (according to the thread handle)
   * By default, these callbacks are empty; custom callbacks can be installed
   * through the ThreadLifecycle::Launch configuration builder using the
   * corresponding builder functions (e.g. `.atExit(λ)`). The passed functor
   * can either take no argument, or a single argument with a reference to
   * some `*this` subtype, which must be reachable by static downcast from
   * the ThreadLifecycle base type.
   */
  class ThreadHookable
    : public thread::ThreadLifecycle<thread::PolicyLifecycleHook>
    {
    public:
      using ThreadLifecycle::ThreadLifecycle;
    };
  
  
  
  /**
   * Launch an autonomous self-managing thread (and forget about it).
   * The thread-object is allocated to the heap and will delete itself on termination.
   * @tparam TAR concrete type of the subclass to be started as autonomous detached thread.
   * @param launchBuilder a flexible thread launch configuration to be used for starting;
   *        especially this contains the thread function and arguments to run in the thread.
   * @note  hooks `atExit` and `onOrphan` defined in the \a launchBuilder will be overridden.
   * @remarks the `atExit` hook is the last piece of code executed within the thread; here it
   *        takes a pointer to the allocated instance of type \a TAR and deletes it from heap.
   *        Obviously this will invoke the destructor of the thread-wrapper eventually, at which
   *        point the `onOrphan` hook is invoked (since the thread is still running); at this point
   *        the C++ thread-handle is detached, so that invoking the dtor of std::thread will not
   *        terminate the system, rather just let the thread go away. The net result is an object
   *        of type \a TAR placed on the heap and kept there precisely as long as the thread runs.
   */
  template<class TAR = ThreadHookable>
  inline void
  launchDetached (ThreadHookable::Launch&& launchBuilder)
  {
    static_assert (lib::meta::is_Subclass<TAR, ThreadHookable>());
    
    new TAR{move(launchBuilder)
                 .atExit([](TAR& selfAllocation)
                           {
                             delete &selfAllocation;
                           })
                 .onOrphan([](thread::ThreadWrapper& wrapper)
                           {
                             if (wrapper.isLive())
                               wrapper.threadImpl_.detach();
                           })};
     // Note: allocation tossed on the heap deliberately
  } //  The thread-function will pick up and manage *this
  
  /**
   * Launch an autonomous self-managing thread (and forget about it).
   * @tparam TAR concrete type of the subclass to be started as autonomous detached thread.
   * @param args a valid argument list to call the ctor of thread::ThreadLifecycle
   */
  template<class TAR = ThreadHookable, typename...INVO>
  inline void
  launchDetached (string const& threadID, INVO&& ...args)
  {
    using Launch = typename TAR::Launch;
    launchDetached<TAR> (Launch{forward<INVO> (args)...}
                          .threadID (threadID));
  }
  
  /** Special variant bind a member function of the subclass into the autonomous thread */
  template<class TAR, typename...ARGS>
  inline void
  launchDetached (string const& threadID, void (TAR::*memFun) (ARGS...), ARGS ...args)
  {
    using Launch = typename TAR::Launch;
    launchDetached<TAR> (Launch{std::move (memFun)
                               ,thread::InstancePlaceholder<TAR>{}
                               ,forward<ARGS> (args)...
                               }
                               .threadID (threadID));
  }
  
  /** Special variant without explicitly given thread-ID */
  template<class TAR, typename...ARGS>
  inline void
  launchDetached (void (TAR::*memFun) (ARGS...), ARGS ...args)
  {
    return launchDetached (util::joinDash (lib::meta::typeSymbol<TAR>(), args...)
                          ,memFun
                          ,forward<ARGS> (args)...
                          );
  }
  
  
} // namespace lib
#endif /*LIB_THREAD_H*/
