/*
  FixedLocation  - directly positioning a MObject to a fixed location  

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file fixedlocation.cpp
 ** @todo stalled effort towards a session implementation from 2008
 ** @todo 2016 likely to stay, but expect some extensive rework
 */


#include "steam/mobject/session/fixedlocation.hpp"

namespace steam {
namespace mobject {
namespace session {
  
  
  FixedLocation* 
  FixedLocation::clone ()  const
  { 
    return new FixedLocation (*this);
  }
  
  
  /** */
  void
  FixedLocation::intersect (LocatingSolution& solution)  const
  {
    LocatingPin::intersect (solution);
    
    TODO ("either set position or make overconstrained");
    if (solution.minTime <= this->time_)
      solution.minTime = this->time_;
    else
      solution.impo = true;
    if (solution.maxTime >= this->time_)
      solution.maxTime = this->time_;
    else
      solution.impo = true;
  }
  
  
  
}}} // namespace steam::mobject::session
