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
 ** @todo 4/2026 this header seems to be more of a concept draft from the early days.
 **       At that time, the C++ language did not have much function tools, did not
 **       support lambdas and tuples, and thus invoking a functor object and passing
 **       an arbitrary sequence of stored arguments was an innovative concept to be
 **       implemented from scratch. And during that development process, some ideas
 **       regarding the specific handling of relevant argument types was intermingled
 **       with the creation of generic general purpose tooling. However, with the
 **       switch to C++23, in summer 2025, most of that old implementation was
 **       removed and replaced by standard library facilities. The #activated_
 **       flag seems to be the only piece of actual functionality here.
 ** @see Command
 ** @see command-closure.hpp
 ** @see command-storage-holder.hpp
 **
 */



#ifndef CONTROL_COMMAND_OP_CLOSURE_H
#define CONTROL_COMMAND_OP_CLOSURE_H

#include "lib/meta/function.hpp"
#include "steam/control/command-closure.hpp"
#include "steam/control/argument-erasure.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/nocopy.hpp"

#include <sstream>
#include <string>


namespace steam {
namespace control {
  
  using lib::meta::_Fun;
  using lib::meta::Tuple;
  
  using std::string;
  
  
  /**
   * closure to deal with the actual command operation.
   * This includes holding the invocation parameter tuple
   */
  template<typename SIG>
  class OpClosure
    : util::NonAssign
    {
      using Args = _Fun<SIG>::Args;
      
      Tuple<Args> params_;
      bool activated_;
      
    public:
      using ArgTuple = Tuple<Args>;
      
      
      OpClosure()
        : params_{}
        , activated_{false}
        { }
      
      explicit
      OpClosure (ArgTuple const& args)
        : params_(args)
        , activated_(true)
        { }
      
      bool
      isValid ()  const
        {
          return activated_;
        }
      
      
      /** Core operation: use the embedded argument tuple for invoking a functor
       *  @param unboundFunctor an function object, whose function arguments are
       *         required to match the types of the embedded ParamStorageTuple
       *  @note  the functor might actually _modify the param values._
       *         Thus this function can't be const.
       */
      void
      invoke (CmdFunctor const& unboundFunctor)
        {
          std::apply (unboundFunctor.getFun<SIG>(), params_);
        }
      
      
      operator string()  const
        {
          std::ostringstream buff;
          buff << "OpClosure";
          lib::meta::joinTupleParen (params_, buff);
          return buff.str();
        }
    };
  
  
}} // namespace steam::control
#endif /*CONTROL_COMMAND_OP_CLOSURE_H*/
