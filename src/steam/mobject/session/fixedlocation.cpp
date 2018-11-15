/*
  FixedLocation  - directly positioning a MObject to a fixed location  

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/


/** @file fixedlocation.cpp
 ** @todo stalled effort towards a session implementation from 2008
 ** @todo 2016 likely to stay, but expect some extensive rework
 */


#include "steam/mobject/session/fixedlocation.hpp"

namespace proc {
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
  
  
  
}}} // namespace proc::mobject::session
