/*
  MobjectRef  -  active external reference to an MObject within the Session

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


/** @file mobject-ref.cpp 
 ** Implementation part of the MObjectRef facility. 
 ** Any C++ client about to use a MObjectRef needs to be linked (dynamically) against
 ** this implementation. Especially, this implementation allows to (re)-connect transparently
 ** to the PlacementIndex within the session, so to establish a direct link to the Object
 ** denoted by the reference. 
 ** 
 ** @see PlacementRef
 ** @see PlacementIndex
 **
 */


#include "steam/mobject/mobject-ref.hpp"
#include "steam/mobject/mobject.hpp"
#include "steam/mobject/placement.hpp"


namespace proc {
namespace mobject {
  
  /** */
  
  
  LUMIERA_ERROR_DEFINE (INVALID_PLACEMENTREF, "unresolvable placement reference, or of incompatible type");
  LUMIERA_ERROR_DEFINE (BOTTOM_PLACEMENTREF,  "NIL placement-ID marker encountered.");
  LUMIERA_ERROR_DEFINE (BOTTOM_MOBJECTREF,    "NIL MObjectRef encountered");
  
  
  
  
  
}} // namespace proc::mobject
