/*
  RelativeLocation  -  Placement implementation attaching MObjects relative to another one

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


#include "proc/mobject/session/relativelocation.hpp"
#include "proc/mobject/mobject.hpp"

namespace proc {
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
  
  
}}} // namespace proc::mobject::session
