/*
  ENGINE-BUFFER-MANAGER.hpp  -  central buffer allocation coordinator

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file engine-buffer-manager.hpp
 ** Central hub of buffer management within the Lumiera Render Engine.
 ** 
 ** @todo WIP-WIP-WIP 4/2026
 ** 
 ** @see buffer-provider.hpp
 ** @see LocalBufferStore_test
 ** @see BufferProviderProtocol_test
 */

#ifndef VAULT_MEM_ENGINE_BUFFR_MANAGA_H
#define VAULT_MEM_ENGINE_BUFFR_MANAGA_H


#include "lib/error.hpp"
#include "lib/hash-value.h"
#include "include/logging.h"
#include "vault/mem/buffhandle.hpp"
#include "vault/mem/type-handler.hpp"
#include "vault/mem/buffer-local-tag.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <unordered_map>


namespace vault{
namespace mem {
  
  using lib::HashVal;
  using util::unConst;
  
  namespace err = lumiera::error;
  
 
  
  
  /** */

  
  
  
  namespace { // internal blah
    
  }
  
  
  
  /* === deaf === */
  
  
  
  
}} // namespace vault::mem
#endif /*VAULT_MEM_ENGINE_BUFFR_MANAGA_H*/
