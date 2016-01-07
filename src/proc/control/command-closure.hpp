/*
  COMMAND-CLOSURE.hpp  -  defining execution targets and parameters for commands

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


/** @file command-closure.hpp
 ** A closure enabling self-contained execution of commands within the ProcDispatcher.
 ** After defining a proc-layer command, at some point the function arguments
 ** of the contained operation are "closed" by storing concrete argument values.
 ** These values will be fed later on to the operation when the command is invoked.
 ** 
 ** Most of the command machinery accesses this function closure through the generic
 ** interface CmdClosure, while, when defining a command, subclasses typed to the specific
 ** function arguments are created. Especially, there is an ArgumentHolder template,
 ** which is used to define the storage for the concrete arguments. This ArgumentHolder
 ** internally contains an Closure<SIG> instance (where SIG is the signature of the
 ** actual command operation function), which implements the invocation of the
 ** operation function with the stored argument tuple.
 ** 
 ** \par Command Closure and Lifecycle
 ** When defining a command, Mutation objects are to be created based on a concrete function.
 ** These are stored embedded into a type erasure container, thus disposing the specific type
 ** information of the function and function arguments. Each command needs an Mutation object
 ** holding the command operation and an UndoMutation holding the undo functor. 
 ** 
 ** Later on, any command needs to be made ready for execution by binding it to a specific
 ** execution environment, which especially includes the target objects to be mutated by the
 ** command. Effectively, this means "closing" the Mutation (and UNDO) functor(s)) with the
 ** actual function arguments. These arguments are stored embedded within an ArgumentHolder,
 ** which thereby acts as closure. Besides, the ArgumentHolder also has to accommodate for
 ** storage holding the captured UNDO state (memento). Internally the ArgumentHolder
 ** has to keep track of the actual types, thus allowing to re-construct the concrete
 ** function signature when closing the Mutation.
 ** 
 ** Finally, when invoking the command, it passes a \c CmdClosure& to the Mutation object,
 ** which allows the embedded function to be called with the concrete arguments. Besides
 ** just invoking it, a command can also be used like a prototype object. To support this
 ** use case it is possible to re-bind to a new set of command arguments, and to create
 ** a clone copy of the argument (holder) without disclosing the actual types involved. 
 ** 
 ** @see Command
 ** @see ProcDispatcher
 ** @see command-argument-holder.hpp
 **
 */



#ifndef CONTROL_COMMAND_CLOSURE_H
#define CONTROL_COMMAND_CLOSURE_H

#include "lib/bool-checkable.hpp"
#include "lib/meta/typelist.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/function-closure.hpp"
#include "lib/meta/function-erasure.hpp"
#include "lib/meta/tuple.hpp"
#include "lib/meta/maybe-compare.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"
#include "proc/control/argument-erasure.hpp"
#include "lib/typed-allocation-manager.hpp"

#include <memory>
#include <functional>
#include <sstream>
#include <string>




namespace proc {
namespace control {
  
  using lib::meta::FunctionSignature;
  using lib::meta::Tuple;
  using lib::meta::BuildTupleAccessor;
  using lib::meta::func::TupleApplicator;
  using lib::meta::FunErasure;
  using lib::meta::StoreFunction;
  using lib::meta::NullType;
  
  using lib::meta::equals_safeInvoke;
  using lib::TypedAllocationManager;
  using util::unConst;
  using std::function;
  using std::ostream;
  using std::string;
  
  
  
  LUMIERA_ERROR_DECLARE (UNBOUND_ARGUMENTS);  ///< Command functor not yet usable, because arguments aren't bound
  
  
  /** 
   * A neutral container internally holding 
   * the functor used to implement the Command
   */
  typedef FunErasure<StoreFunction> CmdFunctor;
  
  class CommandImplCloneBuilder;
  
  
  class CmdClosure;
  typedef std::shared_ptr<CmdClosure> PClo;
  
  
  
  /** Interface */
  class CmdClosure
    : public lib::BoolCheckable<CmdClosure>
    {
    public:
      virtual ~CmdClosure() {}
      
      virtual operator string() const                      =0;
      virtual bool isValid ()   const                      =0;    ///< does this closure hold a valid argument tuple?
      virtual bool isCaptured () const                     =0;    ///< does this closure hold captured UNDO state?
      virtual bool equals (CmdClosure const&)  const       =0;    ///< is equivalent to the given other closure?
      virtual void bindArguments (Arguments&)              =0;    ///< store a set of parameter values within this closure
      virtual void invoke (CmdFunctor const&)              =0;    ///< invoke functor using the stored parameter values
      virtual void accept (CommandImplCloneBuilder&) const =0;    ///< assist with creating clone closure without disclosing concrete type
    };
  
  
  inline ostream& operator<< (ostream& os, const CmdClosure& clo) { return os << string(clo); }
  
