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
 ** This translation unit combines the implementation code of several facilities
 ** related to definition and instantiation of concrete command instances.
 ** 
 ** @see command-setup.hpp
 ** @see command-instance-manager.hpp
 ** @see CommandInstanceManager_test
 ** @see command.hpp
 ** @see command-registry.hpp
 **
 */


#include "lib/error.hpp"
#include "include/logging.h"
#include "include/lifecycle.h"
#include "proc/control/command.hpp"
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
    
    std::deque<CmdDefEntry>&
    pendingCmdDefinitions()
    {
      static std::deque<CmdDefEntry> definitionQueue;
      return definitionQueue;
    }
    
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
                           , error::LERR_(BOTTOM_VALUE));
    
    pendingCmdDefinitions().emplace_front (cmdID_, move(definitionBlock));
    return *this;
  }
  
  
  size_t
  CommandSetup::pendingCnt()
  {
    return pendingCmdDefinitions().size();
  }
  
  void
  CommandSetup::invokeDefinitionClosures()
  {
    while (not pendingCmdDefinitions().empty())
      {
        CmdDefEntry& entry = pendingCmdDefinitions().back();
        Symbol& cmdID{get<Symbol>(entry)};
        DefinitionClosure& buildDefinition{get<DefinitionClosure> (entry)};
        
        TRACE (command, "defining Command(%s)...", cmdID.c());
        CommandDef def(cmdID);
        buildDefinition(def);
        pendingCmdDefinitions().pop_back();
      }
  }
  
  namespace { // automatically invoke static command definitions
    
    LifecycleHook schedule_ (ON_GLOBAL_INIT, &CommandSetup::invokeDefinitionClosures);
  }
  
  
  
  
  
  // emit dtors of embedded objects here....
  CommandInstanceManager::~CommandInstanceManager() { }
  
  /** create a CommandInstanceManager and wire it with the given CommandDispatch implementation.
   * Typically, this is done in SessionCommandService. The embedded hash table for pending
   * command instances is sized in relation to the number of registered global Command definitions.
   */
  CommandInstanceManager::CommandInstanceManager (CommandDispatch& dispatcher)
    : dispatcher_{dispatcher}
    , table_{2 * Command::definition_count()}
    { }
  
  
  /** Create and thus "open" a new anonymous command instance.
   * @param prototypeID the underlying Command definition to create a clone copy
   * @param invocationID used to decorate the prototypeID to from an unique instanceID
   * @throws error::Logic in case an instance is for this ID combination is already "open"
   */
  Symbol
  CommandInstanceManager::newInstance (Symbol prototypeID, string const& invocationID)
  {
    Symbol instanceID{prototypeID, invocationID};
    Command& instance = table_[instanceID];
    if (instance)
      throw new error::Logic (_Fmt{"Attempt to create a new Command instance '%s', "
                                   "while an instance for this invocationID %s "
                                   "is currently open for parametrisation and "
                                   "not yet dispatched for execution."}
                                  % instanceID % invocationID
                             , LERR_(DUPLICATE_COMMAND)
                             );
    // create new clone from the prototype
    table_[instanceID] = move (Command::get(prototypeID).newInstance());
    ENSURE (instance, "cloning of command prototype failed");
    return instanceID;
  }
  
  
  /** access the currently "opened" instance with the given instanceID
   * @param instanceID ID as returned from #newInstance, or a global commandID
   * @return instance handle of handle of a global command as fallback
   * @note when given a commandID, which is not known as (decorated) instanceID
   *       within our local registration table, just the globally registered
   *       Command instance is returned.
   * @remark deliberately this function returns by-value. Returning a reference
   *       into the global command registry would be dangerous under concurrency.
   * @throw error::Invalid when the given cmdID unknown both locally and globally.
   * @throw error::Logic when accessing an instance that _was_ known but is
   *       currently no longer "open" (already dispatched command instance)
   */
  Command
  CommandInstanceManager::getInstance (Symbol instanceID)
  {
    auto entry = table_.find(instanceID);
    if (entry == table_.end())
      return Command::get(instanceID);
    if (not entry->second)
      throw error::Logic (_Fmt{"Command instance '%s' is not (yet/anymore) active"}
                              % instanceID
                         , error::LERR_(LIFECYCLE));
    return entry->second;
  }
  
  
  /** @internal retrieve either global or local command instance
   * When matching a globally defined command, an anonymous clone instance
   * will be created. Otherwise a lookup in the local instance table is performed
   * and a matching entry is _moved out of the table_.
   */
  Command
  CommandInstanceManager::getCloneOrInstance (Symbol instanceID, bool must_be_bound)
  {
    Command instance = Command::maybeGetNewInstance (instanceID);
    if (not instance)
      { // second attempt: search of a locally "opened" instance
        auto entry = table_.find(instanceID);
        if (entry == table_.end())
          throw error::Invalid(_Fmt("Command-ID \"%s\" refers neither to a "
                                    "globally registered command definition, "
                                    "nor to an previously opened command instance")
                                   % instanceID
                              , LERR_(INVALID_COMMAND));
        if (not entry->second.isValid())
          throw error::Logic (_Fmt{"Command instance '%s' is not (yet/anymore) active"}
                                  % instanceID
                             , error::LERR_(LIFECYCLE));
        if (not must_be_bound or entry->second.canExec())
          instance = move(entry->second);
      }
    if (must_be_bound and not instance.canExec())
      throw error::State (_Fmt{"attempt to dispatch command instance '%s' "
                               "without binding all arguments properly beforehand"}
                              % instanceID
                         , LERR_(UNBOUND_ARGUMENTS));
    
    ENSURE (instance.isValid() and
           (instance.canExec() or not must_be_bound));
    return instance;
  }
  
  
  /** @internal hand a command over to the dispatcher */
  void
  CommandInstanceManager::handOver (Command&& toDispatch)
  {
    REQUIRE (toDispatch and toDispatch.canExec());
    dispatcher_.enqueue(move (toDispatch));
    ENSURE (not toDispatch);
  }
  
  
  /** hand over the designated command instance to the dispatcher installed
   * on construction. Either the given ID corresponds to a global command definition,
   * in which case an anonymous clone copy is created from this command. Alternatively
   * the given ID matches a previously "opened" local instance (known only to this
   * instance manager). In this case, the instance will really be _moved_ over into
   * the dispatcher, which also means this instance is no longer "open" for
   * parametrisation.
   * @throw error::Logic when the command's arguments aren't bound
   */
  void
  CommandInstanceManager::dispatch (Symbol instanceID)
  {
    handOver (getCloneOrInstance (instanceID, true));
  }
  
  
  /** fire and forget anonymous command instance.
   * This is a simplified interface, allowing to create a clone instance
   * from a global command definition (prototype), bind the arguments and
   * pass this instance to the dispatcher in one shot. To integrate with the
   * extended usage cycle, as a variation the given ID may indicate a previously
   * opened instance, which will then be bound and dispatched likewise.
   * @param instanceID global commandID or previously opened local instanceID
   * @param argSeq command argument tuple packaged as Record<GenNode>, which
   *               is the standard format [sent](BusTerm::act(GenNode)) for
   *               command execution via [UI-bus](ui-bus.hpp)
   */
  void
  CommandInstanceManager::bindAndDispatch (Symbol instanceID, Rec const& argSeq)
  {
    Command instance{getCloneOrInstance (instanceID, false)};
    REQUIRE (instance);
    instance.bindArg (argSeq);
    ENSURE (instance.canExec());
    handOver (move (instance));
  }
  
  
  bool
  CommandInstanceManager::contains (Symbol instanceID)  const
  {
    return util::contains (table_, instanceID)
       and unConst(this)->table_[instanceID].isValid();
  }
  
  
  
}} // namespace proc::control
