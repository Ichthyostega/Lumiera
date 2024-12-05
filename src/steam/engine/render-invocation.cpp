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
 ** @todo WIP-WIP-WIP 12/2024 about to build a Render Node invocation, combining the old
 **                   unfinished draft from 2009 with the new Render Engine code
 */


#include "steam/engine/turnout-system.hpp"
#include "steam/engine/render-invocation.hpp"


using vault::gear::JobParameter;


namespace steam {
namespace engine {
  
  namespace { // Details...
    
  } // (END) Details...
  
  
//  using mobject::Placement;
//  using mobject::session::Effect;
  
  
  string
  RenderInvocation::diagnostic()  const
  {
    return "TODO ... what can the ExitNode tell us here?";    ///////////////////////////////////////////////TICKET #1367 : once we have a working draft of a node invocation, it should be possible to retrieve some Node-Spec here...
  }
  
  InvocationInstanceID
  RenderInvocation::buildInstanceID (HashVal)  const
  {
    return InvocationInstanceID();                     //////////////////////////////////////////////////////TICKET #1278 : so what do we need here for real, finally?
  }
  
  size_t
  RenderInvocation::hashOfInstance (InvocationInstanceID invoKey)  const
  {                                                          ////////////////////////////////////////////////TICKET #1295 : settle upon the parameters actually needed and decide what goes into this hash
    std::hash<size_t> hashr;
    HashVal res = hashr (invoKey.frameNumber);
    return res;
  }
  
  
  /** @note this builds a one-way off invocation state context
   *        and then forwards the call; this may or may not cause
   *        actual calculations, depending on the cache.
   */
  void
  RenderInvocation::invokeJobOperation (JobParameter invoParam)
  {                                                          ////////////////////////////////////////////////TICKET #905 : yess.... finally DO IT
    UNIMPLEMENTED ("how to »doIt«");
    
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
    return theNode_->pull(invocationState, channel);
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
  }
  
  
}} // namespace engine
