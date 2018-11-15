/*
  RELATIVELOCATION.hpp  -  Placement implementation attaching MObjects relative to another one

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


/** @file relativelocation.hpp
 ** @todo stalled effort towards a session implementation from 2008
 ** @todo 2016 likely to stay, but expect some extensive rework
 */


#ifndef MOBJECT_SESSION_RELATIVELOCATION_H
#define MOBJECT_SESSION_RELATIVELOCATION_H

#include "steam/mobject/placement-ref.hpp"
#include "steam/mobject/session/locatingpin.hpp"




namespace proc {
namespace mobject {
namespace session {
  
  using lib::time::Offset;
  
  
  /**
   * @todo just a design sketch, nothing finished yet.
   *       possibly to be reworked or phased out completely.
   *       See Trac #100
   */
  class RelativeLocation : public LocatingPin
    {
//        const PMO & anchor_;  ////////////TODO: ooooops, this is a nasty design problem!!!
      
      const PlaRef anchor_;
      
    public:
      
      /**
       * the possible kinds of RelativePlacements
       */
      enum RelType
      { SAMETIME    /**< place subject at the same time as the anchor  */
      , ATTACH      /**< attach subject to anchor (e.g. an effect to a clip) */
      };
      
    protected:
      RelativeLocation (PlaRef const& a, Offset const& ofs) : anchor_(a), offset_(ofs) { }
      friend class LocatingPin;
      
      /** the kind of relation denoted by this Placement */
      RelType relType;
      
      /** Offset the actual position by this (time) value relative to the anchor point. */
      Offset offset_;
      //TODO: suitable representation?
      
      
      virtual void intersect (LocatingSolution&)  const;
      
      
    public:
      virtual RelativeLocation* clone ()  const;
      
    };
  
  
  
}}} // namespace proc::mobject::session
#endif
