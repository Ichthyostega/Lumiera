/*
  COMMAND-QUEUE.hpp  -  organise ordering of commands within SteamDispatcher

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file command-queue.hpp
 ** Implementation building block of SteamDispatcher to organise commands.
 ** This is the actual implementation of the command queue to allow for
 ** strictly sequential dispatch of commands to work on the session.
 ** 
 ** @todo as of 12/2016 this is fully functional, but we may want to add
 **       some further management functions like purging of expired commands
 ** 
 ** @see CommandQueue_test
 ** @see steam-dispatcher.hpp
 ** @see session-command-service.hpp
 ** @see DispatcherLoop
 ** @see Looper
 **
 */



#ifndef STEAM_CONTROL_COMMAND_QUEUE_H
#define STEAM_CONTROL_COMMAND_QUEUE_H

#include "steam/control/command.hpp"
#include "lib/iter-stack.hpp"
#include "lib/format-string.hpp"
#include "lib/util.hpp"


namespace steam {
namespace control {
  
  namespace error = lumiera::error;
  
  using util::_Fmt;
  using lib::unConst;
  
  
  
  /**
   * Implementation of the Session's command queue.
   * @see DispatcherLoop
   */
  class CommandQueue
    : public lib::IterQueue<Command>
    {
      
    public:
      CommandQueue() { }
      
      
      CommandQueue&
      feed (Command&& cmd)
        {
          if (not cmd.canExec())
            throw error::Logic(_Fmt("Reject '%s'. Not suitably prepared for invocation: %s")
                                   % cmd.getID() % cmd
                              , LERR_(UNBOUND_ARGUMENTS));
          
          lib::IterQueue<Command>::feed (move(cmd));
          return *this;
        }
      
      void
      clear()
        {
          this->stateCore().clear();
        }
    };
  
  
  
}} // namespace steam::control
#endif /*STEAM_CONTROL_COMMAND_QUEUE_H*/
