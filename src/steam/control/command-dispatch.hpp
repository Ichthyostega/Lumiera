/*
  COMMAND-DISPATCH.hpp  -  Interface to enqueue and dispatch command messages

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file command-dispatch.hpp
 ** Interface to abstract the SteamDispatcher's ability to handle command messages.
 ** 
 ** @see steam-dispatcher.hpp
 ** @see session-command-service.hpp
 ** @see DispatcherLoop
 **
 */



#ifndef STEAM_CONTROL_COMMAND_DISPATCH_H
#define STEAM_CONTROL_COMMAND_DISPATCH_H

#include "steam/control/command.hpp"



namespace steam {
namespace control {
  
  
  
  
  /**
   * Interface of a service to perform Commands on the session.
   * Commands committed here need to be ready for actual invocation
   * on the _current session._ They will be sent through a queue
   * to be performed one by one.
   */
  class CommandDispatch
    {
      
    public:
      virtual ~CommandDispatch() { }  ///< this is an interface
      
      virtual void clear()              =0;  /////TODO do we actually need that operation?
      virtual void enqueue (Command&&)  =0;
    };
  
  
  
  
}} // namespace steam::control
#endif /*STEAM_CONTROL_COMMAND_DISPATCH_H*/
