/*
  COMMAND-IMPL.hpp  -  Proc-Layer command implementation (top level)
 
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


/** @file command-impl.hpp
 ** Top level of the command implementation. CommandImpl holds together
 ** the various data and sub-objects involved into the inner workings of a
 ** Proc-Layer command. It serves to implement a "command definition" (prototype)
 ** as well as a concrete command instance. It is a data holder with a well defined
 ** identity and usually located within the (pooled) storage managed by the
 ** CommandRegistry. Client code gets access to a specific CommandImpl through
 ** a Command instance, which is a small (refcounting smart-ptr) handle.
 ** 
 ** //TODO 
 **
 ** @see Command
 ** @see ProcDispatcher
 **
 */



#ifndef CONTROL_COMMAND_IMPL_H
#define CONTROL_COMMAND_IMPL_H

#include "proc/control/command.hpp"
#include "proc/control/command-mutation.hpp"
#include "proc/control/command-argument-holder.hpp"
#include "proc/control/typed-allocation-manager.hpp"
#include "lib/bool-checkable.hpp"

#include <boost/noncopyable.hpp>
#include <boost/operators.hpp>

#include <tr1/memory>
#include <tr1/functional>


namespace control {
  
  using std::tr1::function;
  using std::tr1::shared_ptr;

  
  
  /**
   * @todo Type-comment
   */
  class CommandImpl
    : public lib::BoolCheckable<CommandImpl
           , boost::noncopyable
           >
    {
      Mutation do_;
      UndoMutation undo_;
      
      shared_ptr<CmdClosure> pClo_;
      
      
      template<typename ARG>
      struct _Type
        {
          typedef typename ARG::SIG_op SIG_op;
          typedef typename ARG::SIG_cap SIG_cap;
          typedef typename ARG::SIG_undo SIG_undo;
          
          typedef function<SIG_op> Func_op;
          typedef function<SIG_cap> Func_cap;
          typedef function<SIG_undo> Func_undo;
        };
#define _TY(_ID_) typename _Type<ARG>::_ID_
      
    public:
      /** build a new implementation frame, and do the initial wiring.
       *  On the interface the specific type is discarded afterwards.
       *  This information is still kept though, as encoded into the vtable
       *  of the embedded FunErasure objects holding the command operation
       *  and undo functors, and the vtable of the embedded CmdClosure */
      template<typename ARG>
      CommandImpl (shared_ptr<ARG> pArgHolder
                  ,_TY (Func_op) const& operFunctor
                  ,_TY (Func_cap) const& captFunctor
                  ,_TY (Func_undo) const& undoFunctor
                  )
        : do_(operFunctor)
        , undo_(pArgHolder->tie (undoFunctor, captFunctor))
        , pClo_(pArgHolder)
        { }
      
#undef _TY      
      
      
     ~CommandImpl();
      
      
      /** cloning service for the CommandRegistry:
       *  effectively this is a copy ctor, but as we rely
       *  on a argument holder (without knowing the exact type),
       *  we need to delegate the cloning of the arguments down
       *  while providing a means of allocating storage for the clone */
      CommandImpl (CommandImpl const& orig, TypedAllocationManager& storageManager)
        : do_(orig.do_)
        , undo_(orig.undo_)
        , pClo_(orig.pClo_->createClone(storageManager))  
        { }
     
      
      /** core operation: invoke the command
       *  @param execPattern describes the individual steps
       *         necessary to get this command invoked properly
       */
      void exec (HandlingPattern const& execPattern);
      
      
      void setArguments (Arguments& args)
        {
          pClo_->bindArguments(args);
        }
            
      
      
      /* === diagnostics === */
      
      bool
      isValid()  const
        {
          UNIMPLEMENTED ("command validity self check");
        }
      
      bool
      canExec()  const
        {
          UNIMPLEMENTED ("state check: sufficiently defined to be invoked");
        }
      
      bool
      canUndo()  const
        {
          UNIMPLEMENTED ("state check: has undo state been captured?");
        }
      
      
      
      
    protected:
//     static Command& fetchDef (Symbol cmdID);
     
//     friend class CommandDef;

    };
  ////////////////TODO currently just fleshing  out the API....
  


  
  
} // namespace control
#endif
