/*
  COMMAND-OP-CLOSURE.hpp  -  implementation the closure for a command operation

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file command-op-closure.hpp
 ** Implementation of the concrete (sub)-closure of a command, responsible for
 ** invoking the actual command operation with the concrete (binding) arguments. 
 ** 
 ** @see Command
 ** @see command-closure.hpp
 ** @see command-storage-holder.hpp
 **
 */



#ifndef CONTROL_COMMAND_OP_CLOSURE_H
#define CONTROL_COMMAND_OP_CLOSURE_H

#include "lib/meta/function.hpp"
#include "lib/meta/function-closure.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/meta/tuple-record-init.hpp"
#include "lib/meta/maybe-compare.hpp"
#include "steam/control/command-closure.hpp"
#include "steam/control/argument-erasure.hpp"
#include "lib/typed-allocation-manager.hpp"
#include "lib/format-cout.hpp"

#include <memory>
#include <functional>
#include <sstream>
#include <string>




namespace steam {
namespace control {
  
  using lib::meta::_Fun;
  using lib::meta::Tuple;
  using lib::meta::BuildTupleAccessor;
  using lib::meta::func::TupleApplicator;
  using lib::meta::NullType;
  using lib::meta::buildTuple;
  
  using lib::TypedAllocationManager;
  using std::function;
  using std::ostream;
  using std::string;
  
  
  
  
  
  
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
      TY      & element()        { return std::get<idx> (*this); }
      TY const& element()  const { return std::get<idx> (*this); }
      
    public:
      using BASE::BASE;
      
      
      ////////////////////TODO the real access operations (e.g. for serialising) go here
      
      /////////////////////////////////////////////////////////////TICKET #798 : we need to pick up arguments from a lib::diff::Record.
      
      ostream&
      dump (ostream& output)  const
        {
          return BASE::dump (output << element() << ',');
        }
    };
    
  template<class TUP, uint n>
  class ParamAccessor<NullType, TUP, TUP, n>   ///< used for recursion end of implementation functions
    : public TUP
    {
    public:
      ParamAccessor (TUP const& tup)
        : TUP(tup)
        { }
      
      /** we deliberately support immutable types as command arguments */
      ParamAccessor& operator= (ParamAccessor const&)  =delete;
      ParamAccessor& operator= (ParamAccessor&&)       =delete;
      ParamAccessor (ParamAccessor const&)             =default;
      ParamAccessor (ParamAccessor&&)                  =default;
      
      
      ////////////////////TODO the recursion-end of the access operations goes here
      
      ostream&
      dump (ostream& output)  const
        { 
          return output;
        }
    };
  
  
  
  /**
   * closure to deal with the actual command operation.
   * This includes holding the invocation parameter tuple
   */
  template<typename SIG>
  class OpClosure
    {
      using Args    = typename _Fun<SIG>::Args;
      using Builder = BuildTupleAccessor<ParamAccessor, Args>;
      
      using ParamStorageTuple = typename Builder::Product;
      
      ParamStorageTuple params_;
      bool activated_;
      
    public:
      using ArgTuple = Tuple<Args>;
      
      
      OpClosure()
        : params_(Tuple<Args>())
        , activated_(false)
        { }
      
      explicit
      OpClosure (ArgTuple const& args)
        : params_(args)
        , activated_(true)
        { }
      
      /** we deliberately support immutable types as command arguments */
      OpClosure& operator= (OpClosure const&)  =delete;
      OpClosure& operator= (OpClosure&&)       =delete;
      OpClosure (OpClosure const&)             =default;
      OpClosure (OpClosure&&)                  =default;
      
      
      bool
      isValid ()  const
        {
          return activated_;
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
          params_.dump (buff << "OpClosure(" );
          
          string dumped (buff.str());
          if (10 < dumped.length())
            // remove trailing comma...
            return dumped.substr (0, dumped.length()-1) +")";
          else
            return dumped+")";
        }
    };
  
  
  
  
}} // namespace steam::control
#endif /*CONTROL_COMMAND_OP_CLOSURE_H*/
