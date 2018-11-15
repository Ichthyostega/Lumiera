/*
  COMMAND-INSTANCE-SETUP.hpp  -  Key abstraction for steam/edit operations and UNDO management

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

*/


/** @file command-setup.hpp
 ** Provision for setup of concrete commands for use by the UI.
 ** A *Steam-Layer command* is a functor, which can be parametrised with actual arguments.
 ** It needs to be [defined](\ref command-def.hpp) beforehand, which means to establish an unique
 ** name and to supply three functions, one for the actual command operation, one to capture
 ** state and one to _undo_ the effect of the command invocation. CommandSetup allows to create
 ** series of such definitions with minimal effort. Since any access and mutation from the UI into
 ** the Session data must be performed by invoking such commands, a huge amount of individual command
 ** definitions need to be written eventually.
 ** 
 ** The macro COMMAND_DEFINITION(_NAME_) allows to introduce a new definition with a single line,
 ** followed by a code block, which actually ends up as the body of a lambda function, and receives
 ** the bare CommandDef as single argument with name `cmd`. The `_NAME_` argument of the macro ends up
 ** both stringified as the value of the command-ID, and as an variable holding a new CommandSetup
 ** instance. It is assumed that a header with corresponding _declarations_ (the header \ref cmd.hpp)
 ** is included by all UI elements actually to use, handle and invoke commands towards the
 ** session-command-facade.h
 ** 
 ** @see command-def.hpp
 ** @see command.hpp
 ** @see CommandSetup_test
 ** @see cmd.hpp
 ** @see cmd-context.hpp
 **
 */



#ifndef CONTROL_COMMAND_SETUP_H
#define CONTROL_COMMAND_SETUP_H

#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "steam/control/command.hpp"
#include "lib/symbol.hpp"

#include <functional>
#include <string>




namespace steam {
namespace control {
  
  using std::string;
  using lib::Symbol;
  
  class CommandDef;
  
  using DefinitionClosure = std::function<void(CommandDef&)>;
  
  
  
  /**
   * Marker and Helper for writing Steam-Layer Command definitions.
   * Together with the Macro #COMMAND_DEFINITION, such definitions
   * may be written statically, in DSL-style:
   * - statically define a variable of type CommandSetup,
   *   with external linkage
   * - the ctor argument is what will be used as command-ID
   * - assign a functor, function or lambda to this variable,
   *   with the signature `void(CommandDef&)`
   * - the argument passed to this functor will be the CommandDef
   *   about to be configured and registered. Thus, the body of the
   *   functor should use the member functions of CommandDef to setup
   *   the command's operation, state capturing and undo functions.
   * - behind the scenes, a lumiera::LifecycleHook is scheduled
   *   to run ON_GLOBAL_INIT. When this hook is activated, all the
   *   lambdas assigned to all CommandSetup instances thus far will
   *   be invoked one by one. Which causes all those commands actually
   *   to be defined and configured for use with the session subsystem.
   */
  class CommandSetup
    : util::Cloneable   // copy-construction allowed, but no assignment
    {
      Symbol cmdID_;
      
    public:
     ~CommandSetup();
      CommandSetup (Symbol cmdID);
      
      operator Symbol const&()  const
        {
          return cmdID_;
        }
      operator string()  const
        {
          return string{cmdID_};
        }
      
      /** core functionality: provide a command definition block. */
      CommandSetup& operator= (DefinitionClosure);
      
      /** @internal empty the definition queue and invoke definitions */
      static void invokeDefinitionClosures();
      
      /** diagnostics / test */
      static size_t pendingCnt();
      
      friend bool
      operator== (CommandSetup const& left, CommandSetup const& right)
      {
        return left.cmdID_ == right.cmdID_;
      }
      friend bool
      operator!= (CommandSetup const& left, CommandSetup const& right)
      {
        return left.cmdID_ != right.cmdID_;
      }
    };
  
  
  
  
  /**
   * Macro to write command definitions in a compact form.
   * On expansion, a variable of type CommandSetup will be defined in the current scope,
   * and immediately be assigned by a lambda, whose body is what follows the macro invocation
   */
  #define COMMAND_DEFINITION(_NAME_) \
    CommandSetup _NAME_ = CommandSetup{STRINGIFY(_NAME_)}  = [&](CommandDef& def)
  
  
  
}} // namespace steam::control
#endif /*CONTROL_COMMAND_SETUP_H*/
