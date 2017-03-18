/*
  COMMAND-INSTANCE-SETUP.hpp  -  Key abstraction for proc/edit operations and UNDO management

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
 ** A *Proc-Layer command* is a functor, which can be parametrised with actual arguments.
 ** It needs to be [defined](command-def.hpp) beforehand, which means to establish an unique
 ** name and to supply three functions, one for the actual command operation, one to capture
 ** state and one to _undo_ the effect of the command invocation. CommandSetup allows to create
 ** series of such definitions with minimal effort. Since any access and mutation from the UI into
 ** the Session data must be performed by invoking such commands, a huge amount of individual command
 ** definitions need to be written eventually.
 ** 
 ** The macro COMMAND_DEFINITION(name) allows to introduce a new definition with a single line,
 ** followed by a code block, which actually ends up as the body of a lambda function, and receives
 ** the bare CommandDef as single argument with name `cmd`. The `name` argument of the macro ends up
 ** both stringified as the value of the command-ID, and as an identifier holding a new CommandSetup
 ** instance. It is assumed that a header with corresponding _declarations_ (the header \ref cmd.hpp)
 ** is included by all UI elements actually to use, handle and invoke commands towards the
 ** session-command-facade.h
 ** 
 ** @todo WIP-WIP 3/2017 initial draft
 ** 
 ** @see command-def.hpp
 ** @see command.hpp
 ** @see command-accessor.hpp
 ** @see TODO_CommandSetup_test
 **
 */



#ifndef CONTROL_COMMAND_SETUP_H
#define CONTROL_COMMAND_SETUP_H

#include "lib/error.hpp"
#include "proc/control/command.hpp"
#include "lib/symbol.hpp"
//#include "proc/common.hpp"

#include <boost/noncopyable.hpp>
#include <string>




namespace proc {
namespace control {
  
  using std::string;
//  using lib::Symbol;
  using lib::Literal;
  //using std::shared_ptr;
  
  
  
  /**
   * @todo write type comment
   */
  class CommandSetup
    {
      Literal cmdID_;
      
    public:
     ~CommandSetup();
      CommandSetup (Literal cmdID);
      CommandSetup (CommandSetup const&)  = delete;
      CommandSetup (CommandSetup &&)      = default;
      CommandSetup& operator= (CommandSetup const&) = delete;
      
      operator Literal const&()  const
        {
          return cmdID_;
        }
      operator string()  const
        {
          return string{cmdID_};
        }
      
      
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
  
  
  
  
  /** */
  
  
  
  
}} // namespace proc::control
#endif /*CONTROL_COMMAND_SETUP_H*/
