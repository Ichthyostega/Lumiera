/*
  COMMAND-ARGUMENT-HOLDER.hpp  -  specifically typed container for storage of command arguments

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


/** @file command-argument-holder.hpp
 ** A passive container record holding the actual command arguments & UNDO state.
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



#ifndef CONTROL_COMMAND_ARGUMENT_HOLDER_H
#define CONTROL_COMMAND_ARGUMENT_HOLDER_H

#include "lib/typed-allocation-manager.hpp"
#include "proc/control/command-closure.hpp"
#include "proc/control/memento-tie.hpp"
#include "proc/control/command-impl-clone-builder.hpp"
#include "lib/opaque-holder.hpp"

#include <string>



namespace proc {
namespace control {
  
  using lib::InPlaceBuffer;
  using std::string;
  
  
  namespace { // empty state marker objects for ArgumentHolder
    
    template<typename SIG>
    struct MissingArguments
      : Closure<SIG>
      {
        typedef typename Closure<SIG>::ArgTuple ArgTuple;
        
        MissingArguments ()
          : Closure<SIG> (ArgTuple ())
          { }
        
      private:
        virtual bool isValid ()  const { return false; }
      };
    
    
    template<typename SIG, typename MEM>
    struct UntiedMemento
      : MementoTie<SIG,MEM>
      {
        typedef typename CommandSignature<SIG,MEM>::CaptureSig SIG_cap;
        typedef typename CommandSignature<SIG,MEM>::UndoOp_Sig SIG_undo;
        
        UntiedMemento()
          : MementoTie<SIG,MEM> (function<SIG_undo>(), function<SIG_cap>() )
          { }
      };
  
  } // (END) impl details / empty state marker objects
  
  
  
  
  /**
   * Specifically typed CmdClosure, which serves for
   * actually allocating storage to hold the command arguments
   * and the UNDO state (memento) for Proc-Layer commands.
   * Both the contained components within ArgumentHolder
   * can be in \em empty state; there are no distinct
   * lifecycle limitations. ArgumentHolder is part
   * of Proc-Layer command's implementation
   * and should not be used standalone.
   */
  template<typename SIG, typename MEM>
  class ArgumentHolder
    : public AbstractClosure
    {
      /** copy construction allowed(but no assignment)*/
      ArgumentHolder& operator= (ArgumentHolder const&);
      
      
      typedef Closure<SIG>        ArgHolder;
      typedef MementoTie<SIG,MEM> MemHolder;
      
      typedef InPlaceBuffer<ArgHolder, sizeof(ArgHolder), MissingArguments<SIG> > ArgumentBuff;
      typedef InPlaceBuffer<MemHolder, sizeof(MemHolder), UntiedMemento<SIG,MEM> > MementoBuff;
      
      typedef typename ArgHolder::ArgTuple ArgTuple;
      
      
      /* ====== in-place storage buffers ====== */
      
      ArgumentBuff arguments_;
      MementoBuff  memento_;
      
      
      
      /* ==== proxied CmdClosure interface ==== */
      
    public:
      virtual bool isValid ()  const
        {
          return arguments_->isValid();
        }
      
      virtual bool isCaptured() const
        {
          return memento_->isValid();
        }

      
      
      /** assign a new parameter tuple to this */
      virtual void bindArguments (Arguments& args)
      {
        if (!arguments_->isValid())
          storeTuple (args.get<ArgTuple>());
        else
          arguments_->bindArguments(args);
      }
      
      
      virtual void invoke (CmdFunctor const& func)
        {
          if (!isValid())
            throw lumiera::error::State ("Lifecycle error: can't bind functor, "
                                         "command arguments not yet provided",
                                         LUMIERA_ERROR_UNBOUND_ARGUMENTS);
          
          arguments_->invoke(func);
        }
      
      
      virtual operator string()  const
        {
          return "Command-State{ arguments="
               + (*arguments_? string(*arguments_) : "unbound")
               + ", "+string(*memento_)+"}"
               ;
        }
      
      
      
      /** per default, all data within ArgumentHolder
       *  is set up in \em empty state. Later on, the
       *  command arguments are to be provided by #bind ,
       *  whereas the undo functions will be wired by #tie
       */
      ArgumentHolder ()
        : arguments_()
        , memento_()
        { }
      
      /** copy construction allowed(but no assignment) */
      ArgumentHolder (ArgumentHolder const& oAh)
        : arguments_()
        , memento_()
        {
          if (oAh.arguments_->isValid())  // don't clone garbage from invalid arguments 
            arguments_.template create<ArgHolder> (*oAh.arguments_);
          
          // memento can be cloned as-is, irrespective of activation state 
          memento_.template  create<MemHolder> (*oAh.memento_);
        }
      
      /** assist with creating a clone copy;
       *  this results in invocation of the copy ctor */
      void
      accept (CommandImplCloneBuilder& visitor)  const
        {
          visitor.buildCloneContext (*this);
        }
      
      
      /** has undo state capturing been invoked? */
      bool canUndo () const { return memento_->isValid();   }
      bool empty ()   const { return !arguments_->isValid(); }
      
      
      /** store a new argument tuple within this ArgumentHolder,
       *  discarding any previously stored arguments */
      void
      storeTuple (ArgTuple const& argTup)
        {
          arguments_.template create<ArgHolder> (argTup);
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
      
      bool
      equals (CmdClosure const& other)  const
        {
          const ArgumentHolder* toCompare = dynamic_cast<const ArgumentHolder*> (&other);
          return (toCompare)
              && (*this == *toCompare);
        }
      
      /// Supporting equality comparisons...
      friend bool
      operator== (ArgumentHolder const& a1, ArgumentHolder const& a2)
        {
          return (a1.arguments_->isValid() == a2.arguments_->isValid())
              && (*a1.arguments_ == *a2.arguments_)
              && (a1.memento_->isValid() == a2.memento_->isValid())
              && (*a1.memento_ == *a2.memento_)
               ;
        }
      
      friend bool
      operator!= (ArgumentHolder const& a1, ArgumentHolder const& a2)
        {
          return not (a1 == a2);
        }
    };
    
  
  
}} // namespace proc::control
#endif
