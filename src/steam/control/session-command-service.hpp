/*
  SESSION-COMMAND-SERVICE.hpp  -  public service to invoke commands on the session

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
 ** steam::control::SessionCommand::facade to access this service. This header defines the interface
 ** used to _provide_ this service, not to access it.
 ** 
 ** @see session-command-facade.h
 ** @see facade.hpp subsystems for the Steam-Layer
 ** @see guifacade.cpp starting this service 
 */


#ifndef STEAM_CONTROL_SESSION_COMMAND_SERVICE_H
#define STEAM_CONTROL_SESSION_COMMAND_SERVICE_H


#include "include/session-command-facade.h"
#include "steam/control/command-dispatch.hpp"
#include "steam/control/command-instance-manager.hpp"
#include "common/instancehandle.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/nocopy.hpp"
#include "lib/symbol.hpp"




namespace steam {
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
   * the SteamDispatcher, exposed through the CommandDispatch interface.
   * Additionally, it operates a CommandInstanceManager to allow the
   * creation of local instances "opened" for argument binding.
   * In fact, this is the standard "command cycle" and the
   * intended usage pattern.
   * @warning local command instances are not threadsafe
   * @see DispatcherLoop
   */
  class SessionCommandService
    : public SessionCommand
    , util::NonCopyable
    {
      CommandDispatch& dispatcher_;
      CommandInstanceManager instanceManager_;
      
      
      /* === Interface Lifecycle === */
      
      using ServiceInstanceHandle = lumiera::InstanceHandle< LUMIERA_INTERFACE_INAME(lumieraorg_SessionCommand, 0)
                                                           , SessionCommand
                                                           >;
      ServiceInstanceHandle serviceInstance_;
      
    public:
      SessionCommandService (CommandDispatch& dispatcherLoopInterface);
      
      
      /* === Implementation of the Facade Interface === */
      
      Symbol cycle (Symbol cmdID, string const& invocationID) override;
      void trigger (Symbol cmdID, Rec const& args)            override;
      void bindArg (Symbol cmdID, Rec const& args)            override;
      void invoke  (Symbol cmdID)                             override;
    };
    
  
  
}} // namespace steam::control
#endif /*STEAM_CONTROL_SESSION_COMMAND_SERVICE_H*/
