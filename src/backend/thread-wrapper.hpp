/*
  THREADWRAPPER.hpp  -  thin convenience wrapper for starting lumiera threads
 
  Copyright (C)         Lumiera.org
    2008 - 2010         Hermann Vosseler <Ichthyostega@web.de>
                        Christian Thaeter <ct@pipapo.org>
 
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


#include "lib/error.hpp"
#include "include/logging.h"
#include "lib/bool-checkable.hpp"

extern "C" {
#include "backend/threads.h"
}
#include "backend/threadpool-init.hpp"

#include <tr1/functional>
#include <boost/noncopyable.hpp>


namespace backend {
  
  using std::tr1::bind;
  using std::tr1::function;
  using lib::Literal;
  
  typedef struct nobug_flag* NoBugFlag;
  
  
  
  /****************************************************************************
   * A thin convenience wrapper for dealing with threads,
   * as implemented by the threadpool in the backend (based on pthread).
   * Using this wrapper...
   * - helps with passing data to the function executed in the new thread
   * - allows to bind to various kinds of functions including member functions
   * The new thread starts immediately within the ctor; after returning, the new
   * thread has already copied the arguments and indeed actively started to run.
   *
   */
  class Thread
    : lib::BoolCheckable< Thread
    , boost::noncopyable>                //////TODO: do we want Thread instances to be copyable?
  {
    
  protected:
    typedef function<void(void)> Operation;
    
    
    struct ThreadStartContext
      : boost::noncopyable
      {
        
        Operation const& operation_;
        
        static void
        run (void* arg)
          {
            REQUIRE (arg);
            ThreadStartContext* ctx = reinterpret_cast<ThreadStartContext*>(arg);
            Operation _doIt_(ctx->operation_);
            
            lumiera_thread_sync (); // sync point: arguments handed over
            
            _doIt_(); // execute the actual operation in the new thread
          }
        
      public:
        ThreadStartContext (LumieraThread& handle
                           ,Operation const& operation_to_execute
                           ,Literal& purpose
                           ,NoBugFlag logging_flag
                           ,uint additionalFlags   =0  
                           )
          : operation_(operation_to_execute)
          {
            REQUIRE (!lumiera_error(), "Error pending at thread start") ;
            handle =
              lumiera_thread_run ( LUMIERA_THREADCLASS_INTERACTIVE | additionalFlags
                                 , &run         // invoking the run helper and..
                                 , this         // passing this start context as parameter
                                 , purpose.c()
                                 , logging_flag
                                 );
            if (!handle)
              lumiera::throwOnError();
            
            // make sure the new thread had the opportunity to take the Operation
            // prior to leaving and thereby possibly destroying this local context
            lumiera_thread_sync_other (handle);
          }
      };
    
    
    
    LumieraThread thread_;
    
    Thread() : thread_(0) { }
    
    
  public:
    /** Create a new thread to execute the given operation.
     *  The new thread starts up synchronously, it can't
     *  be cancelled and it can't be joined.
     *  @param purpose fixed char string used to denote the thread for diagnostics
     *  @param logging_flag NoBug flag to receive diagnostics regarding the new thread
     *  @param operation defining what to execute within the new thread. Any functor
     *         which can be bound to function<void(void)>. Note this functor will be
     *         copied onto the stack of the new thread, thus it can be transient.
     */
    Thread (Literal purpose, Operation const& operation, NoBugFlag logging_flag = &NOBUG_FLAG(thread))
      : thread_(0)
      {
        ThreadStartContext (thread_, operation, purpose, logging_flag);
      }
    
    /** automatic conversion: Thread instance can stand-in for a handle
     *  @throws lumiera::error::State when thread isn't running */
    operator LumieraThread()  const
      {
        if (!isValid())
          throw lumiera::error::State("thread not executing (anymore)");
        
        return thread_;
      }
    
    bool
    isValid()  const
      {
        return thread_
            && true    ////////////TODO: how to determine that the thread is still running?
            ;
      }
    
    
    /** Synchronisation barrier. In the function executing in this thread
     *  needs to be a corresponding lumiera_thread_sync() call. Blocking
     *  until both the caller and the thread have reached the barrier.
     */
    void
    sync ()
      {
        REQUIRE (isValid(), "Thread terminated");
        if (!lumiera_thread_sync_other (thread_))
          lumiera::throwOnError();
      }
  };
  
  
  
  
  
  
  /**
   * Variant of the standard case, allowing additionally
   * to join on the termination of this thread.
   */
  class ThreadJoinable
    : public Thread
  {
  public:
    ThreadJoinable (Literal purpose, Operation const& operation,
                    NoBugFlag logging_flag = &NOBUG_FLAG(thread))
      : Thread()
      {
        ThreadStartContext (thread_, operation, purpose, logging_flag,
                            LUMIERA_THREAD_JOINABLE);
      }
    
    
    /** put the caller into a blocking wait until this thread has terminated.
     *  @throws error::Logic if this thread has already terminated
     */
    void join()
      {
        if (!isValid())
          throw lumiera::error::Logic ("joining on an already terminated thread");
        
        lumiera_err errorInOtherThread =
            lumiera_thread_join (thread_);
        thread_ = 0;
        
        if (errorInOtherThread)
          throw lumiera::error::State ("Thread terminated with error:");
      }
  };
  
  
  
} // namespace backend
#endif
