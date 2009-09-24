/*
  COMMAND-INVOCATION.hpp  -  shortcuts and helpers for calling Proc-Layer commands
 
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


/** @file command-invocation.hpp
 ** Some additional helpers and convenience shortcuts to ease command invocation.
 ** Especially, this header defines a set of free \c invoke(...) functions,
 ** allowing to bind to specific arguments and then invoke through the default
 ** HandlingPatern in a single call.
 **
 ** @see Command
 ** @see CommandDef
 ** @see command-binding.hpp
 ** @see command-use1-test.cpp
 **
 */



#ifndef CONTROL_COMMAND_INVOCACTION_H
#define CONTROL_COMMAND_INVOCACTION_H

//#include "pre.hpp"
//#include "lib/symbol.hpp"
//#include "lib/meta/typelist.hpp"
//#include "lib/meta/tuple.hpp"
#include "proc/control/command.hpp"
#include "proc/control/handling-pattern.hpp"
#include "proc/control/argument-tuple-accept.hpp"

//#include <tr1/memory>
#include <tr1/functional>




namespace control {
  
//  using lib::Symbol;
//  using std::tr1::shared_ptr;
  using namespace lumiera::typelist;
  

  namespace com { // define transient invoker objects, to allow for arbitrary bindings
  
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
     */
    struct RuntimeCheckedCommandInvoker
      {
        Command com_;

        RuntimeCheckedCommandInvoker (Command c)
          : com_(c)
          { }
        
        
        template< typename T1>
        ExecResult                                                                        //_________________________________
        operator() (T1 a1)                                                               ///< Invoke command with 1 Argument.
          {
            com_.bind(a1);
            return com_();
          }
        
        
        template< typename T1
                , typename T2
                , typename T3
                >
        ExecResult                                                                        //_________________________________
        operator() (T1 a1, T2 a2, T3 a3)                                                 ///< Invoke command with 3 Arguments
          {
            com_.bind(a1,a2,a3);
            return com_();
          }
      };
  }
  
  
  
  template<typename SIG>
  inline
  com::CommandInvoker<SIG>
  invoke (SIG& operation_func)
  {
    Command command = Command::get ((FuncPtr)&operation_func);
    ASSERT (command);
    return com::CommandInvoker<SIG> (command);
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
  
  
} // namespace control
#endif
