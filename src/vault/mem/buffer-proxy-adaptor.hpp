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
 ** through the buffer lifecycle protocol as defined by BufferProvider.
 ** @remark 3/2026 this solution was developed as pathfinder to resolve a mismatch
 **         between the »Buffer Provider Protocol« and the »Output Slot Protocol«.
 **         Ultimately, however, it was not used; instead, a similar, specially
 **         tailored construct was directly integrated.
 ** @see output-proxy-provider-test.cpp
 ** @see output-buffer-proxy.hpp
 ** @see output-slot.hpp
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
   * Adapter to expose controlled access to some memory resource
   * through a [»Buffer Type«](\ref BuffDescr) and a BuffHandle as front-end,
   * in accordance with the BufferProvider protocol.
   * 
   * This template is an implementation building block and needs to be instantiated
   * with a policy or configuration to define the flexible parts of the behaviour
   * - what a Buffer Type means
   * - is there only one resource or is this setup statefull?
   * - callback functors for the lifecycle stages related to the client's access
   * @todo 3/2026 this is prototyping code and was retained for demonstration purposes.
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
          provideBuffer (HashVal,size_t siz,LocalTag targetMarker, int64_t customArg)  override
            {
              BuffAlloc storageSlot{asBuffer(targetMarker), siz, targetMarker};
              CONF::on_lock (storageSlot);
              return storageSlot;
            }
          
          void
          mark_emitted (HashVal, BuffAlloc storageSlot)  override
            {
              CONF::on_emit (storageSlot);
            }
          
          void
          detachBuffer (HashVal, BuffAlloc storageSlot)  override
            {
              CONF::on_release (storageSlot);
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
      BuffDescr
      getDescriptorFor (TAR& dataBlock)
        {
          return proxyProvider_.registerBuffer(&dataBlock, sizeof(TAR));
        }
      
      template<typename TAR>
      BuffHandle
      lockBuffer (TAR& dataBlock)
        {
          BuffDescr buffType = getDescriptorFor<TAR> (dataBlock);
          ENSURE (buffType.isValid());
          return buffType.lockBuffer();
        }
    };
  
  
}} // namespace vault::mem
#endif /*VAULT_MEM_BUFFER_PROXY_ADAPTOR_H*/
