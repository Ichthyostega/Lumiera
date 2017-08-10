/*
  COMMAND.hpp  -  Key abstraction for proc/edit operations and UNDO management

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

*/


/** @file command.hpp
 ** Proc-Layer command frontend.
 ** A *command* is a functor, which can be invoked according to a pre-defined HandlingPattern.
 ** Most notably, command invocation can be scheduled and logged with the serialiser, and the effect
 ** of any command invocation can be _undone_ later on by invoking the "undo operation" defined
 ** alongside with the command's operation. The command operation is defined through a C/C++ function
 ** and may receive an arbitrary number and type of arguments. After setting up such a CommandDef ,
 ** it can be referred for use through a symbolic ID. Before being able to invoke the command, concrete
 ** function arguments need to be provided (this is called "binding" or "closing the function arguments").
 ** These function arguments are stored within the command definition and remain opaque to the client code
 ** actually invoking the command. Behind the scenes, there is a CommandRegistry, holding an index of the
 ** registered commands and managing the storage for command definitions and arguments. The actual 
 ** Command object used by client code is a small, copyable and ref-counting handle to this 
 ** stored definition backend.
 ** 
 ** # Command definition, argument types and UNDO operation
 ** For a command to be usable at all, a concrete [command definition](command-def.hpp) need to be supplied
 ** somewhere in the code base. Typically this is done through static [command-setup bindings](command-setup.hpp).
 ** Such a command definition links three functions with the name-ID of the command
 ** - the actual command operation
 ** - a function to capture state
 ** - a function to undo the effect of the operation
 ** These functions may take arbitrary arguments, but the signature of this operation is captured at
 ** compile time and embedded as invisible type information within the implementation storage. While this
 ** allows to verify the type of the actual command arguments, this type sanity check can happen only late,
 ** at invocation time (and result in raising an exception). In a similar vein, the state capturing and
 ** the undo function must match with the signature of the main operation, since these functions are
 ** provided with the same arguments as presented to the command function. The UNDO functionality
 ** in Lumiera is based on capturing a _state memento_, not on applying a reverse function.
 ** So it is the responsibility of the capture function to take a suitable state snapshot,
 ** such as to be able to revert to the situation before activating this command, but
 ** only as far as the effect of the actual command function is concerned.
 ** 
 ** @see command-def.hpp
 ** @see command-use1-test.cpp
 ** @see command-use2-test.cpp
 ** @see command-use3-test.cpp
 ** @see ProcDispatcher
 ** @see Session
 **
 */



#ifndef CONTROL_COMMAND_H
#define CONTROL_COMMAND_H

#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "proc/common.hpp"
#include "proc/control/argument-erasure.hpp"
#include "proc/control/argument-tuple-accept.hpp"
#include "proc/control/handling-pattern.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/handle.hpp"

#include <string>




namespace proc {
namespace control {
  
  using std::string;
  using lib::Symbol;
  using std::shared_ptr;
  using lib::meta::Tuple;
  using lib::meta::Types;
  
  
  LUMIERA_ERROR_DECLARE (UNBOUND_ARGUMENTS);  ///< Command functor not yet usable, because arguments aren't bound
  LUMIERA_ERROR_DECLARE (INVALID_COMMAND);    ///< Unknown or insufficiently defined command
  LUMIERA_ERROR_DECLARE (DUPLICATE_COMMAND);  ///< Attempt to redefine an already existing command definition
  LUMIERA_ERROR_DECLARE (INVALID_ARGUMENTS);  ///< Arguments provided for binding doesn't match stored command function parameters
  
  
  
  typedef void* FuncPtr;
  
