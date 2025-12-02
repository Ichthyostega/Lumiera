/*
  ParamProvider  -  interface denoting a source for actual Parameter values

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file paramprovider.cpp
 ** Implementation functions of the Parameter abstraction.
 ** 
 ** @todo as of 2016, we still need to work out our parameter/automation concept.
 */


#include "steam/mobject/paramprovider.hpp"
#include "steam/mobject/parameter.hpp"
#include "steam/mobject/interpolator.hpp"

namespace steam {
namespace mobject {


  template<class VAL>
  VAL
  ParamProvider<VAL>::getValue ()
  {}



}} // namespace steam::mobject
