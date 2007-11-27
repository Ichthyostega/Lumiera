/*
  RELATIVELOCATION.hpp  -  Placement implementation attaching MObjects relative to another one
 
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


#ifndef MOBJECT_SESSION_RELATIVELOCATION_H
#define MOBJECT_SESSION_RELATIVELOCATION_H

#include "proc/mobject/session/locatingpin.hpp"




namespace mobject
  {
  namespace session
    {



    class RelativeLocation : public LocatingPin
      {
        const PMO & anchor_;  ////////////TODO: ooooops, this is a nasty design problem!!!
        
      public:

        /**
         * the possible kinds of RelativePlacements
         */
        enum RelType
        { SAMETIME    /**< place subject at the same time as the anchor  */
        , ATTACH      /**< attach subject to anchor (e.g. an effect to a clip) */
        };

      protected:
        RelativeLocation (const PMO& a, Time ofs) : anchor_(a), offset_(ofs) { }
        friend class LocatingPin;

        /** the kind of relation denoted by this Placement */
        RelType relType;

        /** Offset the actual position by this (time) value relative to the anchor point. */
        Time offset_;
        //TODO: suitable representation?
        
        
        virtual void intersect (LocatingSolution&)  const;
        
        
      public:
        virtual RelativeLocation* clone ()  const;

      };



  } // namespace mobject::session

} // namespace mobject
#endif
