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
 ** @todo WIP-WIP-WIP 2024-12 finally about to connect the unfinished draft from 2009
 **       with the engine structures built bottom-up meanwhile    ///////////////////////////////////////////TICKET #905 : Work out what parameters are required to invoke the real code available now....
 ** 
 ** @see engine::ProcNode
 ** @see turnout-system.hpp
 ** @see NodeBase_test
 **
 */

#ifndef ENGINE_RENDER_INVOCATION_H
#define ENGINE_RENDER_INVOCATION_H


#include "vault/gear/job.h"
#include "steam/engine/proc-node.hpp"
//#include "steam/engine/buffhandle.hpp"




namespace steam {
namespace engine {
  
  
  /**
   * A concrete JobFunctor with the ability to activate the »Render Node Network«.
   * @todo write type comment
   * @warning WIP-WIP 2024-12 rework render node invocation for »Playback Vertical Slice«
   */
  class RenderInvocation
    : public vault::gear::JobClosure                         ////////////////////////////////////////////////TICKET #1287 : should inherit from JobFunctor, get rid of the C-isms
    {
      ProcNode& theNode_;
      
      
      /* === JobFunctor Interface === */
      
      JobKind
      getJobKind()  const override
        {
          return CALC_JOB;
        }
      
      string diagnostic()  const override;
      
      InvocationInstanceID buildInstanceID (HashVal)  const override;      //////////////////////////////////TICKET #1278 : so what do we need here for real, finally?
      size_t hashOfInstance (InvocationInstanceID invoKey)  const override;
      
      void invokeJobOperation (vault::gear::JobParameter);
      
    public:
      RenderInvocation (ProcNode& exitNode)
        : theNode_(exitNode)
        { }
    };
  
  
  
}} // namespace steam::engine
#endif
