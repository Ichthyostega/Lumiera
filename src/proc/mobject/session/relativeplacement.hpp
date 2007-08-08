/*
  RELATIVEPLACEMENT.hpp  -  Placement implemnetaion providing various ways of attaching a MObject to another one
 
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


#ifndef PROC_MOBJECT_SESSION_RELATIVEPLACEMENT_H
#define PROC_MOBJECT_SESSION_RELATIVEPLACEMENT_H

#include "proc/mobject/placement.hpp"
#include "common/time.hpp"




namespace proc
  {
  namespace mobject
    {
    namespace session
      {
      
      
      
      class RelativePlacement : public proc::mobject::Placement
        {
        public:
          
          /**
           * the possible kinds of RelativePlacements
           */
          enum RelType
          { SAMETIME    /** place subject at the same time as the anchor  */
          , ATTACH      /** attach subject to anchor (e.g. an effect to a clip) */
          };
          
        protected:
          MObject* anchor;

          /** the kind of relation denoted by this Placement */
          RelType relType;

          /** Offset the actual position by this (time) value relative to the anchor point. */
          cinelerra::Time offset;
          //TODO: suitable representation?
          
        };
        
        
        
    } // namespace proc::mobject::session

  } // namespace proc::mobject

} // namespace proc
#endif
