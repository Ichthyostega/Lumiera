/*
  Effect(Asset)  -  Effect or media processing component

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file asset/effect.cpp
 ** Implementation level functionality regarding the Asset representation of effects
 ** @todo still unimplemented and stalled work as of 2016
 */


#include "steam/asset/effect.hpp"

namespace steam {
namespace asset {
  
  Proc::ProcFunc*
  Effect::resolveProcessor()  const
  {
    UNIMPLEMENTED ("do the actual resolution plugin -> callable processing function");
  }



}} // namespace asset
