/*
  DISPATCH-TABLE.hpp  -  implementation of frame job creation

   Copyright (C)
     2012,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file dispatch-table.hpp
 ** Implementation details of render job generation.
 ** @todo 6/2023 »PlaybackVerticalSlice« : completing and integrating the core engine step by step
 */


#ifndef STEAM_ENGINE_DISPATCH_TABLE_H
#define STEAM_ENGINE_DISPATCH_TABLE_H

#include "steam/common.hpp"
#include "steam/engine/dispatcher.hpp"



namespace steam {
namespace engine {
  
  using lib::time::TimeSpan;
  using lib::time::FSecs;
  using lib::time::Time;
//  
//  class ExitNode;
  
  /**
   * @todo 6/2023 gradually building up the core engine components...
   */
  class DispatchTable
    : public Dispatcher
    {
      
      /* ==== Dispatcher interface ==== */
      
      size_t     resolveModelPort (ModelPort)                      override;
      JobTicket& getJobTicketFor  (size_t, TimeValue nominalTime)  override;
      
    protected:
      /** timerange covered by this RenderGraph */
      TimeSpan segment_;
      
    public:
      DispatchTable()
        : segment_(Time::ZERO, FSecs(5))
        {
          UNIMPLEMENTED ("anything regarding the Engine backbone");
        }
      
    };
  
  
  
}} // namespace steam::engine
#endif
