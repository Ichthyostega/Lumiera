/*
  EXPLICITPLACEMENT.hpp  -  special Placement yielding an absolute location (Time,Output) for a MObject

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


/** @file §§§
 ** TODO §§§
 */


#ifndef MOBJECT_EXPLICITPLACEMENT_H
#define MOBJECT_EXPLICITPLACEMENT_H

#include "proc/mobject/placement.hpp"



namespace proc {
namespace mobject {
  
  
  
  /**
   * Special kind of Placement, where the location of the
   * MObject has been nailed down to a fixed position.
   * The Session maintains a special list of Placements,
   * which actually is a snapshot of all Session contents fixed
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
      const Pipe pipe;
      
      typedef std::pair<Time,Pipe> SolutionData;  //TODO (ichthyo considers better passing of solution by subclass)
      
      /** no need to resolve any further, as this ExplicitPlacement
       *  already \e is the result of a resolve()-call.
       *                                           //////////////////////TICKET #439
       */
      virtual
      ExplicitPlacement resolve ()  const 
        { 
          return *this; 
        }
      
      
    protected:
      /*  @todo ichthyo considers a much more elegant implementation utilising a subclass 
       *        of FixedLocation, which would serve as Placement::LocatingSolution, and
       *        would be used as LocatingPin::chain subobject as well, so that it could
       *        be initialised directly here in the ExplicitPlacement ctor. 
       *        (ichthyo: see Trac #100)
       */
      ExplicitPlacement (const Placement<MObject>& base, const SolutionData found)
        : Placement<MObject>(base),
          time(found.first), pipe(found.second)
        { };
        
      friend ExplicitPlacement Placement<MObject>::resolve () const;
      
    private:
      /** copying prohibited, ExplicitPlacement is effectively const! */
      ExplicitPlacement& operator= (const ExplicitPlacement&);
    };
  
  
  
}} // namespace proc::mobject
#endif
