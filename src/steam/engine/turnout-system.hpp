/*
  TURNOUT-SYSTEM.hpp  -  Encapsulation of the state corresponding to a render calculation

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file turnout-system.hpp
 ** THe actual state of a frame rendering evaluation parametrised for a single job.
 ** The rendering of frames is triggered from a render job, and recursively retrieves the data
 ** from predecessor render nodes, prepared, configured and interconnected by the Builder.
 ** Some statefull aspects can be involved into this recursive evaluation, beyond the data
 ** passed directly through the recursive calls and interconnected data buffers. Notably,
 ** some operations need direct call parameters, e.g. the frame number to retrieve or
 ** the actual parametrisation of an effect, which draws from _parameter automation._
 ** Moreover, when rendering interactively, parts of the render pipeline may be
 ** changed dynamically by mute toggles or selecting an output in the viever's
 ** _Switch Board.
 ** 
 ** The TurnoutSystem is related to the actual incidence and is created dynamically,
 ** while connecting to all the pre-existing \ref Turnout elements, sitting in the ports
 ** of those render nodes touched by the actual render invocation. It acts as mediator and
 ** data exchange hub, while gearing up the actual invocation to cause calculation of media data
 ** in the render nodes connected below, passing coordination parameters alongside.
 ** @todo WIP-WIP-WIP 12/2024 now combining the draft from 2009 / 2012 with recent engine development
 */


#ifndef STEAM_ENGINE_TURNOUT_SYSTEM_H
#define STEAM_ENGINE_TURNOUT_SYSTEM_H


#include "steam/engine/state-closure.hpp" /////////////////////OOO will take on a different role (if any)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Rebuild the Node Invocation
#include "lib/nocopy.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/hetero-data.hpp"


namespace steam  {
namespace engine {
  
  using lib::time::Time;
  
  template<typename VAL>
  class ParamStorageFrame
    : util::NonCopyable
    {
      
    };


  /**
   * Communication hub to coordinate and activate the »Render Node Network« performance.
   * An instance will be created on the stack for each evaluation of a [render job](\ref RenderInvocation).
   * It provides access to common invocation parameters, an extension system to register further _data slots,_
   * and initiates the recursive pull()-call into the render node network as attached for this call.
   */
  class TurnoutSystem
    {
      /////////////////////////////////////////////////////////OOO Storage: ich brauche Overflow-Buckets. KISS ==> erst mal intrusive linked List
    public:
      TurnoutSystem (Time absoluteNominalTime)
        { }
      // this is a copyable front-end object
    };
    
    
}} // namespace steam::engine
#endif /*STEAM_ENGINE_TURNOUT_SYSTEM_H*/
