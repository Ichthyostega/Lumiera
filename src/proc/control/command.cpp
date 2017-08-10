/*
  Command  -  Key abstraction for proc/edit operations and UNDO management

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file command.cpp
 ** Implementation of the command frontend.
 ** Within this file, the implementation level of the command frontend
 ** is linked to the implementation of the command registry. Client code
 ** is shielded from those implementation classes and need only include
 ** command.hpp
 ** 
 ** More specifically, the actual number and type of arguments and the
 ** concrete functions implementing the command operation are known only
 ** at the time of the command definition; this detailed type information
 ** is erased afterwards, allowing client code to use a simple frontend.
 ** 
 ** @see command.hpp
 ** @see command-registry.hpp
 **
 */


#include "lib/util.hpp"
#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/format-string.hpp"
#include "proc/control/command.hpp"
#include "proc/control/command-def.hpp"
#include "proc/control/command-impl.hpp"
#include "proc/control/command-registry.hpp"
#include "proc/control/command-impl-clone-builder.hpp"
#include "proc/control/handling-pattern.hpp"

#include <utility>
#include <sstream>
#include <string>

using std::ostringstream;
using std::string;
using std::move;
using util::cStr;
using util::_Fmt;


namespace proc {
namespace control {
  namespace error = lumiera::error;
  
  LUMIERA_ERROR_DEFINE (INVALID_COMMAND,   "Unknown or insufficiently defined command");
  LUMIERA_ERROR_DEFINE (DUPLICATE_COMMAND, "Attempt to redefine an already existing command definition");
  LUMIERA_ERROR_DEFINE (INVALID_ARGUMENTS, "Arguments provided for binding doesn't match stored command function parameters");
  LUMIERA_ERROR_DEFINE (UNBOUND_ARGUMENTS, "Command mutation functor not yet usable, because arguments aren't bound");
  LUMIERA_ERROR_DEFINE (MISSING_MEMENTO,   "Undo functor not yet usable, because no undo state has been captured");
  
  
  namespace { // some common error checks...
    
    void
    ___check_notBottom (const Command *handle, lib::Literal operation_descr)
    {
      REQUIRE (handle);
      if (!handle->isValid())
        throw error::Invalid (operation_descr+" an undefined command"
                             , LUMIERA_ERROR_INVALID_COMMAND);
    }
    
    void
    ___check_isBound (const Command *handle)
    {
      REQUIRE (handle);
      if (!handle->canExec())
        throw error::State ("Lifecycle error: command arguments not bound"
                           , LUMIERA_ERROR_UNBOUND_ARGUMENTS);
    }
    
