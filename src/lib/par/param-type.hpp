/*
  PARAM-TYPE.hpp  -  parameter scale and type descriptor

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file param-type.hpp
 ** Specification of the value domain and scale of a parameter.
 ** A ParamType descriptor implicitly also encodes the concrete value type, the
 ** allowed value range, clipping behaviour and specifics of the scale (discrete,
 ** stepping, continuous, logarithmic...).
 ** 
 ** @todo WIP-WIP-WIP can be considered an initial draft and placeholder, as of 2026
 */


#ifndef LIB_PAR_PARAM_TYPE_H
#define LIB_PAR_PARAM_TYPE_H


#include "lib/par/domain.hpp"


namespace lib {
namespace par {
  
  
  
  /**
   * Interface: represent a specific configuration of parameter value type,
   * range and scale. Every ParamType descriptor is also a _prototype_ and has
   * an unique identity, ensured by registration, so that it acts as a type identifier. 
   * Furthermore, the interface exposes allowed conversion and conforming operations.
   */
  class ParamType
    {
    public:
      virtual ~ParamType();  ///< this is an interface
    };
  
  
  
}} // namespace lib::par
#endif /*LIB_PAR_PARAM_TYPE_H*/
