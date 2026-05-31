/*
  HEAP-MEM-BUFFER_STORE.hpp  -  plain heap allocating BufferProvider implementation for tests

   Copyright (C)
     2011,2026        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file heap-mem-provider.hpp
 ** Dummy engine::BufferProvider BufferStore implementation, for sake of conceptual
 ** demonstration and unit testing. Notably this buffer allocator implementation is straight forward,
 ** if not outright silly: it happily claims more and more heap blocks and never releases any memory
 ** dynamically. This both demonstrates the simplest possible implementation of storage handling, and
 ** allows to investigate additional tracking status flags for each allocated block after the fact.
 ** 
 ** @see DiagnosticOutputSlot
 ** @see DiagnosticBufferProvider
 ** @see buffer-provider-protocol-test.cpp
 ** @see engine-ctx.hpp
 ** @see naive-buffer-setup.hpp
 */

#ifndef VAULT_MEM_HEAP_MEM_BUFFER_STORE_H
#define VAULT_MEM_HEAP_MEM_BUFFER_STORE_H


#include "vault/mem/buffer-provider-setup.hpp"

#include <unordered_map>


namespace vault {
namespace mem   {
  
  
  /**
   * Naive demo implementation of the BufferProvider::BufferStore.
   * This is kind of a faked implementation of the BufferProvider backend and uses a
   * linearly growing table of heap allocated buffer blocks, which will never be discarded,
   * unless the object is discarded as a whole. Paired with the instrumentation and diagnostics
   * API of DiagnosticBufferProvider, this can be used to confirm assumptions regarding
   * buffer usage from a unit test setup. Even blocks already marked as "released"
   * are actually retained and can be investigated after the fact.
   */
  class HeapMemBufferStore
    : public BufferProviderSetup::Store
    {
      class Alloc;
      using Index = std::unordered_map<Buff*,Alloc>;
      
      Index allocIdx_;
      
      
      /* === BufferStore interface === */
      
      uint prepareBuffers (HashVal typeID, uint cnt, size_t)    override;
      BuffAlloc provideBuffer (HashVal,size_t,LocalTag,int64_t) override;
      void mark_emitted (HashVal typeID, BuffAlloc) override;
      void detachBuffer (HashVal typeID, BuffAlloc) override;
      
    public:
     ~HeapMemBufferStore();
      HeapMemBufferStore();
    };
  
  
  
}} // namespace vault::mem
#endif /*VAULT_MEM_HEAP_MEM_BUFFER_STORE_H*/
