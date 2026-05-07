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
 ** allocation coordinator is responsible to provide pre-allocated blocks
 ** for render buffers. These allocations are categorised by their size,
 ** and will be sent down to LocalMemPool instances running in each
 ** worker thread. This involves asynchronous messaging to avoid
 ** any blocking of work jobs due to central resource management.
 ** 
 ** @todo WIP-WIP 4/2026 The intention is to use a family of pools eventually,
 **       with some of the most relevant tile sizes. The EngineBufferManager
 **       would then actually retain ownership of all allocations and just
 **       _lease_ them to the LocalMemPool instances. Allocations would be
 **       added in extents, likely also using some usage bitmap. However
 **       building any such kind of optimised allocator requires to conduct
 **       very detailed _and_ realistic performance observations; furthermore
 **       it is likely that we'd need some dynamic scheme to select suitable
 **       tile sizes and pool dimensions. Not sure if it is even possible
 **       to improve performance by such a scheme; with some likelihood
 **       the majority of the benefits will be reaped already just by
 **       introducing thread-local pools without wasting too much memory.
 ** 
 ** @see EnginePufferManager_test
 ** @see LocalBufferStore_test
 ** @see BufferProviderProtocol_test
 ** @see buffer-provider.hpp
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
  
  
  
}} // namespace vault::mem
