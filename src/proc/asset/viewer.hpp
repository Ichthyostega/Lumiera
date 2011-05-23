/*
  VIEWER.hpp  -  asset corresponding to a viewer element in the GUI

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file viewer.hpp
 ** structural element within the session.
 ** \c "timeline(theTimelineName),bindSequence(theTimelineName,sequenceID)." 
 ** 
 ** @see Session
 ** @see Sequence
 ** @see StructFactory
 **
 */


#ifndef ASSET_VIEWER_H
#define ASSET_VIEWER_H

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
