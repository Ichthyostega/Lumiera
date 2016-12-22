/*
  LOOPER.hpp  -  proc dispatcher loop and timing control logic

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file looper.hpp
 ** Implementation building block of ProcDispatcher to control waiting and timing.
 ** This helper encapsulates the loop control logic to separate it from actual
 ** implementation of timing and waiting (per pthread condition variables).
 ** It exposes a combined condition (to be used for waiting) plus any further
 ** controls to manage the operation of the actual queue. The actual tasks to
 ** be controlled are installed as functors.
 ** 
 ** @todo WIP-WIP as of 12/2016 
 ** 
 ** @see DispatcherLooper_test
 ** @see proc-dispatcher.hpp
 ** @see DispatcherLoop
 ** @see CommandQueue
 **
 */



#ifndef PROC_CONTROL_LOOPER_H
#define PROC_CONTROL_LOOPER_H

#include "lib/time/timevalue.hpp"
#include "backend/real-clock.hpp"
//#include "common/subsys.hpp"
//#include "lib/depend.hpp"

//#include <memory>
#include <functional>



namespace proc {
namespace control {
  
//  using lib::Symbol;
//  using std::bind;
//  using std::function;
  using lib::time::Time;
  using lib::time::TimeVar;
  using lib::time::Offset;
  using lib::time::Duration;
  using backend::RealClock;
  
  namespace {
    /**
     * Latency to trigger the Builder after processing command(s).
     * This allows to collect and aggregate commands trickling in from the UI,
     * especially from dragging and mouse wheel. Once the builder has started,
     * further commands will be blocked and enqueued.
     * @todo this value should be retrieved from configuration                  ////////////////////////////////TICKET #1052 : access application configuration
     * @see Looper::establishWakeTimeout()
     */
    const uint PROC_DISPATCHER_BUILDER_DELAY_ms = 50;
    
    /**
     * Factor to slow down the latency when the command queue is not empty.
     * The builder attempts first to dispatch all commands in the queue, before
     * triggering the Builder again. However, if the extended latency period
     * has been passed, a builder run will be forced, even if further commands
     * are still waiting in the queue.
     * @todo this value should be retrieved from configuration                  ////////////////////////////////TICKET #1052 : access application configuration
     */
    const uint PROC_DISPATCHER_BUSY_SLOWDOWN_FACTOR = 15;
  }
  
  
  /**
   * @todo write Type-comment (WIP 12/2016)
   * 
   * @warning the Looper _is not threadsafe,_
   *        since it is intended to be run exclusively from
   *        the Session working thread.
   * @see DispatcherLoop::run()
   */
  class Looper
    {
      using Predicate = std::function<bool(void)>;
      
      bool shutdown_ = false;
      bool disabled_ = false;
      Predicate hasCommandsPending_;
      
      uint dirty_ = 0;
      TimeVar gotDirty_ = Time::NEVER;
      
        
    public:
      template<class FUN>
      Looper(FUN determine_commands_are_waiting)
        : hasCommandsPending_(determine_commands_are_waiting)
      { }
      
      // standard copy acceptable
      
      
      /* == working state logic == */
      
      bool isDying()    const  { return shutdown_; }
      bool isDisabled() const  { return disabled_ or isDying(); }
      bool isWorking()  const  { return hasCommandsPending_() and not isDisabled(); }
      bool idleBuild()  const  { return dirty_ and not hasCommandsPending_(); }
      bool runBuild()   const  { return (idleBuild() or forceBuild()) and not isDisabled(); }
      bool isIdle()     const  { return not (isWorking() or runBuild() or isDisabled()); }
      
      
      /* == operation control == */
      
      void
      triggerShutdown()
        {
          shutdown_ = true;
        }
      
      void
      enableProcessing(bool yes =true)
        {
          disabled_ = not yes;
        }
      
      /** invoking this function signals
       *  that all consequences of past state changes
       *  have been processed and are duly resolved.
       */
      void
      markStateProcessed()
        {
          if (runBuild())
            --dirty_;
          ENSURE (dirty_ <= 2);
        }
      
      bool
      hasPendingChanges()  const
        {
          UNIMPLEMENTED("state transition logic");
        }
      
      /** state fusion to control (timed) wait */
      bool
      requireAction()
        {
          if (isWorking() and not dirty_)
            {
              dirty_ = 2;
              startBuilderTimeout();
            }
          
          return isWorking()
              or forceBuild()
              or isDying();
        }
      
      /** state fusion to control looping */
      bool
      shallLoop()  const
        {
          return not isDying();
        }
      
      ulong                                        /////////////////////////////////////////////TICKET #1056 : better return a std::chrono value here 
      getTimeout()  const
        {
          if (isDisabled())
            return 0;
          else
            return wakeTimeout_ms()
                 * (dirty_ and not isWorking()? 1 : slowdownFactor());
        }
      
      /* == diagnostics == */
      
//    size_t size() const ;
//    bool empty()  const ;
      
    private:
      static uint wakeTimeout_ms();
      static uint slowdownFactor();

      void startBuilderTimeout();
      bool forceBuild()  const;
    };
  ////////////////TODO 12/16 currently just fleshing  out the API....
  
  
  /** @internal establish the typical timeout for idle sleep.
   * When the ProcDispatcher has no work to do, it needs to wake up regularly
   * for a checkpoint, to determine if the Builder needs to be triggered or
   * the shutdown-flag be checked. So the period established here defines
   * some kind of minimal reaction especially for the builder, so to ensure
   * that further commands trickling in get a chance to be enqueued before
   * the builder run effectively blocks command processing. Add to this
   * the typical average running time of the builder, to get the reaction
   * period visible to the user as update response delay within the UI.
   * @todo find a way how to retrieve this value from application config!    ////////////////////TICKET #1052 : access application configuration
   */
  inline uint
  Looper::wakeTimeout_ms()
  {
    return PROC_DISPATCHER_BUILDER_DELAY_ms;
  }
  
  inline uint
  Looper::slowdownFactor()
  {
    return PROC_DISPATCHER_BUSY_SLOWDOWN_FACTOR;
  }
  
  inline void
  Looper::startBuilderTimeout()
  {
    gotDirty_ = RealClock::now();
  }
  
  /** @internal logic to enforce a builder run,
   * once some extended time period has been passed.
   */
  inline bool
  Looper::forceBuild()  const
  {
    static Duration maxBuildTimeout{Time(getTimeout(), 0)};
    
    return dirty_
       and maxBuildTimeout < Offset(gotDirty_, RealClock::now());
  }                                                                          ///////////////////TICKET #1055 : likely to become more readable when Lumiera Time has std::chrono integration 

  
  
}} // namespace proc::control
#endif /*PROC_CONTROL_LOOPER_H*/
