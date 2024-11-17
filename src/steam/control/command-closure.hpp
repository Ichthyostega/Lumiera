/*
  COMMAND-CLOSURE.hpp  -  defining execution targets and parameters for commands

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file command-closure.hpp
 ** A closure enabling self-contained execution of commands within the SteamDispatcher.
 ** After defining a steam-layer command, at some point the function arguments
 ** of the contained operation are "closed" by storing concrete argument values.
 ** These values will be fed later on to the operation when the command is invoked.
 ** 
 ** Most of the command machinery accesses this function closure through the generic
 ** interface CmdClosure, while, when defining a command, subclasses typed to the specific
 ** function arguments are created. Especially, there is an StorageHolder template,
 ** which is used to define the storage for the concrete arguments. This StorageHolder
 ** internally contains an OpClosure<SIG> instance (where SIG is the signature of the
 ** actual command operation function), which implements the invocation of the
 ** operation function with the stored argument tuple.
 ** 
 ** # Command Closure and Lifecycle
 ** When defining a command, Mutation objects are to be created based on a concrete function.
 ** These are stored embedded into a type erasure container, thus disposing the specific type
 ** information of the function and function arguments. Each command needs an Mutation object
 ** holding the command operation and an UndoMutation holding the undo functor. 
 ** 
 ** Later on, any command needs to be made ready for execution by binding it to a specific
 ** execution environment, which especially includes the target objects to be mutated by the
 ** command. Effectively, this means "closing" the Mutation (and UNDO) functor(s)) with the
 ** actual function arguments. These arguments are stored embedded within an StorageHolder,
 ** which thereby acts as closure. Besides, the StorageHolder also has to accommodate for
 ** storage holding the captured UNDO state (memento). Internally the StorageHolder
 ** has to keep track of the actual types, thus allowing to re-construct the concrete
 ** function signature when closing the Mutation.
 ** 
 ** Finally, when invoking the command, it passes a `CmdClosure&` to the Mutation object,
 ** which allows the embedded function to be called with the concrete arguments. Besides
 ** just invoking it, a command can also be used like a prototype object. To support this
 ** use case it is possible to re-bind to a new set of command arguments, and to create
 ** a clone copy of the argument (holder) without disclosing the actual types involved. 
 ** 
 ** @see Command
 ** @see SteamDispatcher
 ** @see command-storage-holder.hpp
 ** @see command-op-closure.hpp
 **
 */



#ifndef CONTROL_COMMAND_CLOSURE_H
#define CONTROL_COMMAND_CLOSURE_H

#include "lib/meta/function-erasure.hpp"
#include "steam/control/argument-erasure.hpp"
#include "lib/diff/gen-node.hpp"

#include <memory>
#include <string>


namespace steam {
namespace control {
  
  using lib::meta::FunErasure;
  using lib::meta::StoreFunction;
  
  using std::string;
  
  
  
  /** 
   * A neutral container internally holding 
   * the functor used to implement the Command
   */
  typedef FunErasure<StoreFunction> CmdFunctor;
  
  class CommandImplCloneBuilder;
  
  
  
  
  /** Interface */
  class CmdClosure
    {
    public:
      virtual ~CmdClosure() {}
      explicit operator bool()  const { return isValid(); }
      
      virtual operator string() const                      =0;
      virtual bool isValid ()   const                      =0;    ///< does this closure hold a valid argument tuple?
      virtual bool isCaptured () const                     =0;    ///< does this closure hold captured UNDO state?
      virtual void bindArguments (Arguments&)              =0;    ///< store a set of parameter values within this closure
      virtual void bindArguments (lib::diff::Rec const&)   =0;    ///< store a set of parameter values, passed as GenNode sequence
      virtual void unbindArguments()                       =0;    ///< discard any parameters and return to _unbound state_        
      virtual void invoke (CmdFunctor const&)              =0;    ///< invoke functor using the stored parameter values
      virtual void accept (CommandImplCloneBuilder&) const =0;    ///< assist with creating clone closure without disclosing concrete type
    };
  
  
  using PClo = std::shared_ptr<CmdClosure>;
  
  
  
}} // namespace steam::control
#endif /*CONTROL_COMMAND_CLOSURE_H*/
