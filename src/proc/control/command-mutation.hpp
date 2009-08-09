/*
  COMMAND-MUTATION.hpp  -  functor encapsulating the actual operation of proc-Command
 
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


/** @file command-mutation.hpp
 ** Core of a Proc-Layer command: functor containing the operation to be executed.
 ** Each command holds two of these functors: one representing the actual operation
 ** and one to undo the effect of this operation. The latter involves the capturing
 ** and storing of a "memento" value behind the scenes. But towards Command, the
 ** Mutation acts as interface to conceal these details, as well as the actual
 ** type and parameters of the functions to be invoked. Thus, Mutation's
 ** public interface just consists of a function call operator \c void() .
 ** 
 ** \par Lifecycle
 ** Mutation objects are to be created based on a concrete function object, which then
 ** gets embedded into a type erasure container, thus disposing the specific type information.
 ** Moreover, building on this lib::OpaqueHolder yields a fixed size storage for Mutation objects,
 ** allowing them to be embedded immediately within the CommandImpl frame.
 ** 
 ** Later on, any command needs to be made ready for execution by binding it to a specific
 ** execution environment, which especially includes the target objects to be mutated by the
 ** command. This procedure includes "closing" the Mutation (and UNDO) functor(s) with the
 ** actual function arguments. These arguments are stored embedded within an ArgumentHolder,
 ** which thereby acts as closure. Besides, the ArgumentHolder also has to accommodate for
 ** storage holding the captured UNDO state (memento). Thus, internally the ArgumentHolder
 ** has to keep track of the actual types, thus allowing to re-construct the concrete
 ** function signature when closing the Mutation.
 ** 
 ** @see Command
 ** @see ProcDispatcher
 ** @see MementoTie binding memento state
 **
 */



#ifndef CONTROL_COMMAND_MUTATION_H
#define CONTROL_COMMAND_MUTATION_H

//#include "pre.hpp"
#include "lib/error.hpp"
#include "lib/bool-checkable.hpp"
#include "proc/control/command-closure.hpp"
#include "proc/control/memento-tie.hpp"

#include <iostream>
#include <string>



namespace control {
  
  using std::ostream;
  using std::string;
  
  
  
  
  /**
   * Unspecific command functor for implementing Proc-Layer Command.
   * To be created from an tr1::function object, which later on
   * can be \link #close closed \endlink with a set of actual 
   * function arguments. The concrete type of the function
   * and the arguments is concealed (erased) on the interface,
   * while the unclosed/closed - state of the functor can be
   * checked by bool() conversion.
   */
  class Mutation
    : public lib::BoolCheckable<Mutation>
    {
      CmdFunctor func_;
      CmdClosure* clo_;
      
    public:
      template<typename SIG>
      Mutation (function<SIG> const& func)
        : func_(func),
          clo_(0)
        { }
      
      virtual ~Mutation() {}
      
      
      virtual Mutation&
      close (CmdClosure& cmdClosure)
        {
          ///////////////////////////////////////////////////////////////////////////////TODO we need a better approach; this breaks when re-binding
          REQUIRE (!clo_, "Lifecycle error: already closed over the arguments");
          REQUIRE (func_, "Param error: not bound to a valid function");
          func_ = cmdClosure.closeArguments(func_);
          clo_ = &cmdClosure;
          return *this;
        }
      
      
      void
      operator() ()
        {
          if (!clo_)
            throw lumiera::error::State ("Lifecycle error: function arguments not yet provided",
                                         LUMIERA_ERROR_UNBOUND_ARGUMENTS);
          invoke (func_);
        }
      
      
      /** diagnostics */
      operator string()  const
        { 
          return isValid()? "Mutation("+string (*clo_)+")"
                          : "Mutation(untied)";
        }
      
      virtual bool
      isValid ()   const
        {
          return func_ && clo_;
        }
      
      
    private:
      void
      invoke (CmdFunctor & closedFunction)
        {
          closedFunction.getFun<void()>() ();
        }
    };
  
  
  inline ostream& operator<< (ostream& os, Mutation const& muta) { return os << string(muta); }
  
  
  
  
  
  
  
  
  /**
   * Specialised version of the command Mutation functor,
   * used to implement the UNDO functionality. The operation
   * executed when invoking this functor is the UNDO operation
   * of the respective command; additionally we need another
   * functor to capture the state to be restored on UNDO.
   * Both functors are wired up internally to cooperate
   * and store the state (memento), which is implemented
   * by the specifically typed MementoTie object passed
   * in on construction. All these specific details
   * are concealed on the interface
   */
  class UndoMutation
    : public Mutation
    {
      Mutation captureMemento_;
      
    public:
      template<typename SIG, typename MEM>
      UndoMutation (MementoTie<SIG,MEM> & mementoHolder)
        : Mutation (mementoHolder.tieUndoFunc())
        , captureMemento_(mementoHolder.tieCaptureFunc())
        { }
      
      
      virtual Mutation&
      close (CmdClosure& cmdClosure)
        {
          Mutation::close(cmdClosure);
          captureMemento_.close(cmdClosure);
          return *this;
        }
      
      
      Mutation&
      captureState ()
        {
          if (!Mutation::isValid())
            throw lumiera::error::State ("need to bind function arguments prior to capturing undo state",
                                         LUMIERA_ERROR_UNBOUND_ARGUMENTS);
          
          captureMemento_();
          return *this;
        }
      
      
    private:
      virtual bool
      isValid ()  const
        {
          return Mutation::isValid() && captureMemento_;
        }
      
      
    };
  
  
} // namespace control
#endif
