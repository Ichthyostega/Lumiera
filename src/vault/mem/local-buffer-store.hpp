/*
  LOCAL-BUFFER-STORE.hpp  -  buffer memory handling with thread-local sub-services

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file local-buffer-store.hpp
 ** Production-grade implementation of engine::BufferProvider memory handling.
 **
 ** @see LocalBufferStore_test 
 ** @see buffer-provider.hpp
 ** @see buffer-provider-protocol-test.cpp
 ** @see simple-buffer-state-registry.hpp
 */

#ifndef VAULT_MEM_LOCAL_BUFFER_STORE_H
#define VAULT_MEM_LOCAL_BUFFER_STORE_H


#include "vault/mem/buffer-provider-setup.hpp"
#include "vault/mem/buffhandle.hpp" ///////////////TODO
#include "lib/local-slice.hpp"
#include "lib/depend.hpp"


namespace vault {
namespace mem   {
  
  
  
  /**
   * @todo type comment
   */
  class LocalBufferStore
    : public BufferProviderSetup::Store
    {
      
    public:
      LocalBufferStore()
        { }
    };
  
  
  
}} // namespace vault::mem
#endif /*VAULT_MEM_LOCAL_BUFFER_STORE_H*/
