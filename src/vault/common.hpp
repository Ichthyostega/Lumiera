/*
  COMMON.hpp  -  common types and definitions for the Vault-Layer
 

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file vault/common.hpp
 ** Basic set of definitions and includes commonly used together (Vault).
 ** Including common.hpp gives you a common set of elementary declarations
 ** widely used within the C++ code of the Vault-Layer. Besides that, this
 ** header is used to attach the doxygen documentation comments for all
 ** the primary Vault-Layer namespaces
 ** 
 ** @see main.cpp
 ** 
 */



#ifndef VAULT_COMMON_H
#define VAULT_COMMON_H




/* =====  frequently used common types ===== */

#include "lib/error.hpp"   // pulls in NoBug via loggging.h
#include "lib/nocopy.hpp"
#include "lib/symbol.hpp"
#include "lib/time/timevalue.hpp"



/**
 * Vault-Layer implementation namespace root.
 * The lowest layer in Lumiera's architecture provides the interface to
 * the operating system and similar fundamental high-performance services:
 * - Scheduler
 * - Frame Cache
 * - I/O support
 * - persistent storage format
 * - specialised memory management
 * @ingroup vault
 */
namespace vault {
  
  
  /**
   * I/O subsystem and support.
   * System level and platform services.
   */
  namespace io { }
  
  
  /**
   * Output management and connectors.
   */
  namespace out { }
  
  
  /**
   * Memory Management.
   */
  namespace mem { }
  
  
  /**
   * Active working gear and plumbing.
   * Notably the Scheduler and performance observation and self-regulation are implemented here.
   */
  namespace gear { }
  
  
  /**
   * The frame and media cache.
   */
  namespace cache { }
  
  
  /**
   * Lumiera Render Toolkit.
   * This is a set of adapters and thin wrappers on top of external implementation,
   * to provide some commonplace generic functionality in an abstracted way for use
   * from the actual render jobs.
   */
  namespace toolkit { }
  
  
  /**
   * Persistent storage formats.
   * Support for structured data storage and exchange, for session and command log.
   * Import / Export and metadata handling
   */
  namespace storage { }
  
} //(End)namespace steam

#endif /*VAULT_COMMON_H*/
