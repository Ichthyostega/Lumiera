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
 ** on the name-ID of the timeline: \c Struct::retrieve(Query<Timeline>("id(theName)."))
 ** Additionally, the binding to a specific sequence may be established alongside:
 ** \c "timeline(theTimelineName),bindSequence(theTimelineName,sequenceID)." 
 ** 
 ** @see Session
 ** @see Sequence
 ** @see StructFactory
 **
 */


#ifndef ASSET_TIMELINE_H
#define ASSET_TIMELINE_H

#include "proc/asset/struct.hpp"
//#include "proc/mobject/mobject.hpp"
//#include "proc/mobject/placement.hpp"
#include "proc/mobject/mobject-ref.hpp"
//#include "proc/mobject/session/binding.hpp"         ////TODO avoidable??
#include "lib/p.hpp"
#include "lib/element-tracker.hpp"


//#include <vector>
//#include <string>

//using std::vector;
//using std::string;

namespace mobject {
namespace session {
  
  class Binding;
  typedef MORef<Binding> RBinding;
}}


namespace asset {
  
  
//  using lumiera::P;
  class Timeline;
  typedef lumiera::P<Timeline> PTimeline;
  
  
  /**
   * TODO type comment
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
  
  
} // namespace asset
#endif
