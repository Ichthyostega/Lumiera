/*
  ARGUMENT-ERASURE.hpp  -  helper for passing on a dynamic typed argument tuple

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file argument-erasure.hpp
 ** Implementation helper to bind Steam-Layer commands with arbitrary argument tuples
 ** @internal details of the [command handling framework](\ref command.hpp).
 */



#ifndef CONTROL_ARGUMENT_ERASURE_H
#define CONTROL_ARGUMENT_ERASURE_H

#include "lib/error.hpp"


namespace lumiera {
namespace error   {
  LUMIERA_ERROR_DECLARE (INVALID_ARGUMENTS);  ///< Arguments provided for binding doesn't match stored command function parameters
  LUMIERA_ERROR_DECLARE (UNBOUND_ARGUMENTS);  ///< Command functor not yet usable, because arguments aren't bound
}}

namespace steam   {
namespace control {
  namespace err = lumiera::error;
  
  
  /**
   * Adapter interface for invoking an argument binding for a command
   * \em without the need to disclose the concrete types and number of arguments.
   * At the receiver side, the concrete type can be restored by a dynamic cast.
   * This requires the receiver somehow to know the desired argument types;
   * usually this receiver will be an implementation object, whose exact
   * type has been erased after definition, while the implementation
   * internally of course knows the type and thus can perform an
   * dynamic cast on the passed on argument tuple
   */
  struct Arguments;
  
  template<typename TUP>
  struct TypedArguments;
  
  
  struct Arguments
    {
      virtual ~Arguments() {}
     
      template<typename TUP>
      TUP const&
      get()
        {
          TypedArguments<TUP>* dest = dynamic_cast<TypedArguments<TUP>*> (this);
          if (not dest)
            throw err::Invalid{"Wrong type or number of arguments"
                              , LERR_(INVALID_ARGUMENTS)};
          return dest->args_;
        }
    };
  
  
  template<typename TUP>
  struct TypedArguments
    : Arguments
    {
      TUP const& args_;
      
      TypedArguments (TUP const& a)
        : args_(a)
        { }
    };
  
  
  
}} // namespace steam::control
#endif
