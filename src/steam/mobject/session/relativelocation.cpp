/*
  RelativeLocation  -  Placement implementation attaching MObjects relative to another one

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file relativelocation.cpp
 ** LocatingPin (constraint) to attach media objects relative to each other
 ** @todo stalled effort towards a session implementation from 2008
 ** @todo 2016 likely to stay, but expect some extensive rework
 */


#include "steam/mobject/session/relativelocation.hpp"
#include "steam/mobject/mobject.hpp"

namespace steam {
namespace mobject {
namespace session {
  
  RelativeLocation* 
  RelativeLocation::clone ()  const
  { 
    return new RelativeLocation (*this);
  }
  
  
  void 
  RelativeLocation::intersect (LocatingSolution& solution)  const
  {
    LocatingPin::intersect (solution);
    
    TODO ("either set position or make overconstrained");
  }
  
  
}}} // namespace steam::mobject::session
