/*
  RENDER-OUTPUT-CTX.hpp  -  output control parameters for rendering

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file render-output-ctx.hpp
 ** A configuration context with output control parameters for render processing.
 ** This context parameter store is associated with the [calculation streams](\ref CalcStream)
 ** and accessed from the [render node invocation](\ref RenderInvocation) to retrieve additional
 ** and context dependent settings for aspects beyond the automated parameters and the actual
 ** render function. Notably the output DataSink is abstracted and the corresponding target
 ** data buffer is exposed through this context.
 ** @todo »Playback Vertical Slice« -- effort towards first integration of render process ///////////////////TICKET #1221
 ** @todo 6/2026 still not entirely decided regarding the organisation of Render Job parameters;
 **       introducing such a context seems a good way to retain some limited flexibility while
 **       keeping matters focused.
 */


#ifndef STEAM_ENGINE_RENDER_OUTPUT_CTX_H
#define STEAM_ENGINE_RENDER_OUTPUT_CTX_H

#include "steam/common.hpp"
#include "vault/mem/buffhandle.hpp"
#include "vault/out/output-slot.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/nocopy.hpp"


namespace steam {
namespace engine{
  
  using lib::time::Time;
  using vault::mem::BuffHandle;
  
  
  
  /**
   * Render output control parameters and configuration details.
   */
  class RenderOutputCtx
    : util::MoveOnly
    {
      using Output = vault::out::OutputSlot::DataSink;
      Output sink_;
      
    public:
      BuffHandle getOutputBuffer (Time absoluteNominalTime);
    };
  
  
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_RENDER_OUTPUT_CTX_H*/
