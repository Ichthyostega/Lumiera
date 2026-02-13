/*
  NAIVE-BUFFER_SETUP.hpp  -  plain heap allocating BufferProvider implementation for tests

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file naive-buffer-setup.hpp
 ** Dummy implementation of the BufferProvider interface to support writing unit tests.
 ** This setup for a BufferProvider is notably straight forward and basically allocates
 ** a new heap block for any buffer, without deallocating memory ever. This allows to
 ** attach additional tracking status flags for each allocated block and validate
 ** allocator behaviour after the fact.
 ** 
 ** @see DiagnosticOutputSlot
 ** @see DiagnosticBufferProvider
 ** @see buffer-provider-protocol-test.cpp
 ** @see render-environment.cpp
 */

#ifndef STEAM_ENGINE_NAIVE_BUFFER_STEUP_H
#define STEAM_ENGINE_NAIVE_BUFFER_STEUP_H


#include "lib/error.hpp"
#include "steam/engine/buffer-provider-setup.hpp"
#include "steam/engine/heap-mem-buffer-store.hpp"

#include <memory>


namespace steam {
namespace engine {
  
  namespace error = lumiera::error;
  
  
  /**
   * simple BufferProvider implementation with additional allocation tracking.
   * @internal used as PImpl by DiagnosticBufferProvider and DiagnosticOutputSlot.
   * 
   * This dummy implementation of the BufferProvider interface uses a linearly growing
   * table of heap allocated buffer blocks, which will never be discarded, unless the object
   * is discarded as a whole. There is an additional testing/diagnostics API to access the
   * tracked usage information, even when blocks are already marked as "released".
   */
  class NaiveBufferSetup
    : public BufferProviderSetup
    {
    public:
      NaiveBufferSetup()
        : BufferProviderSetup{*this}
        { }
        
        auto buildStage() { return std::make_unique<Stage> ("Naive_HeapAllocated"); }
        auto buildStore() { return std::make_unique<HeapMemBufferStore>(); }   /////////////////////////////////TICKET #1410 : turn into a subclass of BufferSetup::Store and push down implementation
    };
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_NAIVE_BUFFER_STEUP_H*/
