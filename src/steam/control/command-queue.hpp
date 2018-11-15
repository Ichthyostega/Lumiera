/*
  COMMAND-QUEUE.hpp  -  organise ordering of commands within ProcDispatcher

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


/** @file command-queue.hpp
 ** Implementation building block of ProcDispatcher to organise commands.
 ** This is the actual implementation of the command queue to allow for
 ** strictly sequential dispatch of commands to work on the session.
 ** 
 ** @todo as of 12/2016 this is fully functional, but we may want to add
 **       some further management functions like purging of expired commands
 ** 
 ** @see CommandQueue_test
 ** @see proc-dispatcher.hpp
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


namespace proc {
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
  
  
  
}} // namespace proc::control
#endif /*STEAM_CONTROL_COMMAND_QUEUE_H*/
