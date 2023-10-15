/*
  LOOPER.hpp  -  steam dispatcher loop and timing control logic

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
 ** Implementation building block of SteamDispatcher to control waiting and timing.
 ** This helper encapsulates the loop control logic to separate it from actual
 ** implementation of timing and waiting (per pthread condition variables).
 ** It exposes a combined condition (to be used for waiting) plus any further
 ** state predicates necessary to manage the state transitions regarding the
 ** SteamDispatcher implementation:
 ** - detect working state, based on a closure to detect an non empty CommandQueue
 ** - handle the disabling and shutdown of the dispatching task
 ** - detect an idle state to allow the DispatcherLoop to go to sleep
 ** - detect the need to run the builder after handling a command
 ** - manage timeout to run the builder with a slight latency
 ** - manage an extended timeout to enforce builder run eventually.
 ** - offer a "check point" where all state is balanced, which can be
 **   used as a synchronisation point to halt the loop.
 ** 
 ** @see DispatcherLooper_test
 ** @see steam-dispatcher.hpp
 ** @see DispatcherLoop
 ** @see CommandQueue
 **
 */



#ifndef STEAM_CONTROL_LOOPER_H
#define STEAM_CONTROL_LOOPER_H

#include "lib/time/timevalue.hpp"
#include "vault/real-clock.hpp"

#include <functional>
#include <chrono>



namespace steam {
namespace control {
  
  using lib::time::Time;
  using lib::time::TimeVar;
  using lib::time::Offset;
  using lib::time::Duration;
  using std::chrono::milliseconds;
  using vault::RealClock;
  
  namespace {
    /**
     * Latency to trigger the Builder after processing command(s).
     * This allows to collect and aggregate commands trickling in from the UI,
     * especially from dragging and mouse wheel. Once the builder has started,
     * further commands will be blocked and enqueued.
     * @todo this value should be retrieved from configuration                  ////////////////////////////////TICKET #1052 : access application configuration
     * @see Looper::establishWakeTimeout()
     */
    const uint STEAM_DISPATCHER_BUILDER_DELAY_ms = 50;
    
    /**
     * Factor to slow down the latency when the command queue is not empty.
     * The builder attempts first to dispatch all commands in the queue, before
     * triggering the Builder again. However, if the extended latency period
     * has been passed, a builder run will be forced, even if further commands
     * are still waiting in the queue.
     * @todo this value should be retrieved from configuration                  ////////////////////////////////TICKET #1052 : access application configuration
     */
    const uint STEAM_DISPATCHER_BUSY_SLOWDOWN_FACTOR = 15;
  }
  
  
  
  /**
   * Encapsulated control logic for the session thread loop.
   * This helper component was factored out from the loop body
   * for sake of clarity and to allow unit testing of the logic
   * in isolation. It is based on logical relations together
   * with the following assumptions
   * - Looper::shallLoop controls the loop's `while` condition
   * - at the begin of the loop the thread possibly enters a blocking
   *   wait state; the wake-up condition is provided by Looper::requireAction.
   * - then, in the actual loop body, depending on the predicates calculated here,
   *   either the builder run is triggered, or a single command is dispatched
   *   from the queue to work on the session.
   * - after returning from these active operations, at the end of the loop,
   *   the state evaluation is updated by Looper::markStateProcessed
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
      bool inChange_ = false;
      bool hasWork_  = false;
      bool isDirty_  = false;
      
      TimeVar gotDirty_ = Time::NEVER;
      
      Predicate hasCommandsPending_;
      
        
    public:
      template<class FUN>
      Looper(FUN determine_commands_are_waiting)
        : hasCommandsPending_(determine_commands_are_waiting)
      { }
      
      // standard copy acceptable
      
      
      /* == working state logic == */
      
      bool isDying()    const  { return shutdown_; }
      bool isDisabled() const  { return disabled_ or isDying(); }
      bool useTimeout() const  { return isDirty_ and not isDisabled(); }
      bool isWorking()  const  { return hasWork_ and not isDisabled(); }
      bool idleBuild()  const  { return isDirty_ and not hasWork_; }
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
      
      /** invoking this function signals that
       *  all consequences of past state changes
       *  have been processed and are duly resolved.
       * @remark the implementation actually does not need to watch out for command processing state
       *         directly, only the managing of builder runs requires active state transitions here.
       *         When the conditions for triggering the Builder are met, control flow typically just
       *         has emptied the command queue. Thus we need to let one invocation pass by; the next
       *         loop iteration will begin after waking up from a short sleep and trigger the build,
       *         so the following (second) invocation can clear the builder dirty state.
       */
      void
      markStateProcessed()
        {
          inChange_ = false;
          if (runBuild())
            isDirty_ = false; // assume the builder has been triggered in the loop body
        }
      
      bool
      hasPendingChanges()  const  ///< "check point"
        {
          return inChange_;
        }
      
      /** state fusion to control (timed) wait */
      bool
      requireAction()
        {
          hasWork_ = hasCommandsPending_();
          bool proceedImmediately = isWorking() or forceBuild() or isDying();
          inChange_ = proceedImmediately or useTimeout();
          
          if (isWorking() and not isDirty_)
            { // schedule Builder run after timeout
              startBuilderTimeout();
              isDirty_ = true;
            }
          
          return proceedImmediately;
        }
      
      /** state fusion to control looping */
      bool
      shallLoop()  const
        {
          return not isDying();
        }
      
      milliseconds
      getTimeout()  const
        {
          if (not useTimeout())
            return milliseconds::zero();
          else
            return milliseconds{
                    wakeTimeout_ms()
                    * (isDirty_ and not isWorking()? 1 : slowdownFactor())};
        }
      
      
    private:
      static uint wakeTimeout_ms();
      static uint slowdownFactor();

      void startBuilderTimeout();
      bool forceBuild()  const;
    };
  
  
  
  /** @internal establish the typical timeout for idle sleep.
   * When the SteamDispatcher has no work to do, it needs to wake up regularly
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
    return STEAM_DISPATCHER_BUILDER_DELAY_ms;
  }
  
  inline uint
  Looper::slowdownFactor()
  {
    return STEAM_DISPATCHER_BUSY_SLOWDOWN_FACTOR;
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
    static Duration maxBuildTimeout{Time(wakeTimeout_ms() * slowdownFactor(), 0)};
    
    return isDirty_
       and maxBuildTimeout < Offset(gotDirty_, RealClock::now());
  }                                                                          ///////////////////TICKET #1055 : likely to become more readable when Lumiera Time has std::chrono integration 

  
  
}} // namespace steam::control
#endif /*STEAM_CONTROL_LOOPER_H*/
