/*
  FORK.hpp  -  A grouping device within the Session.

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


#ifndef MOBJECT_SESSION_FORK_H
#define MOBJECT_SESSION_FORK_H

#include "proc/mobject/session/meta.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/time/timevalue.hpp"



namespace proc {
namespace mobject {
namespace session {     //////////////////////////////////////////////////////TICKET #637
  
  using lib::time::Time;
  
  class Fork;
  
  using ForkID = lib::idi::EntryID<Fork>;
  
  
  
  
                        //////////////////////////////////////////////////////TICKET #646
                        //////////////////////////////////////////////////////TICKET #715
    
    /**
     * A Fork serves as grouping device within the Session.
     * Within the timeline, forks are used to organise continuous trails of consecutive
     * clips -- similar to the role of "Tracks" in other editing applications. Just, in
     * Lumiera a Fork is \em only a grouping device and not limited just to video or audio.
     * Moreover, Forks are also used to organise and group clips in preparation to editing
     * (the so called "media bins"). And forks can be used to create assortments of effects
     * and other editing tools. In all these cases, the Fork creates a \em scope.
     * 
     * The corresponding Placement by which this Fork object is attached to the session
     * defines fallback placing properties to be used by all objects placed within the
     * scope of this fork, unless individual objects specify more concrete placements.
     * Typically, the placement within a fork of the timeline is used to derive the
     * default processing pipe connections, define a layer or pan for sound and also
     * for for disabling groups of clips. But note that forks are always nested,
     * forming a tree like grouping device with hierarchically nested scopes.
     * \par
     * This Fork Media Object (sometimes referred to as "fork-MO") is always dealt with
     * locally within one Sequence. Client code normally doesn't have to care for creating
     * or retrieving a fork-MO. Rather, it refers to the global fork-asset-ID. The same
     * holds true when placing some other Media Object into the scope of a fork:
     * the corresponding placement just refers the global forkID, while the builder
     * automatically retrieves the matching fork-MO for the Sequence in question.
     */
    class Fork : public Meta
      {
        Time start_;    /////////////////////////////////TODO: this doesn't belong here. The start position is governed by the Placement!!!
        ForkID id_;
        
      
        string
        initShortID()  const
          {
            return buildShortID("Fork");
          }
        
        bool isValid()  const;
        
      protected:
        Fork (ForkID const&);
        friend class MObjectFactory;
        
      public:
        /** Child tracks in a tree structure */
//      vector<Placement<Track> > subTracks;  ///////////TODO: it should really work with PlacementRefs! this here is just a decoy!!!!!!!
/////////////////////////////////////////////////////////TICKET #513
        
        bool isSameID (string const&);
        
      };
      
    
    inline bool
    Fork::isSameID(string const& refID)
    {
      return refID == id_.getSym();
    }
    
    
  } // namespace mobject::session
  
  /** Placement<Fork> defined to be subclass of Placement<Meta> */
  template class Placement<session::Fork, session::Meta>;
  typedef Placement<session::Fork, session::Meta> PFork;
  
}} // namespace proc::mobject
#endif /*MOBJECT_SESSION_FORK_H*/
