/*
  WISH.hpp  -  LocatingPin representing a low-priority directive
 
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
 
*/


#ifndef MOBJECT_SESSION_WISH_H
#define MOBJECT_SESSION_WISH_H

#include "proc/mobject/session/allocation.hpp"



namespace mobject
  {
  namespace session
    {


    /**
     * LocatingPin representing a low-priority directive by the user,
     * to be fulfilled only if possible (and after satisfying the
     * more important LocatingPins)
     */
    class Wish : public Allocation
      {
        
      };
    
    
    
  } // namespace mobject::session

} // namespace mobject
#endif
