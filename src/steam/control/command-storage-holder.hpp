/*
  COMMAND-STORAGE-HOLDER.hpp  -  specifically typed container for storage of command arguments

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


/** @file command-storage-holder.hpp
 ** A passive container record holding the actual command arguments & UNDO state.
 ** Effectively, this is the top level CmdClosure Implementation, which in turn
 ** delegates to sub-closures for the operation arguments and for UNDO management.
 ** While all command objects themselves have a common type (type erasure),
 ** the actual argument tuple and the state memento for UNDO can't. Especially,
 ** the size of arguments and memento will depend on their respective types.
 ** Thus, to manage somehow the storage of this data, we create a common holder,
 ** which can than be managed by a custom allocator / object pool.
 ** 
 ** @see Command
 ** @see CmdClosure    storage of command arguments
 ** @see MementoTie    wiring of UNDO functions & memento
 ** @see UndoMutation  execution of UNDO
 ** @see command-argument-test.cpp
 **
 */



#ifndef CONTROL_COMMAND_STORAGE_HOLDER_H
#define CONTROL_COMMAND_STORAGE_HOLDER_H

#include "lib/typed-allocation-manager.hpp"
#include "steam/control/command-op-closure.hpp"
#include "steam/control/memento-tie.hpp"
#include "steam/control/command-impl-clone-builder.hpp"
#include "lib/opaque-holder.hpp"

#include <string>



namespace steam {
namespace control {
  
  using lib::InPlaceBuffer;
  using std::string;
  
  
  
  
  
  /**
   * This is "the" top level CmdClosure implementation.
   * It is a specifically typed CmdClosure, which serves for
   * actually allocating storage to hold the command arguments
   * and the UNDO state (memento) for Steam-Layer commands.
   * Both the contained components within StorageHolder
   * can be in \em empty state; there are no distinct
   * lifecycle limitations. StorageHolder is part
   * of Steam-Layer command's implementation
   * and should not be used standalone.
   */
  template<typename SIG, typename MEM>
  class StorageHolder
    : public CmdClosure
    {
      using ArgHolder = OpClosure<SIG>;
      using MemHolder = MementoTie<SIG,MEM>;
      
      using ArgumentBuff = InPlaceBuffer<ArgHolder>;
      using  MementoBuff = InPlaceBuffer<MemHolder>;
      
      using ArgTuple = typename ArgHolder::ArgTuple;
      using Args     = typename lib::meta::RebindTySeq<ArgTuple>::Seq; // std::tuple<ARGS...> to Types<ARGS...>
      
      
      /* ====== in-place storage buffers ====== */
      
      ArgumentBuff arguments_;
      MementoBuff  memento_;
      
      
      
      /* ==== proxied CmdClosure interface ==== */
      
    public:
      virtual bool
      isValid ()  const override
        {
          return arguments_->isValid();
        }
      
      virtual bool
      isCaptured() const override
        {
          return memento_->isValid();
        }

      
      
      /** assign a new parameter tuple to this */
      virtual void
      bindArguments (Arguments& args)  override
        {
          storeTuple (args.get<ArgTuple>());
        }
      
      /** assign a new set of parameter values to this.
       * @note the values are passed packaged into a sequence
       *       of GenNode elements. This is the usual way
       *       arguments are passed from the UI-Bus
       */
      virtual void
      bindArguments (lib::diff::Rec const&  paramData)  override
        {
          storeTuple (buildTuple<Args> (paramData));
        }
      
      /** discard any argument data and return to _empty_ state */
      virtual void
      unbindArguments()  override
        {
          clearStorage();
        }
      
      
      virtual void
      invoke (CmdFunctor const& func)  override
        {
          if (!isValid())
            throw lumiera::error::State ("Lifecycle error: can't bind functor, "
                                         "command arguments not yet provided",
                                         LERR_(UNBOUND_ARGUMENTS));
          
          arguments_->invoke(func);
        }
      
      
      virtual
      operator string()  const override
        {
          return "Command-State{ arguments="
               + (arguments_->isValid()? string(*arguments_) : "unbound")
               + ", "+string(*memento_)+"}"
               ;
        }
      
      
      
      /** per default, all data within StorageHolder
       *  is set up in \em empty state. Later on, the
       *  command arguments are to be provided by #bind ,
       *  whereas the undo functions will be wired by #tie
       */
      StorageHolder ()
        : arguments_()
        , memento_()
        { }
      
      /** copy construction allowed(but no assignment).
       * @remarks rationale is to support immutable argument values,
       *          which means default/copy construction is OK
       */
      StorageHolder (StorageHolder const& oAh)
        : arguments_()
        , memento_()
        {
          if (oAh.arguments_->isValid())  // don't clone garbage from invalid arguments 
            arguments_.template create<ArgHolder> (*oAh.arguments_);
          
          // memento can be cloned as-is, irrespective of activation state 
          memento_.template  create<MemHolder> (*oAh.memento_);
        }
      
      
      /** copy construction allowed(but no assignment)*/
      StorageHolder& operator= (StorageHolder const&)  = delete;
      
      
      
      /** assist with creating a clone copy;
       *  this results in invocation of the copy ctor */
      void
      accept (CommandImplCloneBuilder& visitor)  const override
        {
          visitor.buildCloneContext (*this);
        }
      
      
      /** has undo state capturing been invoked? */
      bool canUndo () const { return memento_->isValid();   }
      bool empty ()   const { return !arguments_->isValid(); }
      
      
      /** store a new argument tuple within this StorageHolder,
       *  discarding any previously stored arguments */
      void
      storeTuple (ArgTuple const& argTup)
        {
          arguments_.template create<ArgHolder> (argTup);
        }
      
      void
      clearStorage ()
        {
          arguments_.template create<ArgHolder>();
          memento_->clear();
        }
      
      
      typedef typename CommandSignature<SIG,MEM>::OperateSig SIG_op;
      typedef typename CommandSignature<SIG,MEM>::CaptureSig SIG_cap;
      typedef typename CommandSignature<SIG,MEM>::UndoOp_Sig SIG_undo;
      
      /** create a new memento storage wiring, discarding existing memento state.
       *  @note any bound undo/capture functions based on the previously held MementoTie
       *        are silently invalidated; using them will likely cause memory corruption! */
      MementoTie<SIG,MEM>&
      tie (function<SIG_undo> const& undoFunc,
           function<SIG_cap> const& captureFunc)
        {
          return memento_.template create<MemHolder> (undoFunc,captureFunc);
        }
      
      /** just re-access an existing memento storage wiring.
       *  Used when cloning the closure */
      MementoTie<SIG,MEM>&
      getMementoWiring ()
        {
          return *memento_;
        }
      
      
      
      /** direct "backdoor" access to stored memento value.
       *  @throw LUMIERA_ERROR_MISSING_MEMENTO when invoked
       *         prior to \c tie(..) and capturing any state */
      MEM&
      memento ()
        {
          return memento_->getState();
        }
    };
    
  
  
}} // namespace steam::control
#endif /*CONTROL_COMMAND_STORAGE_HOLDER_H*/