  class AbstractClosure
    : public CmdClosure
    {
      bool isValid()    const { return false; }
      bool isCaptured() const { return false; }
      void accept (CommandImplCloneBuilder&) const {}
    };
  
  
  
  
  
  /** Helper for accessing an individual function parameter */
  template
    < typename TY
    , class BASE
    , class TUP
    , uint idx
    >
  class ParamAccessor
    : public BASE
    {
      TY      & element()        { return          BASE::template getAt<idx>(); }
      TY const& element()  const { return unConst(this)->template getAt<idx>(); }
      
    public:
        
      ParamAccessor(TUP const& tuple)
        : BASE(tuple)
        { }
      
      
      ////////////////////TODO the real access operations (e.g. for serialising) go here
      
      /////////////////////////////////////////////////////////////TICKET #798 : we need to pick up arguments from a lib::diff::Record.
      
      
      ostream&
      dump (ostream& output)  const
        {
          return BASE::dump (output << element() << ',');
        }
      
      friend bool
      compare (ParamAccessor const& p1, ParamAccessor const& p2)
        {
          return equals_safeInvoke (p1.element(), p2.element())
              && compare ( static_cast<BASE>(p1)
                         , static_cast<BASE>(p2) );
        }
    };
    
  template<class TUP>
  class ParamAccessor<NullType, TUP, TUP, 0>   ///< used for recursion end of implementation functions
    : public TUP
    {
    public:
      ParamAccessor(TUP const& tuple)
        : TUP(tuple)
        { }
      
      ////////////////////TODO the recursion-end of the access operations goes here
      
      ostream&
      dump (ostream& output)  const
        { 
          return output;
        }
      
      friend bool
      compare (ParamAccessor const&, ParamAccessor const&)
        {
          return true;
        }
    };
  
  
  
  template<typename SIG>
  class Closure
    : public AbstractClosure
    {
      typedef typename FunctionSignature< function<SIG> >::Args Args;
      
      
      typedef BuildTupleAccessor<Args,ParamAccessor> Builder;
      typedef typename Builder::Accessor ParamStorageTuple;
      
      ParamStorageTuple params_;
      
    public:
      typedef Tuple<Args> ArgTuple;
      
      Closure (ArgTuple const& args)
        : params_(Builder (args))
        { }
      
      /** create a clone copy of this, without disclosing the exact type */
      PClo
      createClone (TypedAllocationManager& storageManager)
        {
          return storageManager.create<Closure> (*this);
        }
      
      /** assign a new parameter tuple to this */
      void
      bindArguments (Arguments& args)
      {
        params_ = args.get<ArgTuple>();
      }
      
      
      /** Core operation: use the embedded argument tuple for invoking a functor
       *  @param unboundFunctor an function object, whose function arguments are
       *         required to match the types of the embedded ParamStorageTuple
       *  @note  ASSERTION failure if the function signature 
       *         doesn't match the argument types tuple.
       *  @note  the functor might actually \em modify the param values.
       *         Thus this function can't be const.
       */
      void
      invoke (CmdFunctor const& unboundFunctor)
        {
          TupleApplicator<SIG> apply_this_arguments(params_);
          apply_this_arguments (unboundFunctor.getFun<SIG>());
        }
      
      
      
      operator string()  const
        {
          std::ostringstream buff;
          params_.dump (buff << "Closure(" );
          
          string dumped (buff.str());
          if (8 < dumped.length())
            // remove trailing comma...
            return dumped.substr (0, dumped.length()-1) +")";
          else
            return dumped+")";
        }
      
      
      bool isValid ()   const { return true; }
      
      /// Supporting equality comparisons...
      friend bool operator== (Closure const& c1, Closure const& c2)  { return compare (c1.params_, c2.params_); }
      friend bool operator!= (Closure const& c1, Closure const& c2)  { return not (c1 == c2); }
      
      bool
      equals (CmdClosure const& other)  const
        {
          const Closure* toCompare = dynamic_cast<const Closure*> (&other);
          return (toCompare)
              && (*this == *toCompare);
        }
    };
  
  
  
  
}} // namespace proc::control
#endif
