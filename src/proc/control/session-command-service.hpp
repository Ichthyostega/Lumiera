/*
  SESSION-COMMAND-SERVICE.hpp  -  public service to invoke commands on the session

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

/** @file session-command-service.hpp
 ** A public service offered by the Session, implementing the SessionCommand facade interface.
 ** This is the primary way to invoke commands and cause edit operations within the Session.
 ** Through this service, the user interface or other external entities may invoke pre defined
 ** commands and pass the appropriate arguments. Commands are small functions operating directly
 ** on the Session interface; each command is complemented with a state capturing function and
 ** an UNDO function.
 ** 
 ** This service is the implementation of a layer separation facade interface. Clients should use
 ** proc::control::SessionCommand::facade to access this service. This header defines the interface
 ** used to _provide_ this service, not to access it.
 **
 ** @see facade.hpp subsystems for the Proc-Layer
 ** @see guifacade.cpp starting this service 
 */


#ifndef PROC_CONTROL_SESSION_COMMAND_SERVICE_H
#define PROC_CONTROL_SESSION_COMMAND_SERVICE_H


#include "include/session-command-facade.h"
#include "proc/control/command-dispatch.hpp"
#include "common/instancehandle.hpp"
#include "lib/singleton-ref.hpp"
#include "lib/diff/gen-node.hpp"

#include <boost/noncopyable.hpp>



namespace proc {
namespace control {
  
  using lib::diff::Rec;
  
  
  
  /***********************************************************//**
   * Actual implementation of the SessionCommand service
   * within the Lumiera Session subsystem. Creating an instance
   * of this class automatically registers corresponding interface
   * with the Lumiera Interface/Plugin system and creates a forwarding
   * proxy within the application core to route calls through this interface.
   * 
   * This service is backed by implementation facilities embedded within
   * the ProcDispatcher, exposed through the CommandDispatch interface.
   * @see DispatcherLoop
   */
  class SessionCommandService
    : public SessionCommand
    , boost::noncopyable
    {
      CommandDispatch& dispatcher_;
      
      
      /* === Implementation of the Facade Interface === */
      
      void bindArg (string const& cmdID, Rec const& args) override;
      void invoke (string const& cmdID)                   override;
      
      
      /* === Interface Lifecycle === */
      
      typedef lumiera::InstanceHandle< LUMIERA_INTERFACE_INAME(lumieraorg_SessionCommand, 0)
                                     , SessionCommand
                                     > ServiceInstanceHandle;
      
      lib::SingletonRef<SessionCommand> implInstance_;
      ServiceInstanceHandle serviceInstance_;
      
    public:
      SessionCommandService (CommandDispatch& dispatcherLoopInterface);
      
    };
    
  
  
}} // namespace proc::control
#endif /*PROC_CONTROL_SESSION_COMMAND_SERVICE_H*/