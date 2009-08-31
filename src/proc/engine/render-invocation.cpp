/*
  RenderInvocation  -  Interface for creating processing nodes of various kinds
 
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
 
* *****************************************************/


#include "proc/engine/render-invocation.hpp"

namespace engine {
  
  namespace { // Details...

    
  } // (END) Details...
  
  
//  using mobject::Placement;
//  using mobject::session::Effect;
  
  
  /** @internal */
  BuffHandle
  RenderInvocation::allocateBuffer (BufferDescriptor const&)
  {
    UNIMPLEMENTED ("allocate a suitable buffer to hold a frame of the denoted type");
  }
  
  
      
  void
  RenderInvocation::releaseBuffer (BuffHandle& bh)
  {
    UNIMPLEMENTED ("free a buffer");
  }
  
  
      
  BuffHandle
  RenderInvocation::fetch (FrameID const& fID)
  {
    UNIMPLEMENTED ("fetch a buffer with input data");
  }
  
  
      
  void
  RenderInvocation::is_calculated (BuffHandle const& bh)
  {
    UNIMPLEMENTED ("declare a buffer as fully calculated and done");
  }
  
  
      
  FrameID const&
  RenderInvocation::genFrameID (NodeID const&, uint chanNo)
  {
    UNIMPLEMENTED ("derive/generate an ID to denote this specific fame+Node position in the graph");
  }
  
  

  BuffTableStorage&
  RenderInvocation::getBuffTableStorage() /////////////TODO need somehow to denote the specific storage requirements
  {
    UNIMPLEMENTED ("allocate a chunk of storage suitable for holding the buffer pointer tables");
  }
  
  
} // namespace engine
