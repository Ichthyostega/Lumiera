/*
  Command  -  Key abstraction for proc/edit operations and UNDO management
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/


/** @file command.cpp 
 ** //TODO 
 ** 
 ** @see command.hpp
 ** @see command-registry.hpp
 **
 */


#include "proc/control/command.hpp"
#include "proc/control/command-def.hpp"
#include "proc/control/command-registry.hpp"
#include "proc/control/handling-pattern.hpp"
//#include "proc/mobject/mobject-ref.hpp"
//#include "proc/mobject/mobject.hpp"
//#include "proc/mobject/placement.hpp"

//#include <boost/format.hpp>
//using boost::str;

namespace control {
  
  LUMIERA_ERROR_DEFINE (INVALID_COMMAND,   "Unknown or insufficiently defined command");
  LUMIERA_ERROR_DEFINE (INVALID_ARGUMENTS, "Arguments provided for binding doesn't match stored command function parameters");
  LUMIERA_ERROR_DEFINE (UNBOUND_ARGUMENTS, "Command mutation functor not yet usable, because arguments aren't bound");
  LUMIERA_ERROR_DEFINE (MISSING_MEMENTO,   "Undo functor not yet usable, because no undo state has been captured");
  
  

  Command::~Command() { }
  
  
  /** @internal to be invoked by #fetchDef */
  Command::Command (CommandImpl* pImpl)
  {
    Handle::activate (pImpl, CommandRegistry::killCommandImpl);
  }
  

  /** */
  Command& 
  Command::get (Symbol cmdID)
  {
    UNIMPLEMENTED ("fetch an existing command from the internal cmd registry");
  }
  
  
  Command&
  Command::fetchDef (Symbol cmdID)
  {
    Command* cmd = CommandRegistry::queryIndex (cmdID);
    if (cmd)
      ////////////////////////////////////////////////////////////////////////TODO: race
      return *cmd;
    
    Command newDefinition (CommandRegistry::newCommandImpl());
    
    return CommandRegistry::track (cmdID, newDefinition);
  }
  
  
  CommandDef
  Command::storeDef (Symbol newCmdID)
  {
    UNIMPLEMENTED ("create a new definition & prototype based on this command");
  }
  
  
  bool
  Command::remove (Symbol cmdID)
  {
    UNIMPLEMENTED ("de-register a single command instance");
  }
  
  
  bool
  Command::undef (Symbol cmdID)
  {
    UNIMPLEMENTED ("completely drop a command definition, together with all dependent instances");
  }
  
  
  
  size_t
  Command::definition_count()
  {
    UNIMPLEMENTED ("return number of command definitions currently in the registry");
  }
  
  
  
  size_t
  Command::instance_count()
  {
    UNIMPLEMENTED ("return number individual command instances currently in the registry");
  }
  
  
  
  bool
  CommandDef::isValid()  const
  {
    UNIMPLEMENTED ("command *definition* validity self check");
  }
  
  
  
  bool
  Command::isValid()  const
  {
    UNIMPLEMENTED ("command validity self check");
  }
  
  
  
  bool
  Command::canExec()  const
  {
    UNIMPLEMENTED ("state check: sufficiently defined to be invoked");
  }
  
  
  
  bool
  Command::canUndo()  const
  {
    UNIMPLEMENTED ("state check: has undo state been captured?");
  }

  
  
  
  void
  Command::undo ()
  {
    exec (getDefaultHandlingPattern().howtoUNDO());
  }
  
  
  void
  Command::exec (HandlingPattern const& execPattern)
  {
    execPattern.invoke (*this);
  }
  
  
  void
  Command::execSync ()
  {
    exec (HandlingPattern::get(HandlingPattern::SYNC_THROW));
  }
  
  
  HandlingPattern const&
  Command::getDefaultHandlingPattern()  const
  {
    UNIMPLEMENTED ("manage the default command handling pattern");
  }
  
  
  template<typename TYPES>
  void
  Command::bindArg (Tuple<TYPES> const& args)
  {
    UNIMPLEMENTED ("create an argument-binding, with runtime type check");
  }


  

  
  
  
  
} // namespace control
