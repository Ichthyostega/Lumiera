/*
  PROC-DISPATCHER.hpp  -  Proc-Layer command dispatch and execution

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file proc-dispatcher.hpp
 ** Dispatch and execute mutation operations on the High-level model.
 ** The ProcDispatcher is the application facility to back and support working
 ** on the Session. While the session itself is just a data structure, _operating_
 ** the session means to process commands changing that data structure and it means
 ** to evaluate the _session model_ and _build_ a render nodes network in accordance
 ** to that model's meaning. This _operation aspect_ of the session thus has a lifecycle
 ** and this is what we call the "session subsystem" within the application. When this is
 ** running, the SessionCommand facade is opened, accepting commands to work on the session.
 ** Such commands are passed through a dispatch queue to ensure consistent session state
 ** and to force strictly sequential processing of commands. Moreover, the ProcDispatcher's
 ** responsibility is to care for triggering the Builder after any changes induced by those
 ** commands. The Builder is guaranteed to run _eventually_, yet with some leeway.
 ** 
 ** Talking of state, independent of the lifecycle running state, the ProcDispatcher can be
 ** _activated or deactivated_. In active state, commands are dequeued and processed, while
 ** in inactive state commands are just accepted and queued, assuming the SessionCommand
 ** interface is currently opened. This activation state is controlled by the session
 ** datastructure itself, more precisely the SessionManager. A session can be stored,
 ** closed, reset and loaded with existing content from persistent storage, and only
 ** when the data structure is complete and consistent, processing can be enabled.
 **
 ** @see Command
 ** @see Session
 ** @see proc-dispatcher.cpp for details of operational semantics
 **
 */



#ifndef PROC_CONTROL_PROC_DISPATCHER_H
#define PROC_CONTROL_PROC_DISPATCHER_H

#include "common/subsys.hpp"
#include "lib/depend.hpp"
#include "lib/sync.hpp"

#include <memory>



namespace proc {
namespace control {
  
  using std::unique_ptr;
  using lumiera::Subsys;
  
  
  class DispatcherLoop;
  
  
  /**
   * Guard to manage processing commands working on the session.
   * A static application facility, actually backing and implementing
   * the »session subsystem«. Embedded within the implementation of this
   * class is the _»session loop thread«_ to perform any session mutation
   * commands and to operate the Builder, which translates the session
   * contents into a render nodes network. Also embedded herein is
   * the implementation of proc::control::SessionCommandService
   */
  class ProcDispatcher
    : public lib::Sync<>
    {
      unique_ptr<DispatcherLoop> runningLoop_;
      bool active_{false};
      
    public:
      static lib::Depend<ProcDispatcher> instance;
      
      bool start (Subsys::SigTerm);
      bool isRunning();
      void requestStop()  noexcept;
      
      void activate();
      void deactivate();
      void awaitDeactivation();
      void clear();
      
      bool empty()  const ;
      
    private:
      void endRunningLoopState();
      
     ~ProcDispatcher();
      ProcDispatcher();
      friend class lib::DependencyFactory<ProcDispatcher>;
    };
  
  
  
  
}} // namespace proc::control
#endif /*PROC_CONTROL_PROC_DISPATCHER_H*/
