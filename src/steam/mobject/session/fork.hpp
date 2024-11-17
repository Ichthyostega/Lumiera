/*
  FORK.hpp  -  A grouping device within the Session.

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file fork.hpp
 ** Organisational grouping device within the Session model ("Track" / "Media Bin").
 ** Within Lumiera, Tracks bear no direct relation to the rendering or calculation process;
 ** rather they are just conceived as a space for the user to arrange the parts included
 ** into the edit.
 ** 
 ** A Fork is a nested tree-shaped structure. When integrated into a sequence, it will be
 ** rendered in the familiar way, as tracks with media clips. But at the same time, when
 ** accessed through the _Asset management view_ ("bookkeeping view"), a fork appears as
 ** nested folder structure to hold media clips.
 ** 
 ** Most importantly, a Fork defines a _system of nested scopes._ When discovering details
 ** of the wiring, setup and configuration, the Build process will look into the enclosing
 ** scope to fill in any part not defined locally at a given media object. Go give a typical
 ** example, the _volume for sound playback_ can be defined in some root scope, causing all
 ** sound objects to _inherit_ that volume setting -- unless shadowed by a more specialised
 ** setting closer in scope to the sound object in question. This allows to set up global
 ** properties and then to override them locally, for a group of objects located in some
 ** sub-fork.
 ** 
 ** @note to stress this point: in Lumiera we do _not conceive tracks as some kind of
 **       channel, with media data flowing through the tracks._ Also, _tracks are not layers._
 **       This also means, there is _no distinction in audio and video tracks._
 **       We leave it at the user's discretion how she wants to organise the edit.
 ** 
 ** @todo WIP implementation of session core from 2010
 ** @todo as of 2016, this effort is considered stalled but basically valid
 */


#ifndef MOBJECT_SESSION_FORK_H
#define MOBJECT_SESSION_FORK_H

#include "steam/mobject/session/meta.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/time/timevalue.hpp"



namespace steam {
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
//      vector<Placement<Track>> subTracks;   ///////////TODO: it should really work with PlacementRefs! this here is just a decoy!!!!!!!
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
  
}} // namespace steam::mobject
#endif /*MOBJECT_SESSION_FORK_H*/
