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
 ** The active core within a CalcStream, causing the render mechanism to re-trigger repeatedly.
 ** Rendering is seen as an open-ended, ongoing process, and thus the management and planning
 ** of the render process itself is performed chunk wise and embedded into the other rendering
 ** calculations. The _"rendering-as-it-is-planned-right-now"_ can be represented as a closure
 ** to the jobs, which perform and update this plan on the go. And in fact, the head of the
 ** planning process, the CalcStream, maintains this closure instance, as parametrised
 ** with the appropriate configuration for the specific playback/render process underway.
 ** Enclosed into this instance lives the actual job planning pipeline, connected at the
 ** rear to the dispatcher and thus to the fixture and the low-level model
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
namespace engine {
  
  
  
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
