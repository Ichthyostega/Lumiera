/*
  COMMAND-IMPL-CLONE-BUILDER.hpp  -  Cloning command implementation without disclosing concrete type
 
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


/** @file command-impl-clone-builder.hpp
 ** Helper for creating an implementation clone, based on the visitor pattern.
 ** This file deals with the problem of creating a clone from top level without
 ** any specific type information. While generally this means passing down the
 ** allocation interface, the specific problem here is that multiple parts of the
 ** command implementation need to be cloned and re-wired with the cloned partners,
 ** which requires re-creating the specifically typed context used at initial setup.
 ** 
 ** Ticket #301 : it may well be that the need for such a facility is a symptom of
 ** misaligned design, but I rather doubt so -- because both the memento holder and
 ** the command closure need a specifically typed context, and there is no reason
 ** for combining them into a single facility. 
 ** 
 ** @see CommandRegistry#createCloneImpl
 ** @see CommandImpl
 ** @see ArgumentHolder#createClone
 ** @see command-clone-builder-test.cpp
 **
 */



#ifndef CONTROL_COMMAND_IMPL_CLONE_BUILDER_H
#define CONTROL_COMMAND_IMPL_CLONE_BUILDER_H

//#include "proc/control/command.hpp"
//#include "proc/control/command-closure.hpp"
#include "proc/control/command-mutation.hpp"
#include "lib/typed-allocation-manager.hpp"
//#include "lib/bool-checkable.hpp"

#include <boost/noncopyable.hpp>
//#include <boost/operators.hpp>

#include <tr1/memory>
//#include <tr1/functional>


namespace control {
  
  using lib::TypedAllocationManager;
//  using std::tr1::function;
  using std::tr1::shared_ptr;
  
  
  
  /**
   * Visitor to support creating a CommandImpl clone.
   * Created and managed by CommandRegistry, on clone creation
   * an instance of this builder object is passed down to re-gain
   * a fully typed context, necessary for re-wiring the undo functors
   * and the memento storage within the cloned parts.
   */
  class CommandImplCloneBuilder
//    : public lib::BoolCheckable<CommandImpl
//           , boost::noncopyable
//           >
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
      CommandImpl (shared_ptr<ARG> pArgHolder
                  ,_TY (Func_op) const& operFunctor
                  ,_TY (Func_cap) const& captFunctor
                  ,_TY (Func_undo) const& undoFunctor
                  )
        : do_(operFunctor)
        , undo_(pArgHolder->tie (undoFunctor, captFunctor))
        , pClo_(pArgHolder)
        , defaultPatt_(HandlingPattern::defaultID())
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
        , defaultPatt_(orig.defaultPatt_)
        { }
      
      
      void
      setArguments (Arguments& args)
        {
          pClo_->bindArguments(args);
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
              && HandlingPattern::get(defaultPatt_).isValid();
        }
      
      bool
      canExec()  const    ///< state check: sufficiently defined to be invoked 
        {
          return isValid()
              && pClo_->isValid();
        }
      
      bool
      canUndo()  const    ///< state check: has undo state been captured? 
        {
          return isValid() && pClo_->isCaptured();
        }
      
      
      
      friend bool
      operator== (CommandImpl const& ci1, CommandImpl const& ci2)
      {
        return (ci1.do_ == ci2.do_)
//          && (ci1.undo_ == ci2.undo_)                     // causes failure regularly, due to the missing equality on boost::function. See Ticket #294
            && (ci1.defaultPatt_ == ci2.defaultPatt_)
            && (ci1.canExec() == ci2.canExec())
            && (ci1.canUndo() == ci2.canUndo())
            && (ci1.pClo_->equals(*ci2.pClo_))
             ;
      }
      
      friend bool
      operator!= (CommandImpl const& ci1, CommandImpl const& ci2)
      {
        return !(ci1==ci2);
      }
    };
  
  
  
  
  
} // namespace control
#endif
