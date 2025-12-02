/*
  RELATIVELOCATION.hpp  -  Placement implementation attaching MObjects relative to another one

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file relativelocation.hpp
 ** @todo stalled effort towards a session implementation from 2008
 ** @todo 2016 likely to stay, but expect some extensive rework
 */


#ifndef MOBJECT_SESSION_RELATIVELOCATION_H
#define MOBJECT_SESSION_RELATIVELOCATION_H

#include "steam/mobject/placement-ref.hpp"
#include "steam/mobject/session/locatingpin.hpp"




namespace steam {
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
  
  
  
}}} // namespace steam::mobject::session
#endif
