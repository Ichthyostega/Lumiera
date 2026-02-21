/*
  BUFFER-PROXY-ADAPTOR.hpp  -  Adaptor to access existing allocation via buffer handling protocol

   Copyright (C)
     2024,2026        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file buffer-proxy-adaptor.hpp
 ** Adapter to expose a given memory block through a BuffHandle.
 ** This implementation building-block allows to integrate some specific data access
 ** (e.g. related to input / output) through the buffer lifecycle protocol as defined by BufferProvider. 
 ** @todo BROKEN as of 12/2024 //////////////////////////////////////////////////////////////////////////////TICKET #1387 : can not properly compose BufferProvider
 ** @see output-slot.hpp
 ** @see output-proxy-provider-test.cpp
 */

#ifndef STEAM_ENGINE_BUFFER_PROXY_ADAPTOR_H
#define STEAM_ENGINE_BUFFER_PROXY_ADAPTOR_H


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/meta/util.hpp"
//#include "lib/hash-value.h"
#include "steam/engine/buffer-provider.hpp"
#include "steam/engine/buffer-provider-setup.hpp"
#include "steam/engine/heap-mem-buffer-store.hpp"
#include "steam/engine/simple-buffer-state-registry.hpp"
//#include "steam/engine/engine-ctx.hpp"
//#include "steam/engine/type-handler.hpp"
//#include "steam/engine/buffer-local-tag.hpp"
#include "lib/nocopy.hpp"

#include <functional>
#include <utility>
//#include <memory>


namespace steam {
namespace engine {
  
  using lib::Literal;
//  using std::unique_ptr;
//  using std::forward;
  
  
  
  
  /**
   * Adapter to expose access controlled access to some memory resource
   * through a [»Buffer Type«](\ref BuffDescr) and a BuffHandle as front-end,
   * in accordance with the BufferProvider protocol.
   * 
   * This template is an implementation building block and needs to be instantiated
   * with a policy or configuration to define the flexible parts of the behaviour
   * - what a Buffer Type means
   * - is there only one resource or is this setup statefull?
   * - callback functors for the lifecycle stages related to the client's access
   * @todo WIP-WIP 2/2026 used as prototype to forge a path ahead to resolve
   *       the structural problems with OutputSlot vs. BufferProvider   ///////////////////////////////////////TICKET #1415 : prototyping to resolve structural mismatch between OutputSlot / DataSink / BufferProvider
   */
  class BufferProxyAdaptor
    : util::NonCopyable
    {
      
      class PreRiggedBufferStage
        : public SimpleBufferStateRegistry
        {
        public:
          PreRiggedBufferStage()
            : SimpleBufferStateRegistry{"ResourceProxy"}
            { }
        };
      
      class ProxyBufferStore
        : public BufferProviderSetup::Store
        {
          Buff*
          asBuffer (LocalTag targetMarker)
            {
              void* buffMem{targetMarker};
              return static_cast<Buff*> (buffMem);
            }
          
          uint
          prepareBuffers (uint,size_t,HashVal)  override
            {
              WARN (engine, "Announce invoked on a Proxy Buffer Provider; "
                            "Implies misuse as generic BufferProvider");
              return 1;
            }
          
          Slot
          provideBuffer (size_t,HashVal, LocalTag targetMarker)  override
            {
              TODO ("invoke LOCK callback");
              return {asBuffer(targetMarker), targetMarker};
            }
          
          void
          mark_emitted (size_t,HashVal, LocalTag const&)  override
            {
              TODO ("invoke EMIT callback");
            }
          
          void
          detachBuffer (size_t,HashVal,Slot)  override
            {
              TODO ("invoke RELEASE callback");
            }
          
        };
      
      class PassThroughBufferProvider
        : public BufferProviderSetup
        {
            
        public:
          PassThroughBufferProvider ()
            : BufferProviderSetup{*this}
            { }
            
            auto buildStage() { return std::make_unique<PreRiggedBufferStage>(); }
            auto buildStore() { return std::make_unique<ProxyBufferStore>(); }
            
            BuffDescr
            registerBuffer (void* buff, size_t siz)
              {
                REQUIRE (siz);
                REQUIRE (buff);
                auto& typeKey = bufferStage_->defineBufferType (siz, TypeHandler::RAW, LocalTag(buff));
                return buildDescriptor (typeKey);
              }
        };
      
      PassThroughBufferProvider proxyProvider_;
      
      
    public:
      BufferProxyAdaptor()
        : proxyProvider_{}
        { }
      
      template<typename TAR>
      BuffHandle
      lockBuffer (TAR& dataBlock)
        {
          BuffDescr buffType = proxyProvider_.registerBuffer(&dataBlock, sizeof(TAR));
          ENSURE (buffType.isValid());
          return buffType.lockBuffer();
        }
      
    };
  
  
  
  
  /* === Implementation === */
  
  /** convenience shortcut: */
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_BUFFER_PROXY_ADAPTOR_H*/
