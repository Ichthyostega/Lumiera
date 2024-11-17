/*
  RenderInvocation  -  Interface for creating processing nodes of various kinds

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file render-invocation.cpp
 ** Implementation details regarding the invocation of a single render node
 ** @deprecated very likely to happen in a different way, while the concept remains valid
 ** @todo unfinished draft from 2009 regarding the render process
 */


#include "steam/engine/render-invocation.hpp"
#include "steam/engine/turnout-system.hpp"


namespace steam {
namespace engine {
  
  namespace { // Details...

    
  } // (END) Details...
  
  
//  using mobject::Placement;
//  using mobject::session::Effect;
  
  
  /** @note this builds a one-way off invocation state context
   *        and then forwards the call; this may or may not cause
   *        actual calculations, depending on the cache.
   *  @todo for real use within the engine, the pull()-call should be
   *        dispatched through the scheduler; of course then the call semantics
   *        would be completely different. Maybe this invocation will be test only?  
   */
  BuffHandle
  RenderInvocation::operator[] (size_t channel)
  {
    REQUIRE (theNode_);
    REQUIRE (channel < size());
    
    StateProxy invocationState;
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
    return theNode_->pull(invocationState, channel);
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
  }
  
  
}} // namespace engine
