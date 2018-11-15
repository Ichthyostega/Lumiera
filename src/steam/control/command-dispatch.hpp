/*
  COMMAND-DISPATCH.hpp  -  Interface to enqueue and dispatch command messages

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


/** @file command-dispatch.hpp
 ** Interface to abstract the ProcDispatcher's ability to handle command messages.
 ** 
 ** @see proc-dispatcher.hpp
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
