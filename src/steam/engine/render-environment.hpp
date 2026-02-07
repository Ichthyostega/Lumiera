/*
  RENDER-ENVIRONMENT.hpp  -  setup of the Lumiera Render Engine

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file render-environment.hpp
 ** A configuration context for processing within the Render Engine.
 ** This context can be accessed from within the ongoing render processes to query
 ** strategy settings; actually, all the global support facilities within the Render Engine
 ** are attached here. Thus it might also be used to control the lifecycle (2026 not clear yet)
 ** @todo 2026 the concept is quite old, but I have reestablished it now to use it as an anchor.
 **       it is not clear yet if there will be several sub environments for different playback
 **       configurations; effectively, this is a conceptual placeholder now and needs to be
 **       put into actual use to see where this leads us to....  ////////////////////////////////////////////TICKET #1409
 ** 
 ** @todo 4/2023 »Playback Vertical Slice« -- effort towards first integration of render process ////////////TICKET #1221
 */


#ifndef STEAM_ENGINE_RENDER_ENVIRONMENT_H
#define STEAM_ENGINE_RENDER_ENVIRONMENT_H

#include "steam/common.hpp"
#include "steam/engine/dispatcher.hpp"
#include "steam/play/timings.hpp"
//#include "lib/nocopy.hpp"


namespace steam {
namespace engine{
  
  
  
  /**
   * Abstract definition of the environment
   * hosting a given render activity (CalcStream).
   * Exposes all the operations necessary to adjust the
   * runtime behaviour of the render activity, like e.g.
   * re-scheduling with modified playback speed. Since the
   * CalcStream is an conceptual representation of "the rendering",
   * the actual engine implementation is kept opaque this way.
   */
  class RenderEnvironment
    {
    public:
      virtual ~RenderEnvironment() { }   ///< this is an interface
      
      virtual play::Timings& effectiveTimings()   =0;
      virtual Dispatcher&    getDispatcher()      =0;
    };
  
  
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_RENDER_ENVIRONMENT_H*/