  class CommandDef;
  class CommandImpl;
  
  
  /**
   *  Handle object representing a single Command instance to be used by client code.
   *  Commands are accessed \link #get through a symbolic ID \endlink; there needs to be
   *  a CommandDef somewhere to specify the actual operation and to define, how the
   *  effect of the command can be undone. Moreover, the command's definition 
   *  refers to a HandlingPattern, which describes how the command is actually
   *  to be executed (the default is scheduling it within the ProcDispatcher)
   *  
   *  Client code usually just
   *  - creates a command instance by referring to a command ID
   *  - maybe binds to concrete arguments (e.g. a target object)
   *  - triggers command execution through operator()
   *  - maybe checks the return value for errors
   *  
   * @remark lightweight implementation as smart-ptr
   */
  class Command
    : public AcceptAnyBinding<Command       // accepts arbitrary bind(..) calls (with runtime check)
           , Command&                      //  (return value of bind() functions is *this)
           , lib::Handle<CommandImpl>     //   actually implemented as ref counting Handle
           >                             //
    {
      typedef lib::Handle<CommandImpl> _Handle;
      
    public:
      /* === command registry === */
      static Command get (Symbol cmdID);
      static bool remove (Symbol cmdID);
      
      /** create a clone definition */
      Command storeDef (Symbol newCmdID)  const;
      Command newInstance ()  const;
      
      static Command maybeGetNewInstance (Symbol cmdID);
      
      
      Command (Symbol cmdID) { *this = get (cmdID); }
      Command() { }          ///< undefined command
     ~Command();
      
      // default copy acceptable
      Command (Command &&)  = default;
      Command (Command const&) = default;
      Command& operator= (Command &&) = default;
      Command& operator= (Command const&) = default;
      
      
      
      /* === command lifecycle === */
      
      template<typename...TYPES>
      Command& bindArg (std::tuple<TYPES...> const&);
      
      Command& bindArg (lib::diff::Rec const&);
      
      Command& unbind();
      
      
      ExecResult operator() () ;
      ExecResult exec () ;
      ExecResult undo () ;
      
      
      /** core operation: invoke the command
       *  @param execPattern describes the individual steps
       *         necessary to get this command invoked properly
       */
      ExecResult exec (HandlingPattern const& execPattern);
      ExecResult exec (HandlingPattern::ID);
      
      ExecResult undo (HandlingPattern const& execPattern);
      ExecResult undo (HandlingPattern::ID);
      
      /** invoke using a default "synchronous" execution pattern */
      ExecResult execSync ();
      
      /** @return ID of the execution pattern used by operator() */
      HandlingPattern::ID getDefaultHandlingPattern()  const;
      
      /** define a handling pattern to be used by default
       *  @return ID of the currently defined default pattern */
      HandlingPattern::ID setHandlingPattern (HandlingPattern::ID);
      
      
      
      /* === diagnostics === */
      
      static size_t definition_count();
      static size_t instance_count();
      
      bool canExec()  const;
      bool canUndo()  const;
      
      static bool defined (Symbol cmdID);
      static bool canExec (Symbol cmdID);
      static bool canUndo (Symbol cmdID);
      
      void duplicate_detected (Symbol)  const;
      
      Symbol getID() const noexcept;
      bool isAnonymous() const;
      
      operator string() const;
      friend bool operator== (Command const&, Command const&);
      friend bool operator<  (Command const&, Command const&);
      
      
    protected:
      static Command fetchDef (Symbol cmdID);
      void activate (shared_ptr<CommandImpl> &&, Symbol cmdID =0);
      
      friend class CommandDef; //...invoking those two functions during definition stage
      
      
    private:
      void setArguments (Arguments&);
      void setArguments (lib::diff::Rec const&);
      static bool equivalentImpl (Command const&, Command const&);
    };
  
  
  
  
  
  inline ExecResult
  Command::operator() ()
  {
    return exec (getDefaultHandlingPattern());
  }
  
  inline ExecResult
  Command::exec ()
  {
    return exec (getDefaultHandlingPattern());
  }
  
  inline ExecResult
  Command::undo ()
  {
    return undo (getDefaultHandlingPattern());
  }
  
  
  template<typename...TYPES>
  inline Command&
  Command::bindArg (std::tuple<TYPES...> const& tuple)
  {
    TypedArguments<std::tuple<TYPES...>> args(tuple);
    this->setArguments (args);
    return *this;
  }
  
  
  inline Command&
  Command::bindArg (lib::diff::Rec const& paramData)
  {
    this->setArguments (paramData);
    return *this;
  }

  
  
  
  /* == state predicate shortcuts == */
  
  inline bool
  Command::defined (Symbol cmdID)
  {
    return fetchDef(cmdID).isValid();
  }
  
  
#define _FAILSAFE_COMMAND_QUERY(_ID_, _QUERY_) \
      try                                       \
        {                                        \
          return Command::get(_ID_)._QUERY_;      \
        }                                          \
      catch(lumiera::error::Invalid&)              \
        {                                          \
          lumiera_error(); /* ignore errorstate */ \
          return false;                            \
        }
  
  
  inline bool
  Command::canExec (Symbol cmdID)
  {
    _FAILSAFE_COMMAND_QUERY (cmdID, canExec() );
  }
  
  
  inline bool
  Command::canUndo (Symbol cmdID)
  {
    _FAILSAFE_COMMAND_QUERY (cmdID, canUndo() );
  }

#undef _FAILSAFE_COMMAND_QUERY
  
  
  
  
  inline bool
  operator== (Command const& c1, Command const& c2)
  {
    return (!c1 && !c2)
        || ( c1 &&  c2  && ( &c1.impl() == &c2.impl()
                           || Command::equivalentImpl (c1,c2)
                           )); 
  }
  
  inline bool
  operator!= (Command const& c1, Command const& c2)
  {
    return not (c1 == c2);
  }
  
  /** allow for sets and associative containers */
  inline bool
  operator< (Command const& c1, Command const& c2)
  {
    return (!c1 && c2)
        || ( c1 && c2 && (&c1.impl() < &c2.impl())); 
  }
  
  
  
  
}} // namespace proc::control
#endif
