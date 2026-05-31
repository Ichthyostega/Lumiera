/*
  ENGINE-BUFFER-ALLOCATOR.hpp  -  global buffer allocation scheme

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file engine-buffer-allocator.hpp
 ** Implementation of render buffer allocation for the Lumiera Engine.
 ** Memory for render buffer is allocated in blocks with a size aligned
 ** to the typical cache-line granularity. These allocations are maintained
 ** in thread-local pools and re-used to the extent possible. Excess blocks
 ** are sent back and collected by the EngineBufferManager, which serves
 ** thus as an background collector and manager for the buffer memory.
 ** 
 ** @todo WIP 4/2026 The intention is to use a family of pools eventually,
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

#ifndef VAULT_MEM_ENGINE_BUFFER_ALLOCATOR_H
#define VAULT_MEM_ENGINE_BUFFER_ALLOCATOR_H

#include "lib/error.hpp"
#include "lib/util-quant.hpp"
#include "lib/allocator-handle.hpp"
#include "vault/mem/engine-buffer-manager.hpp"
#include "lib/nocopy.hpp"

#include <memory>
#include <array>


namespace vault{
namespace mem {
  namespace err = lumiera::error;
  
  using std::byte;
  
  
  
  /* === Setup of »naïve« buffer allocation === */
  
  /***********************************************************//**
   * A placeholder definition for the real buffer allocator:
   * - allocates buffers aligned to a fixed #TILE_SIZ
   * - passes any allocation request directly to the heap allocator
   * - performs no state tracking and cleanup whatsoever.
   */
  struct HeapBufferAllocator
    {
      static constexpr size_t TILE_SIZ = 64;
      using BaseTile = std::array<byte, TILE_SIZ>;
      
      using Allo = std::allocator<BaseTile>;
      using AlloT = std::allocator_traits<Allo>;
      
      static_assert (util::isPow2 (sizeof(BaseTile)));
      static_assert (lib::allo::is_Stateless_v<Allo>);
      
      
      static constexpr size_t
      numTiles (size_t sizRequest)
        {
          return util::ceilDiv (sizRequest, sizeof(BaseTile));
        }
      
      static Buff*
      mark_as_Buffer (void* mem)
      {
        return static_cast<Buff*> (mem);
      }
      
      
      /**
       * Allocate continuous memory block large enough to fit.
       * @throws std::bad_alloc when allocation fails
       * @note client is responsible to \ref deallocate()
       */
      Alloc
      allocate (size_t sizRequest)
        {
          Allo allo;
          auto cnt = numTiles (sizRequest);
          auto loc = AlloT::allocate (allo, cnt);
          return {mark_as_Buffer(loc)
                 ,cnt * TILE_SIZ
                 };
        }
      
      /**
       * Deallocates a memory block that was previously
       * [allocated](\ref allocate()) by this buffer allocator.
       */
      void
      deallocate (Alloc alloc)
        {
          Allo allo;
          auto cnt = numTiles (alloc.siz);
          auto mem = reinterpret_cast<BaseTile*> (alloc.mem);
          AlloT::deallocate (allo, mem, cnt);
        }
    };
  
  
  
}} // namespace vault::mem
#endif /*VAULT_MEM_ENGINE_BUFFER_ALLOCATOR_H*/
