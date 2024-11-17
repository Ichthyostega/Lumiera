/*
  RENDER-INVOCATION.hpp  -  initiate the rendering of a single frame

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file render-invocation.hpp
 ** Initiate a single calculation unit within the renderengine.
 ** Usually, this will cause the rendering of a single frame or sub-frame.
 ** 
 ** @todo unfinished draft from 2009 regarding the render process
 ** 
 ** @see engine::ProcNode
 ** @see StateClosure
 ** @see node-basic-test.cpp
 **
 */

#ifndef ENGINE_RENDER_INVOCATION_H
#define ENGINE_RENDER_INVOCATION_H


//#include "steam/engine/state-closure.hpp"
//#include "steam/engine/proc-node.hpp"    ///////////////////////////////TODO clarify if required further on
#include "steam/engine/connectivity-obsolete.hpp"
#include "steam/engine/buffhandle.hpp"
//#include "steam/engine/bufftable-obsolete.hpp"




namespace steam {
namespace engine {
  
  
  /**
   * @todo write type comment
   */
  class RenderInvocation
    {
      ProcNode* theNode_;
      
    public:
      RenderInvocation (ProcNode* exitNode)
        : theNode_(exitNode)
        {
          REQUIRE (theNode_);
        }
      
      size_t size() { return theNode_->nrO(); }
      
      /** pull calculated data from the N-th node output channel */
      BuffHandle operator[] (size_t channel);
      
      
      
    };
  
  ///////////////////////////////////TODO: currently just fleshing out the API
  
  
  
  
  
}} // namespace steam::engine
#endif
