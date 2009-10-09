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
    : public boost::noncopyable
    {
      TypedAllocationManager& allocator_;
      shared_ptr<CmdClosure> newClo_;
      
      
      
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
      CommandImplCloneBuilder (TypedAllocationManager allo)
        : allocator_(allo)
        , newClo_()
        { }
      
      
      /** visit the CommandImpl given as reference 
       *  to re-gain the contained Closure type context
       */
      void
      visit (CommandImpl const& sourceImpl)
        {
          UNIMPLEMENTED ("get access to source context");
        }
      
      
      /** to be executed from within the specifically typed context
       *  of a concrete command ArgumentHolder; prepare the objects
       *  necessary to re-build a "clone" of the UNDO-Functor.
       */
      template<typename ARG>
      void
      buildCloneContext (shared_ptr<ARG> pArgHolder)
        {
          UNIMPLEMENTED ("how to reflect context back");
          newClo_ = pArgHolder; 
        }
      
#undef _TY
      
      
      /** after visitation: use pre-built bits to provide a cloned UndoFunctor */
      UndoMutation const&
      clonedUndoMutation ()
        {
          UNIMPLEMENTED (" getNewUndoMutation ()" );
        }
      
      
      /** after visitation: provide cloned ArgumentHolder,
       *  but already stripped down to the generic usage type */
      shared_ptr<CmdClosure> const&
      clonedClosuere ()
        {
          REQUIRE (newClo_);
          return newClo_;
        }
      
    };
  
  
  
  
  
} // namespace control
#endif
