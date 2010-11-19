/*
  OutputDesignation  -  specifying a desired output destination
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/


#include "lib/error.hpp"
#include "proc/mobject/placement-ref.hpp"
#include "proc/mobject/output-designation.hpp"

namespace mobject {
  
  typedef OutputDesignation::PPipe PPipe;
  typedef OutputDesignation::PID PID;
  
  
  /** create an output designation by directly
   *  specifying the target to connect 
   */
  OutputDesignation::OutputDesignation (PID explicitTarget)
    : spec_() //////////TODO
    { }
  
  
  /** create an output designation indirectly
   *  to be resolved by forwarding the resolution
   *  to the given reference scope / mediator. 
   */
  OutputDesignation::OutputDesignation (RefPlacement const& indirectTarget)
    : spec_() //////////TODO
    { }
  
  
  /** create an output designation by relative specification,
   *  to be resolved based on the stream type and the actual
   *  default target object at hand when resolving.
   *  @param relative_busNr within the collection of target pipes
   *         available for the actual stream type to connect
   *  @note as the relative bus/pipe number defaults to 0,
   *        effectively this becomes a default ctor, denoting
   *        "connect me to the first bus suitable for my stream type" 
   */
  OutputDesignation::OutputDesignation (uint relative_busNr)
    : spec_() //////////TODO
    { }
  
  
  
  PID
  OutputDesignation::resolve (PPipe origin)
  {
    UNIMPLEMENTED ("Forward output designation resolution request to the embedded spec object");
  }
  
  
  
} // namespace mobject
