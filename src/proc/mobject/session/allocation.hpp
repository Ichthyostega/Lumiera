/*
  ALLOCATION.hpp  -  objective to place a MObject in a specific way
 
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


#ifndef MOBJECT_SESSION_ALLOCATION_H
#define MOBJECT_SESSION_ALLOCATION_H

#include <string>

#include "proc/mobject/session/locatingpin.hpp"


using std::string;


namespace mobject
  {
  namespace session
    {


    /**
     * Interface (abstract): any objective, constraint or wish
     * of placing a MObject in a specific way.
     */
    class Allocation : public LocatingPin
      {
      protected:
        /** human readable representation of the condition
         *  characterizing this allocaton, e.g. "t >= 10"
         */
        string repr;
        
        virtual void intersect (LocatingSolution&)  const;
        
      public:
        const string& getRepr () const { return repr; }
        
        virtual Allocation* clone ()  const = 0;
      };



  } // namespace mobject::session

} // namespace mobject
#endif
