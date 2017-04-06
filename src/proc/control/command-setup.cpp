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
#include "include/logging.h"
#include "include/lifecycle.h"
#include "proc/control/command-setup.hpp"
#include "proc/control/command-instance-manager.hpp"
#include "proc/control/command-def.hpp"
#include "lib/symbol.hpp"
#include "lib/format-string.hpp"
#include "lib/util.hpp"

#include <tuple>
#include <utility>

using std::tuple;
using std::get;
using std::function;
using std::move;
using lib::Symbol;
using lumiera::LifecycleHook;
using lumiera::ON_GLOBAL_INIT;
using std::string;
using util::unConst;
using util::_Fmt;


namespace proc {
namespace control {
  namespace error = lumiera::error;
  
  namespace { // implementation details: storage for pending static command definitions...
    
    using CmdDefEntry = std::tuple<Symbol, DefinitionClosure>;
    
    std::deque<CmdDefEntry> pendingCmdDefinitions;
    
  }//(End) implementation details
  
  
  
  
  
  
  CommandSetup::~CommandSetup() { }
  
  /** Start a command setup for defining a Proc-Layer command with the given cmdID
   * @param cmdID the ID under with the new command will be registered
   * @note after defining a static variable of type CommandSetup,
   *       a functor or lambda should be assigned, which then
   *       provides the actual setup of the CommandDef
   */
  CommandSetup::CommandSetup(Symbol cmdID)
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
  
  void
  CommandSetup::invokeDefinitionClosures()
  {
    while (not pendingCmdDefinitions.empty())
      {
        CmdDefEntry& entry = pendingCmdDefinitions.back();
        Symbol& cmdID{get<Symbol>(entry)};
        DefinitionClosure& buildDefinition{get<DefinitionClosure> (entry)};
        
        TRACE (command, "defining Command(%s)...", cmdID.c());
        CommandDef def(cmdID);
        buildDefinition(def);
        pendingCmdDefinitions.pop_back();
      }
  }
  
  namespace { // automatically invoke static command definitions
    
    LifecycleHook schedule_ (ON_GLOBAL_INIT, &CommandSetup::invokeDefinitionClosures);
  }
  
  
  
  
  
  // emit dtors of embedded objects here....
  CommandInstanceManager::~CommandInstanceManager() { }
  
  CommandInstanceManager::CommandInstanceManager (CommandDispatch& dispatcher)
    : dispatcher_{dispatcher}
    , table_{2 * Command::definition_count()}
    { }
  
  
  /** @todo more to come here...*/
  Symbol
  CommandInstanceManager::newInstance (Symbol prototypeID, string invocationID)
  {
    Symbol instanceID{lib::internedString (string{prototypeID}+"."+invocationID)};
    Command& instance = table_[instanceID];
    if (instance)
      throw new error::Logic (_Fmt{"Attempt to create a new Command instance '%s', "
                                   "while an instance for this invocationID %s "
                                   "is currently open for parametrisation and "
                                   "not yet dispatched for execution."}
                                  % instanceID % invocationID
                             , LUMIERA_ERROR_DUPLICATE_COMMAND
                             );
    // create new clone from the prototype
    table_[instanceID] = move (Command::get(prototypeID).newInstance());
    ENSURE (instance, "cloning of command prototype failed");
    return instanceID;
  }
  
  
  Command&
  CommandInstanceManager::getInstance (Symbol instanceID)
  {
    Command& instance = table_[instanceID];
    if (not instance)
      throw error::Invalid (_Fmt{"Command instance '%s' is not (yet/anymore) active"}
                                % instanceID
                           , LUMIERA_ERROR_INVALID_COMMAND);
    return instance;
  }
  
  
  /** */
  void
  CommandInstanceManager::dispatch (Symbol instanceID)
  {
    Command& instance = table_[instanceID];
    if (not instance)
      throw error::Logic (_Fmt{"attempt to dispatch command instance '%s' "
                               "without creating a new instance from prototype beforehand"}
                              % instanceID
                         , error::LUMIERA_ERROR_LIFECYCLE);
    if (not instance.canExec())
      throw error::State (_Fmt{"attempt to dispatch command instance '%s' "
                               "without binding all arguments properly beforehand"}
                              % instanceID
                         , LUMIERA_ERROR_UNBOUND_ARGUMENTS);
    
    REQUIRE (instance and instance.canExec());
    dispatcher_.enqueue(move (instance));
    ENSURE (not instance);
  }
  
  
  bool
  CommandInstanceManager::contains (Symbol instanceID)  const
  {
    return util::contains (table_, instanceID)
       and unConst(this)->table_[instanceID].isValid();
  }
  
  
  
}} // namespace proc::control
