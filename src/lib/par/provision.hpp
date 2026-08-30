/*
  PROVISION.hpp  -  determine how a parameter's value is given

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file provision.hpp
 ** Provide access to a parameter's value, either directly or by automation.
 ** Each parameter has a _base value_ and a time-based current value. The latter
 ** is retrieved by evaluating an automation function.
 ** 
 ** @todo WIP-WIP-WIP can be considered an initial draft and placeholder, as of 2026
 */


#ifndef LIB_PAR_PROVISION_H
#define LIB_PAR_PROVISION_H


#include "lib/par/param-type.hpp"


namespace lib {
namespace par {
  
  
  
  /**
   * Interface: access to a parameter's value, to adjust or evaluate automation.
   * Conceptually, the value is decomposed into a _base value_ and possibly a
   * time-controlled adjustment (automation). The base value can be retrieved
   * and assigned, while the current time-based value is the result of evaluation.
   * Any parameter value is associated to a ParamType that defines the underlying
   * value domain and possibly a scale with range limits and metric constraints.
   */
  class Provision
    {
    public:
      virtual ~Provision();  ///< this is an interface
    };
  
  
  
}} // namespace lib::par
#endif /*LIB_PAR_PROVISION_H*/
