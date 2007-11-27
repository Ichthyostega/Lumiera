/*
  EDL  -  the (high level) Edit Decision List within the current Session
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "proc/mobject/session/edl.hpp"
#include "proc/mobject/session/track.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/mobject/mobject.hpp"

namespace mobject
  {
  namespace session
    {

    /** @deprecated not sure if it is a good idea 
     *              to have this on the interface
     */
    bool 
    EDL::contains (const PMO& placement)
    {
      UNIMPLEMENTED ("test if a given placement is contained within this EDL");
    }
    
    
    PMO& 
    EDL::find (const string& id)
    {
      UNIMPLEMENTED ("serch for a given 'thing' within the EDL");
    }




  } // namespace mobject::session

} // namespace mobject
