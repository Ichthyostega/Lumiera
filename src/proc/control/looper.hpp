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

#include "lib/error.hpp"   ////////TODO needed?
//#include "common/subsys.hpp"
//#include "lib/depend.hpp"

//#include <memory>
#include <functional>



namespace proc {
namespace control {
  
//  using lib::Symbol;
//  using std::bind;
//  using std::function;
  
  namespace {
    /**
     * @todo this value should be retrieved from configuration                  ////////////////////////////////TICKET #1052 : access application configuration
     * @see Looper::establishWakeTimeout()
     */
    const uint PROC_DISPATCHER_BUILDER_DELAY_ms = 50;
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
      uint dirty_    = false;
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
      bool isWorking()  const  { return hasCommandsPending_() and not isDisabled(); }
      bool runBuild()   const  { return (dirty_ and not hasCommandsPending_()) or forceBuild(); }
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
          if (isWorking())
            dirty_ = 2;
          
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
      
      ulong
      getTimeout()  const
        {
          static uint wakeTimeout_ms = establishWakeTimeout();
          return wakeTimeout_ms;
        }
      
      /* == diagnostics == */
      
//    size_t size() const ;
//    bool empty()  const ;
      
    private:
      static uint establishWakeTimeout();
      
      bool forceBuild()  const
        {
          return false;
        }
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
  uint inline
  Looper::establishWakeTimeout()
  {
    return PROC_DISPATCHER_BUILDER_DELAY_ms;
  }
  
  
}} // namespace proc::control
#endif /*PROC_CONTROL_LOOPER_H*/
