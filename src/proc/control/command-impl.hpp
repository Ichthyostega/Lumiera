/*
  COMMAND-IMPL.hpp  -  Proc-Layer command implementation (top level)

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
#include "proc/control/command-closure.hpp"
#include "proc/control/command-mutation.hpp"
#include "lib/format-string.hpp"
#include "lib/nocopy.hpp"

#include <memory>
#include <functional>


namespace proc {
namespace control {
  
  using util::_Fmt;
  using std::function;
  using std::shared_ptr;
  
  
  
  /**
   * Proc-Layer Command implementation.
   * Data record holding together the parts necessary for command execution
   * - command operation functor
   * - a functor to UNDO the command effect
   * - closure holding actual parameters and UNDO state
   * @remarks the ctor is templated on the concrete type of operation arguments.
   *    This information is erased (discarded) immediately after construction.
   *    Usually, CommandImpl instances are created
   *    [within the registry](\ref CommandRegistry::newCommandImpl()).
   *    The implementation type of the closure is `StorageHolder<SIG_OPER,Mem>`
   * @see CmdClosure Closure interface
   * @see StorageHolder
   * @see Mutation
   */
  class CommandImpl
    : util::NonCopyable
    {
      Mutation do_;
      UndoMutation undo_;
      
      shared_ptr<CmdClosure> pClo_;
      
      HandlingPattern::ID defaultPatt_;
      
      
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
      CommandImpl (shared_ptr<ARG> pStorageHolder
                  ,_TY (Func_op) const& operFunctor
                  ,_TY (Func_cap) const& captFunctor
                  ,_TY (Func_undo) const& undoFunctor
                  )
        : do_(operFunctor)
        , undo_(pStorageHolder->tie (undoFunctor, captFunctor))
        , pClo_(pStorageHolder)
        , defaultPatt_(HandlingPattern::defaultID())
        { }
      
#undef _TY
      
      
     ~CommandImpl();
      
      
      /** @internal cloning service for the CommandRegistry:
       *  effectively this is a copy ctor, but as we rely
       *  on a argument holder (without knowing the exact type),
       *  we need to delegate the cloning of the arguments down
       *  to where the exact type info is still available; thus,
       *  a CommandImplCloneBuilder is first passed as a visitor
       *  down and then calls back to perform the copy, providing
       *  an new (clone) closure and UNDO functor already correctly
       *  wired to collaborate.
       *  @see #prepareClone */
      CommandImpl (CommandImpl const& orig
                  ,UndoMutation const& newUndo
                  ,shared_ptr<CmdClosure> const& newClosure)
        : do_{orig.do_}
        , undo_{newUndo}
        , pClo_{newClosure}
        , defaultPatt_{orig.defaultPatt_}
        , cmdID{orig.cmdID}
        { }
      
      explicit operator bool()  const { return isValid(); }
      
      
      /** human-readable marker for diagnostics,
       *  will be (re)assigned when activating this CommandImpl
       */
      Symbol cmdID;
      
      
      /** assist with building a clone copy of this CommandImpl.
       *  By accepting the clone builder as a visitor and dispatching
       *  this visitation down into the concrete closure, the builder
       *  can re-gain the fully typed context available on creation
       *  of the ComandImpl. Within this context, for the clone 
       *  to be created, the UndoMutation has to be re-wired,
       *  otherwise it would continue to cooperate with
       *  original closure.
       */
      void
      prepareClone (CommandImplCloneBuilder& visitor)  const
        {
          ASSERT (pClo_);
          pClo_->accept(visitor);
        }
      
      
    public: /* === implementation of command functionality === */
      
      void
      setArguments (Arguments& args)
        {                                                              //////////////////////////////////////TICKET #1095 : explicit argument arity check here
          pClo_->bindArguments (args);
        }
      
      void
      setArguments (lib::diff::Rec const& paramData)
        {                                                              //////////////////////////////////////TICKET #1095 : explicit argument arity check here
          pClo_->bindArguments (paramData);
        }
      
      void
      discardArguments()
        {
          pClo_->unbindArguments();
        }
      
      void invokeOperation() { do_(*pClo_); }
      void invokeCapture()   { undo_.captureState(*pClo_); }
      void invokeUndo()      { undo_(*pClo_); }
      
      
      
      typedef HandlingPattern::ID PattID;
      
      PattID
      getDefaultHandlingPattern()  const
        {
          return defaultPatt_;
        }
      
      /** define a handling pattern to be used by default
       *  @return ID of the currently defined default pattern */
      PattID
      setHandlingPattern (PattID newID)
        {
          PattID currID = defaultPatt_;
          defaultPatt_ = newID;
          return currID;
        }
      
      
      
      /* === diagnostics === */
      
      bool
      isValid()  const    ///< validity self-check: is basically usable.
        {
          return bool(pClo_) 
             and HandlingPattern::get(defaultPatt_).isValid();
        }
      
      bool
      canExec()  const    ///< state check: sufficiently defined to be invoked 
        {
          return isValid()
             and pClo_->isValid();
        }
      
      bool
      canUndo()  const    ///< state check: has undo state been captured? 
        {
          return isValid() and pClo_->isCaptured();
        }
      
      operator string()  const
        {
          return _Fmt("Cmd|valid:%s, exec:%s, undo:%s |%s")
                     % isValid()
                     % canExec()
                     % canUndo()
                     % (pClo_? string(*pClo_) : util::FAILURE_INDICATOR);
        }
      
      
      
      friend bool
      operator== (CommandImpl const& ci1, CommandImpl const& ci2)
      {
        return (ci1.do_ == ci2.do_)
//         and (ci1.undo_ == ci2.undo_)                     // causes failure regularly, due to the missing equality on boost::function. See Ticket #294
           and (ci1.defaultPatt_ == ci2.defaultPatt_)
           and (ci1.canExec() == ci2.canExec())
           and (ci1.canUndo() == ci2.canUndo())
           and (ci1.pClo_->equals(*ci2.pClo_))
             ;
      }
      
      friend bool
      operator!= (CommandImpl const& ci1, CommandImpl const& ci2)
      {
        return not (ci1==ci2);
      }
    };
  
  
  
  
  
}} // namespace proc::control
#endif
