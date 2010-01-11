/*
  TRACK.hpp  -  A grouping device within the EDL.
 
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


#ifndef MOBJECT_SESSION_TRACK_H
#define MOBJECT_SESSION_TRACK_H

#include "proc/mobject/session/meta.hpp"



namespace asset { class Track; }

namespace mobject {
namespace session {
    
    using lumiera::P;
    
    class Track;
    typedef asset::Track TrackAsset;
    
    typedef P<Track> PTrack;
    typedef P<TrackAsset> PTrackAsset;
    
    
    /**
     * A Track is grouping device within the EDL.
     * The corresponding Placement by which this Track object is referred
     * defines fallback placing properties to be used by all objects placed on this track
     * in case they don't specify more concrete placements.
     * Typically, tracks are used do make default processing pipe connections,
     * define a layer or pan for sound and for for disabling groups
     * of clips. Note tracks are grouped in a tree like fashion.
     * \par
     * This Media Object (often referred to as "track-MO") is always dealt with
     * locally within one EDL. Client code normally doesn't have to care for creating
     * or retrieving track-MO. Rather, it refers to the global track-asset-ID. The same
     * holds true when placing some other Media Object onto a track: the corresponding
     * placement just refers the global trackID, while the builder automatically retrieves
     * the matching track-MO for the EDL in question. If some EDL contains several instances
     * (track-MO) referring to the same trackID (asset), then this causes all objects placed
     * onto this track to be included several times in the resulting render nodes network
     * (possibly with varying placement properties) 
     */
    class Track : public Meta
      {
        Time start_;
        PTrackAsset trackDef_;
        
      protected:
        Track (PTrackAsset&);
        friend class MObjectFactory;
        
      public:
        /** Child tracks in a tree structure */
        vector<Placement<Track> > subTracks;  ///////////TODO: it should really work with PlacementRefs! this here is just a decoy!!!!!!!
/////////////////////////////////////////////////////////TICKET #513
        
        
        virtual bool isValid()  const;
      };
      
      
      
  } // namespace mobject::session
  
  /** Placement<Track> defined to be subclass of Placement<Meta> */
  template class Placement<session::Track, session::Meta>;
  typedef Placement<session::Track, session::Meta> PTrack;
  
} // namespace mobject
#endif
