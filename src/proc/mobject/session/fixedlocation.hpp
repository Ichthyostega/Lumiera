/*
  FIXEDLOCATION.hpp  -  directly positioning a MObject to a fixed location   

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

*/


#ifndef MOBJECT_SESSION_FIXEDLOCATION_H
#define MOBJECT_SESSION_FIXEDLOCATION_H

#include "proc/mobject/session/locatingpin.hpp"



namespace mobject
  {
    class ExplicitPlacement; //TODO trac #100
    
  namespace session
    {

    /** 
     * The most common case of positioning a MObject
     * in the Session: directly specifying a constant position.
     * @todo use a subclass to represent the LocatingSolution?
     *       would solve the construction of a ExplicitPlacement
     *       much more natural. (ichthyo: siehe trac #100) 
     */
    class FixedLocation : public LocatingPin
      {
        Time time_;
        Track track_;
        
        friend class ExplicitPlacement; //TODO trac #100
        
      protected:
        FixedLocation (Time ti, Track tra) : time_(ti), track_(tra) {};
        friend class LocatingPin;
        
        virtual void intersect (LocatingSolution&)  const;
        
      public:
        virtual FixedLocation* clone ()  const;
        
      };



  } // namespace mobject::session

} // namespace mobject
#endif
