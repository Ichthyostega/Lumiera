/*
  COMMAND-MUTATION.hpp  -  functor encapsulating the actual operation of steam-Command

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


/** @file command-mutation.hpp
 ** Core of a Steam-Layer command: functor containing the operation to be executed.
 ** Each command holds two of these functors: one representing the actual operation
 ** and one to undo the effect of this operation. The latter involves the capturing
 ** and storing of a "memento" value behind the scenes. But towards Command, the
 ** Mutation acts as interface to conceal these details, as well as the actual
 ** type and parameters of the functions to be invoked. Thus, Mutation's
 ** public interface just consists of a function call operator.
 ** 
 ** Mutation objects are based on lib::OpaqueHolder, which yields a fixed size storage,
 ** allowing them e.g. to be embedded immediately within the CommandImpl frame.
 ** 
 ** 
 ** @see Command
 ** @see SteamDispatcher
 ** @see MementoTie binding memento state
 **
 */



#ifndef CONTROL_COMMAND_MUTATION_H
#define CONTROL_COMMAND_MUTATION_H

#include "lib/error.hpp"
#include "steam/control/command-closure.hpp"
#include "steam/control/memento-tie.hpp"



namespace steam {
namespace control {
  
  
  
  /**
   * Unspecific command functor for implementing Steam-Layer Command.
   * To be created from an std::function object, which later on gets
   * any necessary arguments from a closure passed in on invocation.
   * The concrete type of the function  and the arguments is
   * concealed (erased) on the interface.
   */
  class Mutation
    {
      const CmdFunctor func_;
      
    public:
      template<typename SIG>
      Mutation (function<SIG> const& func)
        : func_(func)
        { }
      
      
      
      void
      operator() (CmdClosure& clo)
        {
          if (!clo)
            throw lumiera::error::State ("Lifecycle error: function arguments not ready",
                                         LERR_(UNBOUND_ARGUMENTS));
          clo.invoke (func_);
        }
      
      
      
      /// Supporting equality comparisons...
      friend bool
      operator== (Mutation const& m1, Mutation const& m2)
        {
          return (m1.func_ == m2.func_);
        }
      
      friend bool
      operator!= (Mutation const& m1, Mutation const& m2)
        {
          return not (m1==m2);
        }
    };
  
  
  
  
  
  
  
  
  
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
   * are concealed (erased) on the interface
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
      
      
      Mutation&
      captureState (CmdClosure& clo)
        {
          if (!clo)
            throw lumiera::error::State ("need additional function arguments to be able to capture UNDO state",
                                         LERR_(UNBOUND_ARGUMENTS));
          
          captureMemento_(clo);
          return *this;
        }
      
      
    };
  
  
}} // namespace steam::control
#endif
