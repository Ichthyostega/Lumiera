/*
  ParamType  -  parameter type record including domain and scale

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file param-type.cpp
 ** Implementation of parameter type registration.
 ** 
 ** @todo WIP-WIP-WIP can be considered an initial draft and placeholder, as of 2026
 */


#include "lib/error.hpp"
#include "lib/par/parameter.hpp"
#include "lib/par/param-type.hpp"

namespace lib {
namespace par {
  
  
  /* emit VTables in this translation unit... */
  
  ParamType::~ParamType() { }
  Domain::~Domain() { }
  
  
  
}} // namespace lib::par
