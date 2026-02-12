/*
  DiagnosticBufferProvider  -  helper for testing against the BufferProvider interface

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file diagnostic-buffer-provider.cpp
 ** Implementation details of unit test support regarding internals of the render engine.
 ** 
 ** @todo WIP from 2013, half finished, stalled
 ** @see buffer-provider-protocol-test.cpp
 */


//#include "lib/error.hpp"
//#include "include/logging.h"
//#include "lib/meta/function.hpp"

#include "steam/engine/diagnostic-buffer-provider.hpp"
#include "steam/engine/heap-mem-buffer-store.hpp"


namespace steam {
namespace engine {
  
  /**
   * Implementation of tracking and instrumentation
   */
  class DiagnosticBufferProvider::BlockTracker
    {
      
    };
  
  
  DiagnosticBufferProvider::DiagnosticBufferProvider()
    : NaiveBufferSetup{}
    , heapStore_{dynamic_cast<HeapMemProvider&> (*bufferStore_)}
    , tracker_{std::make_unique<BlockTracker>()}
    { }
  
  DiagnosticBufferProvider::~DiagnosticBufferProvider()
    {
//    INFO (proc_mem, "discarding %zu diagnostic buffer entries", outSeq_.size());   ////////////OOO implement based on dedicated tracking information in-object
    }
  
  
  
  
  /* === diagnostic API === */
  
  bool
  BufferDiagnostic::buffer_was_used (uint bufferID)
    {
      NOTREACHED ("shall use new API only");
//    return dbp_.heapStore_.access_emitted(bufferID).was_used();  //////////////////////////////////////////TICKET 1410 : switch to newly defined tracking-API
    }
  
  
  bool
  BufferDiagnostic::buffer_was_closed (uint bufferID)
    {
      NOTREACHED ("shall use new API only");
//    return dbp_.heapStore_.access_emitted(bufferID).was_closed();//////////////////////////////////////////TICKET 1410 : switch to newly defined tracking-API
    }
  
  
  void*
  BufferDiagnostic::accessMemory (uint bufferID)
    {
      NOTREACHED ("shall use new API only");
//    return dbp_.heapStore_.access_emitted(bufferID).accessMemory(); ///////////////////////////////////////TICKET 1410 : switch to newly defined tracking-API
    }
  
  diagn::StateReg&
  BufferDiagnostic::created ()
    {
      UNIMPLEMENTED ("track all created buffer blocks");
    }
  
  diagn::StateReg&
  BufferDiagnostic::emitted ()
    {
      UNIMPLEMENTED ("track all emitted buffer blocks");
    }
  
  diagn::StateReg&
  BufferDiagnostic::released()
    {
      UNIMPLEMENTED ("track all released buffer blocks");
    }
  

  

}} // namespace engine
