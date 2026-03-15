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

#ifndef VAULT_MEM_BUFFER_PROXY_ADAPTOR_H
#define VAULT_MEM_BUFFER_PROXY_ADAPTOR_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/symbol.hpp"
#include "lib/meta/util.hpp"
#include "vault/mem/buffer-provider.hpp"
#include "vault/mem/buffer-provider-setup.hpp"
#include "vault/mem/heap-mem-buffer-store.hpp"
#include "vault/mem/simple-buffer-state-registry.hpp"

#include <functional>
#include <utility>


namespace vault{
namespace mem  {
  
  using lib::Literal;
  using std::move;
  
  
  
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
  template<class CONF>
  class BufferProxyAdaptor
    : util::NonCopyable
    {
      
      class ProxyBufferStore
        : public BufferProviderSetup::Store
        , private CONF
        {
          
          Buff*
          asBuffer (LocalTag targetMarker)
            {
              void* buffMem{targetMarker};
              return static_cast<Buff*> (buffMem);
            }
          
          uint
          prepareBuffers (HashVal,uint,size_t)  override
            {
              WARN (engine, "Announce invoked on a Proxy Buffer Provider; "
                            "Implies misuse as generic BufferProvider");
              return 1;
            }
          
          BuffAlloc
          provideBuffer (HashVal,size_t siz,LocalTag targetMarker)  override
            {
              CONF::on_lock();
              return {asBuffer(targetMarker), siz, targetMarker};
            }
          
          void
          mark_emitted (HashVal, BuffAlloc storageSlot)  override
            {
              CONF::on_emit();
            }
          
          void
          detachBuffer (HashVal, BuffAlloc storageSlot)  override
            {
              CONF::on_release();
            }
          
        public:
          ProxyBufferStore (CONF policy)
            : CONF{move(policy)}
            { }
        };
      
      struct Setup
        : CONF
        {
          auto buildStage() { return std::make_unique<SimpleBufferStateRegistry>("ResourceProxy"); }
          auto buildStore() { return std::make_unique<ProxyBufferStore> (move(*this)); }
        };                                                           //  Note: possible since Setup does not use CONF
      
      class PassThroughBufferProvider
        : public BufferProviderSetup
        {
          
        public:
          PassThroughBufferProvider (Setup setup)
            : BufferProviderSetup{setup}
            { }
            
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
      BufferProxyAdaptor(CONF policy)
        : proxyProvider_{Setup{move(policy)}}
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
  
  
}} // namespace vault::mem
#endif /*VAULT_MEM_BUFFER_PROXY_ADAPTOR_H*/
