/*
  ARGUMENT-ERASURE.hpp  -  helper for passing on a dynamic typed argument tuple

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


/** @file argument-erasure.hpp
 ** Implementation helper to bind Steam-Layer commands with arbitrary argument tuples
 ** @internal details of the [command handling framework](\ref command.hpp).
 */



#ifndef CONTROL_ARGUMENT_ERASURE_H
#define CONTROL_ARGUMENT_ERASURE_H

#include "lib/error.hpp"



namespace steam {
namespace control {
  
  
  LUMIERA_ERROR_DECLARE (INVALID_ARGUMENTS);  ///< Arguments provided for binding doesn't match stored command function parameters
  
  
  
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
      get ()
        {
          TypedArguments<TUP>* dest = dynamic_cast<TypedArguments<TUP>*> (this);
          if (!dest)
            throw lumiera::error::Invalid("Wrong type or number of arguments"
                                         , LUMIERA_ERROR_INVALID_ARGUMENTS);
          
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
