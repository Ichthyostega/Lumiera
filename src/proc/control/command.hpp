/*
  COMMAND.hpp  -  Key abstraction for proc/edit operations and UNDO management
 
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
 
*/


/** @file command.hpp
 ** //TODO 
 **
 ** @see ProcDispatcher
 ** @see Session
 **
 */



#ifndef CONTROL_COMMAND_H
#define CONTROL_COMMAND_H

#include "pre.hpp"
#include "include/symbol.hpp"
#include "proc/control/command-binding.hpp"
#include "proc/control/command-mutation.hpp"         /////TODO: do we need to expose this here?
#include "proc/control/command-closure.hpp"
#include "proc/control/handling-pattern.hpp"
#include "lib/bool-checkable.hpp"
#include "lib/handle.hpp"

//#include <tr1/memory>
#include <string>

///////////////////////////////////////////TODO: define an C-API representation here, make the header multilingual!



namespace control {
  
  using std::string;
  using lumiera::Symbol;
  using std::tr1::shared_ptr;
  
  LUMIERA_ERROR_DECLARE (UNBOUND_ARGUMENTS);  ///< Command functor not yet usable, because arguments aren't bound
  LUMIERA_ERROR_DECLARE (INVALID_COMMAND);    ///< Unknown or insufficiently defined command
  LUMIERA_ERROR_DECLARE (DUPLICATE_COMMAND);  ///< Attempt to redefine an already existing command definition
  LUMIERA_ERROR_DECLARE (INVALID_ARGUMENTS);  ///< Arguments provided for binding doesn't match stored command function parameters

  

  typedef void* FuncPtr;
  
  class CommandDef;
  class CommandImpl;
  
  
  /**
   * @todo Type-comment
   */
  class Command
    : public com::ArgumentBinder<Command    // accepts arbitrary bind(..) calls (with runtime check)
           , lib::Handle<CommandImpl>      //  actually implemented as ref counting Handle
           >
    {
      typedef lib::Handle<CommandImpl> _Handle;
      
    public:
      /* === command registry === */
      static Command get (Symbol cmdID);
      static Command get (FuncPtr func);
      static bool remove (Symbol cmdID); 
      static bool  undef (Symbol cmdID);
      
      Command storeDef (Symbol newCmdID);
      
      Command() { } ///< undefined command
     ~Command();
      
      ExecResult operator() () ;
      ExecResult undo () ;
      
      
      /** core operation: invoke the command
       *  @param execPattern describes the individual steps
       *         necessary to get this command invoked properly
       */
      ExecResult exec (HandlingPattern const& execPattern);
      ExecResult exec (HandlingPattern::ID);
      
      ExecResult execSync ();
      
      /** @return ID of the execution pattern used by operator() */
      HandlingPattern::ID getDefaultHandlingPattern()  const;
      
      /** define a handling pattern to be used by default
       *  @return ID of the currently defined default pattern */
      HandlingPattern::ID setHandlingPattern (HandlingPattern::ID);
      
      
      
      /* === command lifecycle === */
      
      Command& activate (shared_ptr<CommandImpl> const&);
      
      template<typename TYPES>
      Command& bindArg (Tuple<TYPES> const&);
      
      
      /* === diagnostics === */
      
      static size_t definition_count();
      static size_t instance_count();
      
      bool canExec()  const;
      bool canUndo()  const;
      
      operator string() const;
      friend bool operator== (Command const&, Command const&);
      
      
    protected:
      static Command fetchDef (Symbol cmdID);
      
      friend class CommandDef;
      
      
    private:
      void setArguments (Arguments&);
    };
  ////////////////TODO currently just fleshing  out the API....
  

  inline ExecResult
  Command::operator() ()
  {
    return exec (getDefaultHandlingPattern());
  }
  
  
  template<typename TYPES>
  inline Command&
  Command::bindArg (Tuple<TYPES> const& tuple)
    {
      TypedArguments<Tuple<TYPES> > args(tuple);
      this->setArguments (args);
      return *this;
    }
  
  
  
  inline bool
  operator== (Command const& c1, Command const& c2)
    {
      return (!c1 && !c2)
          || ( c1 &&  c2  && (&c1.impl() == &c2.impl())); 
    }
  
  inline bool
  operator!= (Command const& c1, Command const& c2)
    {
      return ! (c1 == c2); 
    }
  

  
  
} // namespace control
#endif