    void
    ___check_canUndo (const Command *handle)
    {
      REQUIRE (handle);
      if (!handle->canUndo())
        throw error::State ("Lifecycle error: command has not yet captured UNDO information"
                           , LUMIERA_ERROR_UNBOUND_ARGUMENTS);
    }
    
  }
  
  
  
  
  /** storage for the singleton factory used to access CommandRegistry */
  lib::Depend<CommandRegistry> CommandRegistry::instance;
  
  
  Command::~Command() { }
  CommandImpl::~CommandImpl() { }
  
  
  
  
  /** Access existing command for use.
   *  @throw error::Invalid if command not 
   *         registered or incompletely defined.
   *  @remark this function deliberately returns by-value.
   *         Returning a reference into the global CommandRegistry
   *         would be dangerous under concurrent access.
   */
  Command
  Command::get (Symbol cmdID)
  {
    Command cmd = CommandRegistry::instance().queryIndex (cmdID);
    if (!cmd)
      throw error::Invalid(_Fmt("Command \"%s\" not found") % cmdID
                          , LUMIERA_ERROR_INVALID_COMMAND);
    
    ENSURE (cmdID == CommandRegistry::instance().findDefinition(cmd));
    return cmd;
  }
  
  
  /** try to access an existing command definition and immediately
   *  create a new clone copy by calling #newInstance()
   * @return when search for this ID was successful, a new anonymous
   *         command instance based on the same definition, but not registered
   *         in the global CommandRegistry. Otherwise, when search fails,
   *         an empty Command instance (as can be tested with `bool` conversion
   */
  Command
  Command::maybeGetNewInstance (Symbol cmdID)
  {
    Command cmd = CommandRegistry::instance().queryIndex (cmdID);
    return cmd? cmd.newInstance()
              : Command{};
  }
  
  
  /** @internal just query an existing instance, if any. */
  Command
  Command::fetchDef (Symbol cmdID)
  {
    return CommandRegistry::instance().queryIndex (cmdID);
  }
  
  
  /** @internal make a command ready for use. Typically to be invoked
   *  through CommandDef during the definition stage, but also used
   *  for activating (anonymous) clone instances.
   *  @param cmdID new ID for creating a separate command registration when provided
   *  @throw error::Logic when \c this is already activated. */
  void
  Command::activate (shared_ptr<CommandImpl> && implFrame, Symbol cmdID)
  {
    REQUIRE (implFrame);
    
    if (this->isValid())
      duplicate_detected (cmdID);
    
    _Handle::activate (move (implFrame));
    if (cmdID)
      {
        CommandRegistry::instance().track (cmdID, *this);
        impl().cmdID = cmdID;
      }
    
    TRACE (command, "%s defined OK", cStr(*this));
  }
  
  
  /** create an independent clone copy of this command
   *  and register it as new command definition under a different ID */
  Command
  Command::storeDef (Symbol newCmdID)  const
  {
    CommandRegistry& registry = CommandRegistry::instance();
    
    ___check_notBottom (this,"Cloning");
    if (registry.queryIndex (newCmdID))
      duplicate_detected (newCmdID);
    
    Command cloneDefinition;
    cloneDefinition.activate (move (registry.createCloneImpl(this->impl())), newCmdID);
    ENSURE (cloneDefinition);
    return cloneDefinition; 
  }
  
  
  /** create independent (anonymous) clone copy of this command */
  Command
  Command::newInstance ()  const
  {
    ___check_notBottom (this,"Cloning");
    CommandRegistry& registry = CommandRegistry::instance();
    shared_ptr<CommandImpl> cloneImpl = registry.createCloneImpl(this->impl());
    
    Command clone;
    clone.activate (move (cloneImpl));
    ENSURE (clone);
    return clone; 
  }
  
  
  /** @note this bit of implementation from CommandRegistry rather
   *  heavily relies on implementation details from CommandImpl and
   *  the help of CommandImplCloneBuilder and StorageHolder. It's
   *  implemented within command.cpp to keep the includes of
   *  the handling patterns clean. */
  shared_ptr<CommandImpl>
  CommandRegistry::createCloneImpl (CommandImpl const& refObject)
  {
    CommandImplCloneBuilder cloneBuilder(allocator_);
    refObject.prepareClone(cloneBuilder);
    return allocator_.create<CommandImpl> (refObject, cloneBuilder.clonedUndoMutation() 
                                                    , cloneBuilder.clonedClosuere());
  }
  
  
  
  void
  Command::duplicate_detected (Symbol newCmdID)  const
  {
    throw error::Logic (_Fmt("Unable to store %s as new command. "
                             "ID \"%s\" is already in use")
                            % *this
                            % newCmdID
                       , LUMIERA_ERROR_DUPLICATE_COMMAND);
  }
  
  
  bool
  Command::remove (Symbol cmdID)
  {
    return CommandRegistry::instance().remove (cmdID); 
  }
  
  
  /** @internal pass a new argument tuple to the
   *  CommandImpl without exposing implementation.
   */
  void
  Command::setArguments (Arguments& args)
  {
    ___check_notBottom (this, "Binding arguments of");
    _Handle::impl().setArguments(args);
  }
  
  
  /** @internal forward a `Record<GenNode>`, which was
   *  typically received via UI-Bus, down to the CommandImpl.
   *  @remarks this is how command arguments are actually
   *           passed from UI to the Session core
   */
  void
  Command::setArguments (lib::diff::Rec const& paramData)
  {
    ___check_notBottom (this, "Binding arguments of");
    _Handle::impl().setArguments(paramData);
  }
  
  
  /** discard any argument data previously bound.
   * @remarks this returns the command into pristine state,
   *          just holding the definition but not any further
   *          argument data or UNDO state. Contrast this to an
   *          defalut constructed Command, which is inactive
   *          and not bound to any definition at all.
   */
  Command&
  Command::unbind()
  {
    ___check_notBottom (this, "Un-binding arguments of");
    _Handle::impl().discardArguments();
    return *this;
  }
  
  
  /** @return the number of command \em definitions currently registered */
  size_t
  Command::definition_count()
  {
    return CommandRegistry::instance().index_size();
  }
  
  
  
