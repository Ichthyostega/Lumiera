/*
  CommandSetup  -  Implementation of command registration and instance management

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/


/** @file command-setup.cpp
 ** Implementation details of instance management for command invocation by the GUI.
 ** 
 ** @see command-setup.hpp
 ** @see command-instance-manager.hpp
 ** @see TODO_CommandInstanceManager_test
 ** @see command.hpp
 ** @see command-registry.hpp
 **
 */


#include "lib/error.hpp"
//#include "lib/symbol.hpp"
//#include "lib/format-string.hpp"
#include "proc/control/command-setup.hpp"
#include "proc/control/command-instance-manager.hpp"


//#include <string>
#include <tuple>
#include <utility>

//using std::string;
using std::tuple;
using std::function;
using std::move;
//using util::cStr;
//using util::_Fmt;


namespace proc {
namespace control {
  namespace error = lumiera::error;
  
  
  namespace { // implementation details of command setup...
    
    using CmdDefEntry = std::tuple<Literal, DefinitionClosure>;
    
    std::deque<CmdDefEntry> pendingCmdDefinitions;
    
    
  }//(End) implementation details
  
  
  
  
  /** storage for.... */
  
  
  CommandSetup::~CommandSetup() { }
  
  /** Start a command setup for defining a Proc-Layer command with the given cmdID */
  CommandSetup::CommandSetup(Literal cmdID)
    : cmdID_(cmdID)
    { }
  
  
  /**
   * @param definitionBlock anything assignable to `function<void(CommandDef&)>`
   * @remarks this operation is intended for a very specific usage pattern, as established
   *          by the macro #COMMAND_DEFINITION. The purpose is to feed a given code block
   *          into the hidden queue for command definitions, from where it will be issued
   *          at the lifecycle event ON_BASIC_INIT (typically at start of application `main()`).
   *          On invocation, the code block is provided with an still unbound CommandDef object,
   *          which has been registered under the Command-ID as stored in this CommandSetup object.
   *          The assumption is that this _definition closure_ will care to define the command,
   *          state capturing and undo operations for the command definition in question. Thus,
   *          the result of invoking this closure will be to store a complete command prototype
   *          into the proc::control::CommandRegistry.
   * @note this operation works by side-effect; the given argument is fed into a hidden static
   *          queue, but not stored within the object instance.
   * @warning invoking this assignment _several times on the same CommandSetup object_ will likely
   *          lead to an invalid state, causing the Lumiera application to fail on start-up. The
   *          reason for this is the fact that CommandDef rejects duplicate command definitions.
   *          Moreover, please note that invoking this operation at any point _after_ the
   *          lifecycle event ON_BASIC_INIT will likely have no effect at all, since the
   *          given closure will then just sit in the static queue and never be invoked.   
   */
  CommandSetup&
  CommandSetup::operator= (DefinitionClosure definitionBlock)
  {
    if (not definitionBlock)
      throw error::Invalid ("unbound function/closure provided for CommandSetup"
                           , error::LUMIERA_ERROR_BOTTOM_VALUE);
    
    pendingCmdDefinitions.emplace_front (cmdID_, move(definitionBlock));
    return *this;
  }
  
  
  size_t
  CommandSetup::pendingCnt()
  {
    return pendingCmdDefinitions.size();
  }
  
  
  // emit dtors of embedded objects here....
  CommandInstanceManager::~CommandInstanceManager() { }
  
  CommandInstanceManager::CommandInstanceManager() { }
  
  
  
  
  /** more to come here...*/
  
  
  
}} // namespace proc::control
