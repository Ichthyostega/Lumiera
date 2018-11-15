/*
  COMMAND-INVOCATION.hpp  -  shortcuts and helpers for calling Steam-Layer commands

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


/** @file command-invocation.hpp
 ** Some additional helpers and convenience shortcuts to ease command invocation.
 ** Especially, this header defines a set of free \c invoke(...) functions,
 ** allowing to bind to specific arguments and then invoke through the default
 ** HandlingPatern in a single call.
 ** 
 ** While in some cases, a command will be set up completely finished and closed
 ** over all it's arguments, usually the CommandDef will just specify the command operation
 ** and undo function, thus leaving the task of binding concrete arguments to the client code.
 ** Thus, depending on the circumstances, there might be usage situations where the exact number
 ** and type of arguments can be detected and checked at compile time, while otherwise this check
 ** needs to be deferred to happen at runtime, when the binding is actually invoked.
 ** 
 ** @see Command
 ** @see CommandDef
 ** @see command-binding.hpp
 ** @see command-use1-test.cpp
 **
 */



#ifndef CONTROL_COMMAND_INVOCACTION_H
#define CONTROL_COMMAND_INVOCACTION_H

#include "proc/control/command.hpp"
#include "proc/control/handling-pattern.hpp"
#include "proc/control/argument-tuple-accept.hpp"

#include <utility>



namespace proc {
namespace control {
  
  using namespace lib::meta;
  
  
  
  namespace com { ///< Proc-Layer command implementation details
                 //    define transient invoker objects, to allow for arbitrary bindings
    
    template<typename SIG>
    struct _DefineBindFunc
      {
        typedef ExecResult SIG_Bind(int);
      };
    
    
    /** @internal transient invoker object for invoking the command based
     *            on a function provided at compile time. Because of the
     *            known signature, we're able to expose a precisely typed
     *            function invocation operator.
     */
    template<typename SIG>
    struct CommandInvoker
      : AcceptArgumentTuple< typename _DefineBindFunc<SIG>::SIG_Bind
                           , CommandInvoker<SIG>
                           >
      {
        Command com_;
        
        CommandInvoker (Command c)
          : com_(c)
          { }
        
        
        template<typename TYPES>
        ExecResult
        bindArg (Tuple<TYPES> const& args)
          {
            com_.bindArg(args);
            ASSERT (com_.canExec());
            return com_();
          }
      };
    
    
    
    /** @internal transient invoker object, usable when the exact signature
     *            of the command's operation isn't known at compile time.
     *            In this case, we allow any invocation call to compile,
     *            but the command will reject unsuitable signatures
     *            at runtime, when fetching the operation functor.
     *  @remarks  actually this kind of invocation is the default case,
     *            since commands are defined statically at application start-up
     *            and invoked via the UI-Bus by command-ID
     */
    struct RuntimeCheckedCommandInvoker
      {
        Command com_;
        
        RuntimeCheckedCommandInvoker (Command c)
          : com_(c)
          { }
        
        
        /** invoke command with arbitrary arguments
         * @warning argument types will be checked at runtime only
         */
        template<typename...ARGS>
        ExecResult
        operator() (ARGS&& ...args)
          {
            com_.bind (std::forward<ARGS> (args)...);
            return com_();
          }
      };
  }
  
  
  
  inline
  com::RuntimeCheckedCommandInvoker
  invoke (Symbol cmdID)
  {
    Command command = Command::get (cmdID);
    ASSERT (command);
    return com::RuntimeCheckedCommandInvoker (command);
  }
  
  
  inline
  com::RuntimeCheckedCommandInvoker
  invoke (const char* cmdID)
  {
    return invoke(Symbol(cmdID));
  }
  
  
}} // namespace proc::control
#endif
