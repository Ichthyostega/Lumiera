/*
  OUTPUT-BUFFER-PROXY.hpp  -  Adaptor to access existing allocation via buffer handling protocol

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file output-buffer-proxy.hpp
 ** Adapter to expose some buffer memory through a BuffHandle as front-end.
 ** This implementation building-block allows to integrate the output buffers
 ** managed by a specific output mechanism with the generic implementation of
 ** OutputSlote, so that only a BuffHandle needs to be exposed to the client.
 ** The client uses this setup in accordance to the »Output Slot Protocol«.
 ** @remark this adaptor is generated automatically when some specific
 **      output technology is exposed through an OutputSlot; at that
 **      point, a custom OutputSlot::Connection implementation
 **      is [instantiated](\ref OutputSlot::allocate), causing
 **      the setup and wiring of OutputSlot::AllocState<CON>.
 ** @see buffer-proxy-adaptor-test.cpp (a pathfinder for this design)
 ** @see output-alloc-state-test.cpp
 ** @see output-slot-connection.hpp
 ** @see output-slot.hpp
 */

#ifndef VAULT_OUT_OUTPUT_BUFFER_PROXY_H
#define VAULT_OUT_OUTPUT_BUFFER_PROXY_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "vault/mem/buffer-provider.hpp"
#include "vault/mem/buffer-provider-setup.hpp"
#include "vault/mem/simple-buffer-state-registry.hpp"
#include "vault/mem/local-buffer-stage.hpp"

#include <type_traits>

namespace vault{
namespace out  {
  
  using lib::HashVal;
  using vault::mem::Buff;
  using vault::mem::LocalTag;
  using vault::mem::BuffAlloc;
  using vault::mem::BuffDescr;
  using vault::mem::TypeHandler;
  using vault::mem::BufferProviderSetup;
  
  
  
  /**
   * Adapter to expose controlled access to the \ref OutputSlot::Connection
   * through a [»Buffer Type«](\ref BuffDescr) and a BuffHandle as front-end,
   * in accordance with the BufferProvider protocol.
   * 
   * This template is an implementation building block for the OutputSlot and
   * creates the crucial link between the DataSink handle exposed to the client
   * and the actual implementation of the connection technology. It needs to be
   * instantiated with the concrete connection implementation type; this happens
   * as part of instantiating OutputSlot::AllocState<CON> — and this is the
   * typical way how a concrete connection is packaged into an OutputSlot.
   * 
   * @tparam CON the actual OutputSlot::Connection implementation
   * @tparam isTest use a naive self-contained metadata hashtable;
   *                by default, a threadsafe system of local metadata tables
   *                is used in conjunction with the EngineBufferMetadata service
   */
  template<class CON, bool isTest=false>
  class OutputBufferProxy
    : public BufferProviderSetup
    {
      
      class OutputBufferStore
        : public BufferProviderSetup::Store
        {
          CON&
          asTarget (LocalTag targetMarker)
            {
              void* targetAdr{targetMarker};
              return * static_cast<CON*> (targetAdr);
            }
          
          uint
          prepareBuffers (HashVal,uint,size_t)  override
            {
              NOTREACHED ("This is not a general purpose BufferProvider");
              return 1;
            }
          
          BuffAlloc
          provideBuffer (HashVal,size_t siz, LocalTag targetMarker, int64_t frameNr)  override
            {
              Buff* buffer = asTarget(targetMarker).claimBufferFor (frameNr);
              BuffAlloc storageSlot{buffer, siz, targetMarker};
              return storageSlot;
            }
          
          void
          mark_emitted (HashVal, BuffAlloc storageSlot)  override
            {
              auto& [buffer,siz,targetMarker] = storageSlot;
              asTarget(targetMarker).publish (buffer);
            }
          
          void
          detachBuffer (HashVal, BuffAlloc storageSlot)  override
            {
              auto& [buffer,siz,targetMarker] = storageSlot;
              asTarget(targetMarker).release (buffer);
            }
        };
      
      struct Setup
        {
          using MetadataRegistry = std::conditional_t<isTest, vault::mem::SimpleBufferStateRegistry
                                                            , vault::mem::LocalBufferStage>;  //  ◁──────────┨ by default use the production-grade implementation
          
          auto buildStage() { return std::make_unique<MetadataRegistry>("OutputBufferProxy"); }
          auto buildStore() { return std::make_unique<OutputBufferStore>(); }
        };
      
      
    public:
      OutputBufferProxy()
        : BufferProviderSetup{Setup{}}
        { }
      
      
      BuffDescr
      getDescriptorFor (CON& connection)
        {
          LocalTag connectionID{&connection};
          size_t buffSiz = connection.getBufferSize();
          auto& typeKey = bufferStage_->defineBufferType (buffSiz, TypeHandler::RAW, connectionID);
          return buildDescriptor (typeKey);
        }
    };
  
  
}} // namespace vault::out
#endif /*VAULT_OUT_OUTPUT_BUFFER_PROXY_H*/
