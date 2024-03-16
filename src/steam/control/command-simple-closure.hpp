/*
  COMMAND-SIMPLE-CLOSURE.hpp  -  demo implementation of command closure

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file command-simple-closure.hpp
 ** Proof-of-concept implementation of CmdClosure.
 ** This is used for test only, to invoke an arbitrary matching functor with
 ** arguments stored embedded within this closure. In the real system, a more
 ** [elaborate version](\ref StorageHolder) of the same concept is used, with
 ** the additional complication of managing the UNDO operation as well.
 ** 
 ** \par historical note
 ** This proof-of-concept variation was split off in an attempt to improve the
 ** overall design of the command / closure system. The original design had the
 ** embedded argument holder also implement the CmdClosure interface, which is
 ** a clever implementation and code-reuse trick, but otherwise caused confusion. 
 ** 
 ** @see Ticket #301
 ** @see CommandMutation_test
 ** @see StorageHolder
 ** @see OpClosure
 **
 */



#ifndef CONTROL_COMMAND_SIMPLE_CLOSURE_H
#define CONTROL_COMMAND_SIMPLE_CLOSURE_H

#include "lib/typed-allocation-manager.hpp"
#include "steam/control/command-op-closure.hpp"
#include "lib/opaque-holder.hpp"

#include <string>



namespace steam {
namespace control {
  namespace err = lumiera::error;
  
  using lib::InPlaceBuffer;
  using std::string;
  
  
  
  
  
  /**
   * Dummy / proof-of-concept implementation of CmdClosure.
   * It is a specifically typed subclass, which serves to hold
   * storage for the concrete invocation arguments within an
   * inline buffer.
   * @note for demonstration and unit testing
   * @see StorageHolder real world implementation
   */
  template<typename SIG>
  class SimpleClosure
    : public CmdClosure
    {
      using ArgHolder = OpClosure<SIG>;
      using ArgumentBuff = InPlaceBuffer<ArgHolder>;
      
      using ArgTuple = typename ArgHolder::ArgTuple;
      using Args     = typename lib::meta::RebindTupleTypes<ArgTuple>::Seq; // std::tuple<ARGS...> to Types<ARGS...>
      
      
      /* ====== in-place argument storage ====== */
      
      ArgumentBuff arguments_;
      
      
      
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
          return false;
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
            throw err::State{"Lifecycle error: can't bind functor, "
                             "command arguments not yet provided"
                            , LERR_(UNBOUND_ARGUMENTS)};
          
          arguments_->invoke(func);
        }
      
      
      virtual
      operator string()  const override
        {
          return "Command-Closure{ arguments="
               + (arguments_->isValid()? string(*arguments_) : "unbound")
               + " }"
               ;
        }
      
      
      
      /** per default, all data within StorageHolder
       *  is set up in \em empty state. Later on, the
       *  command arguments are to be provided by #bind ,
       *  whereas the undo functions will be wired by #tie
       */
      SimpleClosure ()
        : arguments_()
        { }
      
      explicit
      SimpleClosure (ArgTuple const& args)
        : arguments_()
        {
          storeTuple (args);
        }
      
      SimpleClosure (SimpleClosure const& oAh)
        : arguments_()
        {
          if (oAh.arguments_->isValid())  // don't clone garbage from invalid arguments 
            arguments_.template create<ArgHolder> (*oAh.arguments_);
        }
      
      
      void
      accept (CommandImplCloneBuilder&)  const override
        {
          NOTREACHED();
        }
      
      
      /** has undo state capturing been invoked? */
      bool canUndo () const { return false;                  }
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
        }
    };
    
  
  
}} // namespace steam::control
#endif /*CONTROL_COMMAND_SIMPLE_CLOSURE_H*/
