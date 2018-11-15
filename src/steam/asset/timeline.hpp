/*
  TIMELINE.hpp  -  independent top-level element of the Session

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file timeline.hpp
 ** Top level structural element within the session.
 ** Each Lumiera session may contain multiple top level timeline containers,
 ** which at the same time act as structural asset and as part of the public
 ** session API exposed to clients for discovering the session contents.
 ** Actually, Timelines are facade objects, delegating the implementation to
 ** the BindingMO, the Axis and the Sequences/Tracks. 
 ** 
 ** Contrary to usual habits in video/sound editing software, in Lumiera the
 ** tracks are \em not part of the timeline, but rather attached directly to
 ** the sequence container. To be usable, a timeline needs a binding to refer
 ** to such a sequence, but this sequence may be bound into multiple timelines
 ** or even virtual clips simultaneously. 
 ** 
 ** Like every structural asset, the creation of timelines happens automatically
 ** on referral; Timelines can be queried from the StructFactory, providing additional
 ** requested capabilities. Commonly clients will retrieve a given timeline by query
 ** on the name-ID of the timeline: `Struct::retrieve (Query<Timeline>("id(theName)."))`
 ** Additionally, the binding to a specific sequence may be established alongside:
 ** `"timeline(theTimelineName), bindSequence(theTimelineName,sequenceID)."`
 ** 
 ** @todo around 2010, the concept of Timeline and Session binding was defined,
 **       together with a rough preliminary implementation. Up to 2017, there
 **       was no opportunity to set this system really into motion; this is
 **       not necessarily a bad thing, since meanwhile we understand way
 **       better in which way the Session will actually be accessed...
 ** @todo looks like we'll represent a secondary, attached slave display of the
 **       same Timeline rather by materialising it into a TimelineClone within
 **       the session. This topic has been postponed as of 10/2018     //////////////////////////////////////TICKET #1083
 ** @see Session
 ** @see Sequence
 ** @see StructFactory
 **
 */


#ifndef ASSET_TIMELINE_H
#define ASSET_TIMELINE_H

#include "steam/asset/struct.hpp"
//#include "steam/mobject/mobject.hpp"
//#include "steam/mobject/placement.hpp"
#include "steam/mobject/mobject-ref.hpp"
//#include "steam/mobject/session/binding.hpp"         ////TODO avoidable??
#include "lib/p.hpp"
#include "lib/element-tracker.hpp"


//#include <vector>
//#include <string>

//using std::vector;
//using std::string;

namespace proc {
namespace mobject {
namespace session {
  
  class Binding;
  typedef MORef<Binding> RBinding;
}}


namespace asset {
  
  
  class Timeline;
  typedef lib::P<Timeline> PTimeline;
  
  
  /**
   * @todo this new Timeline API was invented about 2010
   *       and remained in half finished state ever since.
   * @todo 2016 can confirm that we still want to go that route
   * @todo we need some actual interface, beyond just creating timelines!
   */
  class Timeline
    : public Struct
    , public lib::AutoRegistered<Timeline>
    {
      typedef mobject::session::RBinding RBinding;
      
      RBinding boundSequence_;
      
      Timeline (Ident const&, RBinding const&);
      
    public:
      /** create and register a new Timeline instance */
      static PTimeline create (Asset::Ident const& idi, RBinding const& sequenceBinding);
      
    protected:
      virtual void unlink ();
      
    };
    
  
  
  
///////////////////////////TODO currently just fleshing the API
  
  
}} // namespace proc::asset
#endif
