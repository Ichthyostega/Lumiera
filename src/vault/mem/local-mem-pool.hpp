/*
  LOCAL-MEM-POOL.hpp  -  working pool of buffer allocations for local reuse

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file local-mem-pool.hpp
 ** A local cache of buffer allocations to facilitate reuse of memory in a worker.
 ** 
 ** @see local-mem-pool-test.cpp
 ** @see local-buffer-store.hpp
 ** @see buffer-provider.hpp
 ** @see buffer-provider-protocol-test.cpp
 ** @see simple-buffer-state-registry.hpp
 */

#ifndef VAULT_MEM_LOCAL_MEM_POOL_H
#define VAULT_MEM_LOCAL_MEM_POOL_H


//#include "vault/mem/buffer-provider-setup.hpp"
//#include "vault/mem/engine-buffer-metadata.hpp"
//#include "vault/mem/buffer-metadata.hpp"
#include "vault/mem/buffhandle.hpp" //////////////TODO
//#include "lib/local-slice.hpp"
//#include "lib/depend.hpp"


namespace vault {
namespace mem   {
  
  
  
  /**
   * Low-level Building block for Render Engine memory management.
   */
  class LocalMemPool
    {
      
    public:
      LocalMemPool()
        { }
    };
  
  
  
}} // namespace vault::mem
#endif /*VAULT_MEM_LOCAL_MEM_POOL_H*/
