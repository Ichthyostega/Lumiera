/*
  TRACK.hpp  -  structural asset used as global track identifier
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#ifndef ASSET_TRACK_H
#define ASSET_TRACK_H

#include "proc/asset/struct.hpp"



namespace asset {
  
  
  /**
   * Structural Asset used as a global identifier for placing
   * media objects onto a given track. Not to be confused with the "track-MO",
   * the MObject which becomes part of the high-level-model:
   * In order actually to use a track within an Sequence, we need to attach a 
   * Placement<mobject::session::Track> to the tree-of-tracks of this Sequence.
   * Thus, we have one global track-identifier (this class here), but maybe
   * several instances (placement of a track-MO) within various Sequences
   */
  class Track : public Struct
    {
    protected:
      Track (const Asset::Ident& idi);
      friend class StructFactoryImpl;
    };
    
  
  typedef P<const Track> PTrack;
  
  
  
} // namespace asset
#endif
