/*
  EnginePufferManager  -  central buffer allocation coordinator

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file engine-buffer-manager.cpp
 ** Implementation of render buffer allocation for the Lumiera Engine.
 ** Instantiated as global service and part of the Engine, this global
 ** buffer coordinator uses the [actual allocator](\ref engine-buffer-allocator.hpp)
 ** internally to maintain a pool of usage buffer memory blocks. Furthermore,
 ** both a synchronous and an asynchronous interface is provided to request
 ** allocations, that will be sent as \ref Alloc entries to some local memory
 ** pool, that is an \ref AllocReceiver, thereby passing control over these
 ** blocks of memory. The contract is that the local memory user has to
 ** return those blocks reliably to the EngineBuffrerManager, which
 ** itself is also an \ref AllocReceiver.
 ** 
 ** @see EnginePufferManager_test
 ** @see LocalBufferStore_test
 */


#include "lib/error.hpp"
#include "lib/util-quant.hpp"
#include "lib/allocator-handle.hpp"
//#include "lib/hash-value.h"
#include "vault/mem/buffhandle.hpp"
#include "vault/mem/engine-buffer-manager.hpp"
#include "vault/mem/engine-buffer-allocator.hpp"
//#include "vault/mem/buffer-local-tag.hpp"
#include "lib/nocopy.hpp"
//#include "lib/util.hpp"

#include <memory>
#include <array>


namespace vault{
namespace mem {
  
//  using lib::HashVal;
//  using util::unConst;
  
  namespace err = lumiera::error;
  
  using std::byte;
  
  
  namespace { // config and internal helpers...
    
  }
  
  
  
  
  
  using BufferAllocator = HeapBufferAllocator;
  
  
  
  
  /* === EngineBufferManager implementation === */
  
  void
  EngineBufferManager::handleAllocRequests()
  {
    requestQueue_.consume_all ([this](AllocRequest request)
                                {
                                  REQUIRE (request.receiver);
                                  BufferAllocator allocator;
                                  Alloc alloc = allocator.allocate (request.sizRequest);
                                  ENSURE (alloc.mem);
                                  request.receiver->supply (alloc);
                                });
  }
  
  
}} // namespace vault::mem
