/*
  BUS-MO.hpp  -  attachment point to form a global pipe

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file bus-mo.hpp
 ** MObject in the Session to represent a processing pipe.
 ** Within the Session model, Pipes are conceptual entities, which do not correspond
 ** 1:1 to some render nodes, but rather help the _user_ to organise the processing steps
 ** required to get some piece of the film into desired shape
 ** @todo stalled effort towards a session implementation from 2010
 ** @todo 2016 likely to stay, but expect some extensive rework
 */


#ifndef STEAM_MOBJECT_SESSION_BUS_MO_H
#define STEAM_MOBJECT_SESSION_BUS_MO_H

#include "steam/mobject/session/meta.hpp"
#include "steam/mobject/builder/buildertool.hpp"


namespace proc {
namespace asset { 
  
  class Pipe;
  typedef lib::P<Pipe> PPipe;

}

namespace mobject {
namespace session {
    
    using asset::PPipe;
    
    /**
     * Model entity corresponding to a global pipe.
     * This MObject acts as scope and attachment point to form a global pipe.
     * Each Timeline (implemented as Binding-MObject) holds a collection of
     * such global pipes, which then in turn may be nested.
     */
    class BusMO : public Meta
      {
        PPipe pipe_;
        
        string
        initShortID()  const
          {
            return buildShortID("Bus");
          }
        
        bool isValid()  const;
        
      public:
        BusMO (PPipe const& pipe_to_represent);
          
        DEFINE_PROCESSABLE_BY (builder::BuilderTool);
        
      };
    
  } // namespace mobject::session
  
  /** Placement<BusMO> defined to be subclass of Placement<Meta> */
  template class Placement<session::BusMO, session::Meta>;
  typedef Placement<session::BusMO, session::Meta> PBus;
  
}} // namespace proc::mobject
#endif
