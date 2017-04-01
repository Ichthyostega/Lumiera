/*
  StateProxy  -  Encapsulation of the state corresponding to a render calculation

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

* *****************************************************/


/** @file stateproxy.cpp
 ** Implementation details of some stateful operations needed from within the render calculation.
 ** @todo unfinished draft from 2009 regarding the render process
 */


#include "proc/engine/stateproxy.hpp"

namespace proc {
namespace engine {
  
  
  namespace { // Details...

    
  } // (END) Details...
  
  
//  using mobject::Placement;
//  using mobject::session::Effect;
  
  
  /** @internal */
  BuffHandle
  StateProxy::allocateBuffer (const lumiera::StreamType*)
  {
    UNIMPLEMENTED ("allocate a suitable buffer to hold a frame of the denoted type");
  }
  
  
      
  void
  StateProxy::releaseBuffer (BuffHandle& bh)
  {
    UNIMPLEMENTED ("free a buffer");
  }
  
  
      
  BuffHandle
  StateProxy::fetch (FrameID const& fID)
  {
    UNIMPLEMENTED ("fetch a buffer with input data");
  }
  
  
      
  void
  StateProxy::is_calculated (BuffHandle const& bh)
  {
    UNIMPLEMENTED ("declare a buffer as fully calculated and done");
  }
  
  
      
  FrameID const&
  StateProxy::genFrameID (NodeID const&, uint chanNo)
  {
    UNIMPLEMENTED ("derive/generate an ID to denote this specific fame+Node position in the graph");
  }
  
  

  BuffTableStorage&
  StateProxy::getBuffTableStorage() /////////////TODO need somehow to denote the specific storage requirements
  {
    UNIMPLEMENTED ("allocate a chunk of storage suitable for holding the buffer pointer tables");
  }
  
  
}} // namespace engine
