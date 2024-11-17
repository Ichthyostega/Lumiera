/*
  TurnoutSystem  -  Encapsulation of the state corresponding to a render calculation

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file turnout-system.cpp
 ** Implementation details of some statefull operations needed from within the render calculation.
 ** @todo unfinished draft from 2009 regarding the render process
 */


#include "steam/engine/turnout-system.hpp"

namespace steam {
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
