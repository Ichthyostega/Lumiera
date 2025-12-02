/*
  Parameter  -  representation of an automatable effect/plugin parameter

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file parameter.cpp
 ** Implementation functions of the Parameter abstraction.
 ** 
 ** @todo as of 2016, we still need to work out our parameter/automation concept.
 */


#include "lib/error.hpp"
#include "steam/mobject/parameter.hpp"
#include "steam/mobject/paramprovider.hpp"

namespace steam {
namespace mobject {
  
  
  template<class VAL>
  VAL
  Parameter<VAL>::getValue ()
  {
    UNIMPLEMENTED ("Any idea how to handle various kinds of parameter types");
  }
  
  
  
}} // namespace mobject
