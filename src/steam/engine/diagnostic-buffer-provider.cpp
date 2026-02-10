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
  
  
  DiagnosticBufferProvider::DiagnosticBufferProvider()
    : NaiveBufferSetup{}
    , heapStore_{dynamic_cast<HeapMemProvider&> (*bufferStore_)}
    { }
  
  DiagnosticBufferProvider::~DiagnosticBufferProvider()
    {
//    INFO (proc_mem, "discarding %zu diagnostic buffer entries", outSeq_.size());   ////////////OOO implement based on dedicated tracking information in-object
    }
  
  
  
  
  /* === diagnostic API === */
  
  bool
  BufferDiagnostic::buffer_was_used (uint bufferID)
    {
      return dbp_.heapStore_.access_emitted(bufferID).was_used();
    }
  
  
  bool
  BufferDiagnostic::buffer_was_closed (uint bufferID)
    {
      return dbp_.heapStore_.access_emitted(bufferID).was_closed();
    }
  
  
  void*
  BufferDiagnostic::accessMemory (uint bufferID)
    {
      return dbp_.heapStore_.access_emitted(bufferID).accessMemory();
    }
  

}} // namespace engine
