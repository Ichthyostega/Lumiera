/*
  DISPOSITION.hpp  -  actual constitution of a parameter descriptor

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file disposition.hpp
 ** Concrete configuration of a parameter descriptor with type and value provision.
 ** This is the full definition, with all details spelled out, and what is actually
 ** stored within the _outer shell_ of a Parameter. The publicly visible interface
 ** of the latter is implemented by delegating to the constituents of the disposition.
 ** 
 ** @todo WIP-WIP-WIP can be considered an initial draft and placeholder, as of 2026
 */


#ifndef LIB_PAR_DISPOSITION_H
#define LIB_PAR_DISPOSITION_H


#include "lib/par/domain.hpp"
#include "lib/par/provision.hpp"


namespace lib {
namespace par {
  
  
  
  /**
   * A complete configuration and setup of a parameter, including a type specification
   * with domain and scale, and a value provision. Can be generated through a builder
   * notation and stored within the Parameter object (as a container).
   */
  class Disposition
    {
    public:
    };
  
  
  
}} // namespace lib::par
#endif /*LIB_PAR_DISPOSITION_H*/
