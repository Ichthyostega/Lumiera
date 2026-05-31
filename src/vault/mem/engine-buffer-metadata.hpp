/*
  ENGINE-BUFFER-METADATA.hpp  -  central buffer metadata registry

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file engine-buffer-metadata.hpp
 ** Coordination of buffer type registration at Render Engine level.
 ** Within the Render Node Network, access to buffer management is abstracted in the form
 ** of [Buffer Descriptors](\ref BuffDescr) and [Buffer Handles](\ref BuffHandle). And since
 ** processing of media data is organised into jobs that are performed by worker(threads),
 ** the necessity arises to handle parts of the buffer allocation and coordination locally,
 ** within thread-local »satellite« services — which implies there must be some central hub
 ** to exchange and coordinate metadata globally. Data processing within the Engine requires
 ** all kind of different buffer sizes and memory layout, and so this diversity is abstracted
 ** into the notion of a **Buffer Type**. The corresponding metadata entry defines, first and
 ** foremost, the _size_ of the allocation. And, in addition, it _can_ define a TypeHandler
 ** with a constructor- and destructor functor, to allow automated setup of specialised layout
 ** within the buffer allocation. Furthermore, a LocalTag can be added that provides additional
 ** implementation level instructions to the underlying memory manager.
 ** 
 ** # Setup
 ** 
 ** The EngineBufferMetadata hub is a central service, to be accessed through dependency-injection.
 ** A single service instance will be created as part of booting up the Render Engine. At this point,
 ** a corresponding setup for the BufferProvider is established, which relies on thread-local parts
 ** within the implementation backend services. Notable the BufferProvider::BufferStage implementation,
 ** which is provided by class LocalBufferStage, establishes a buffer metadata registry table for each
 ** worker thread; under specific and well defined conditions, at certain points in the buffer lifecycle
 ** a synchronisation must be made with the central metadata hub, which also involves a global lock to
 ** prevent data corruption by concurrent access, since at those synchronisation points, a complete
 ** exchange of metadata must be ensured, and completed in a single synchronous call. These data
 ** synchronisation activities are initiated by the thread-local BufferMetadata tables, and the
 ** implementation will rely on lib::Depend<EngineBufferMetadata> for access.
 ** 
 ** @todo WIP-WIP 4/2026 prototyping to work towards a buffer management scheme.
 ** 
 ** @see LocalBufferStage_test
 ** @see BufferMetadata_test
 ** @see buffer-provider.hpp
 ** @see BufferProviderProtocol_test
 */

#ifndef VAULT_MEM_ENGINE_BUFFR_METADAZ_H
#define VAULT_MEM_ENGINE_BUFFR_METADAZ_H


#include "lib/error.hpp"
//#include "lib/hash-value.h"
#include "include/logging.h"
#include "vault/mem/buffer-metadata.hpp"
#include "lib/nocopy.hpp"
#include "lib/sync.hpp"
#include "lib/util.hpp"

#include <utility>

namespace vault{
namespace mem {
//  namespace err = lumiera::error;
  
  using std::move;
  using util::unConst;
  
  using lib::Sync;
  using lib::RecursiveLock_NoWait;
  
  
  
  
  
  /**
   * Exchange hub for buffer metadata in the Render Engine.
   * @note all access is threadsafe (protected by mutex)
   * @remark in the Render Engine, the bulk of metadata handling
   *         happens in the worker threads, using LocalBufferStage.
   */
  class EngineBufferMetadata
    : util::NonCopyable
    , public Sync<RecursiveLock_NoWait>
    {
      BufferMetadata registry_;
      
    public:
      using Key   = metadata::Key;
      using Entry = metadata::Entry;
      
      EngineBufferMetadata()
        : registry_{}
        { }
      
      
      size_t
      cntEntries()  const
        {
          return registry_.cntEntries();
        }
      
      bool
      isKnown (HashVal metaID)  const
        {
          Lock sync{this};
          return registry_.isKnown (metaID);
        }
      
      bool
      isAllotted (HashVal metaID)  const
        {
          Lock sync{this};
          return registry_.isLocked (metaID);
        }
      
      bool
      isAccessible (HashVal metaID)  const
        {
          Lock sync{this};
          return registry_.isAccessible (metaID);
        }
      
      Key const&
      lookup (HashVal metaID)  const
        {
          Lock sync{this};
          return isKnown (metaID)? registry_.get (metaID)
                                 : metadata::Key::INVALID;
        }
      
      Key const&
      defineBufferType (HashVal familyID
                       ,size_t buffSiz
                       ,TypeHandler handlerFunctions  =TypeHandler::RAW
                       ,LocalTag localTag             =LocalTag::UNKNOWN)
        {
          Lock sync{this};
          return lookup (registry_.key (familyID, buffSiz, move(handlerFunctions), localTag));
        }             // possibly create new entry, and return (stable) reference to it
      
      
      /** @warning potentially dangerous;
       *      must ensure that the entry is not used and registered in any worker thread!
       */
      bool
      abandon (HashVal metaID, bool invokeDtor =true)
        {
          Lock sync{this};
          Key const& key = lookup (metaID);
          Entry& metaEntry = static_cast<Entry&> (unConst(key));
          if (metaEntry.isTypeKey())
            return false;
          metaEntry.invalidate (invokeDtor);
          registry_.release (metaEntry);
          return true;
        }
      
      
      /** push a new metadata chain from the local registry into the central hub.
       * @remark typically required after defining a new buffer type locally */
      void
      propagateUp (HashVal metaID, BufferMetadata const& localRegistry)
        {
          Lock sync{this};
          registry_.import(metaID, localRegistry);
        }
      
      /** retrieve a metadata chain from the central registry */
      void
      propagateDown (HashVal metaID, BufferMetadata& localRegistry)  const
        {
          Lock sync{this};
          localRegistry.import(metaID, registry_);
        }
    };
  
  
}} // namespace vault::mem
#endif /*VAULT_MEM_ENGINE_BUFFR_METADAZ_H*/
