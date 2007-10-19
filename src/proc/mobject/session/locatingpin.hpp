/*
  LOCATINGPIN.hpp  -  Chaining and constraining the Placement of a Media Object
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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


#ifndef MOBJECT_SESSION_LOCATINGPIN_H
#define MOBJECT_SESSION_LOCATINGPIN_H


#include "cinelerra.h"

#include <boost/scoped_ptr.hpp>
using boost::scoped_ptr;



namespace mobject
  {
  class Placement;

  namespace session
    {
    class FixedLocation;
    class RelativeLocation;
    
    
    struct LocatingPin
      {
      protected:
      typedef cinelerra::Time Time;
      typedef session::Track* Track;
      
        /** next additional Pin, if any */
        scoped_ptr<LocatingPin> next;
      public:
        
        /* Factory functions for adding LocationPins */
        
        FixedLocation&    operator() (Time, Track);
        RelativeLocation& operator() (PMO refObj, Time offset=0);

      };

  } // namespace mobject::session

} // namespace mobject
#endif
