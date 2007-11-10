/*
  EXPLICITPLACEMENT.hpp  -  special Placement yielding an absolute location (Time,Track) for a MObject
 
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


#ifndef MOBJECT_EXPLICITPLACEMENT_H
#define MOBJECT_EXPLICITPLACEMENT_H

#include "proc/mobject/placement.hpp"



namespace mobject
  {



  /**
   * Special kind of Placement, where the location of the
   * MObject has been nailed down to a fixed position.
   * The Session allways contains one special EDL, which
   * actually is a snapshot of all EDLs contents fixed
   * and reduced to simple positions. This so called Fixture
   * contains only ExplicitPlacement objects and is processed
   * by the Builder to create the render engine  node network.
   *
   * @see Placement#resolve factory method for deriving an ExplicitPlacement 
   */
  class ExplicitPlacement : public Placement<MObject>
    {
    public:
      const Time time;
      const Track track;

      /** no need to resolve any further, as this ExplicitPlacement
       *  already \i is the result of a resolve()-call.
       */
      virtual
      ExplicitPlacement resolve ()  const 
        { 
          return *this; 
        }

    protected:
      /*  @todo ichthyo considers a much more elegant implementation utilizing a subclass 
       *        of FixedLocation, which would serve as Placement::LocatingSolution, and
       *        would be used as LocatingPin::chain subobject as well, so that it could
       *        be initialized directly here in the ExplicitPlacement ctor. 
       *        (ichthyo: siehe Trac #100)
       */
      ExplicitPlacement (Placement<MObject>& base, const session::FixedLocation found)
        : Placement(base),
          time(found.time_), track(found.track_)
        { };
        
      friend ExplicitPlacement::resolve () const;
    };



} // namespace mobject
#endif