  /** @return number of distinguishable registered command \em instances */
  size_t
  Command::instance_count()
  {
    return CommandRegistry::instance().instance_count();
  }
  
  
  
  namespace {
    inline bool
    was_activated (Command const& com)
    {
      return com.isValid();
    }
    
    inline Command
    registered_for (Symbol id)
    {
      return CommandRegistry::instance().queryIndex (id);
    }
  }
  
  
  /** when starting a CommandDef, we immediately place a yet empty
   *  Command object into the index, just assuming it will be defined
   *  properly and consequently get valid at some point. But in case
   *  this doesn't happen (e.g. because the definition is aborted),
   *  we need to clean up this empty pre-registration...
   */
  CommandDef::~CommandDef()
  {
    if (!was_activated (prototype_))
      CommandRegistry::instance().remove (this->id_);
  }
  
  
  /** is this a valid command definition? especially..
   *  - the prototype command is initialised properly
   *  - there is a command definition registered for our command ID
   *  - and the registered command uses the same underlying command impl
   *    record than our prototype
   */
  bool
  CommandDef::isValid()  const
  {
    return (was_activated (prototype_))
        && (prototype_ == registered_for (this->id_))
         ;
  }
  
  
  
  bool
  Command::canExec()  const
  {
    return isValid()
        && impl().canExec();
  }
  
  
  bool
  Command::canUndo()  const
  {
    return isValid()
        && impl().canUndo();
  }
  
  
  bool
  Command::equivalentImpl (Command const& c1, Command const& c2)
  {
    return c1.impl() == c2.impl();
  }
  
  
  Symbol
  Command::getID()  const noexcept
  {
    return isValid()? impl().cmdID
                    : Symbol::FAILURE;
  }
  
  
  /** @return `true` when this command (front-end) was never registered
   * with the CommandRegistry; typically this is the case with instances
   * created from a prototype, when calling Command::newInstance instead
   * of invoking Command::storeDef(Symbol).
   */
  bool
  Command::isAnonymous()  const
  {
    return not CommandRegistry::instance().findDefinition (*this);
  }
  
  
  
  
  /** diagnostics: shows the commandID, if any,
   *  and the degree of definition of this command */
  Command::operator string() const
  {
    ostringstream repr;
    ////////////////////////////////////////////////////////////////////TODO do we need no-throw guarantee here?
    repr << "Command(\""<<getID()<<"\") ";
    if (!isValid())
      repr << "NIL";
    else
      if (canUndo())
        repr << "{undo}";
      else
        if (canExec())
          repr << "{exec}";
        else
            repr << "{def}";
    
    return repr.str();
  }
  
  
  
  
  ExecResult
  Command::exec (HandlingPattern const& execPattern)
  {
    ___check_notBottom (this,"Invoking");
    ___check_isBound   (this);
    
    string cmdName{*this};
    CommandImpl& thisCommand (_Handle::impl());
    return execPattern.exec (thisCommand, cmdName);
  }
  
  
  ExecResult
  Command::undo (HandlingPattern const& execPattern)
  {
    ___check_notBottom (this,"UNDOing");
    ___check_canUndo   (this);
    
    string cmdName{*this};
    CommandImpl& thisCommand (_Handle::impl());
    return execPattern.undo (thisCommand, cmdName);
  }
  
  
  ExecResult
  Command::exec (HandlingPattern::ID pattID)
  {
    return exec (HandlingPattern::get(pattID));
  }
  
  
  ExecResult
  Command::undo (HandlingPattern::ID pattID)
  {
    return undo (HandlingPattern::get(pattID));
  }
  
  
  ExecResult
  Command::execSync ()
  {
    return exec (HandlingPattern::get(HandlingPattern::DUMMY));       ///////////TICKET #211 : should be ID::SYNC_THROW
  }
  
  
  HandlingPattern::ID
  Command::getDefaultHandlingPattern()  const
  {
    ___check_notBottom (this,"Accessing");
    return impl().getDefaultHandlingPattern();
  }
  
  
  HandlingPattern::ID
  Command::setHandlingPattern (HandlingPattern::ID pattID)
  {
    ___check_notBottom (this, "Configuring");
    return impl().setHandlingPattern(pattID);
  }
  
  
  
  
  
}} // namespace proc::control
