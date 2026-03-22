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
 ** @see output-proxy-provider-test.cpp
 ** @see output-buffer-proxy.hpp
 ** @see output-slot.hpp
 */

#ifndef VAULT_OUT_OUTPUT_BUFFER_PROXY_H
#define VAULT_OUT_OUTPUT_BUFFER_PROXY_H


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
namespace out  {
  
  using std::move;
  using lib::Literal;
  using lib::HashVal;
  using vault::mem::Buff;
  using vault::mem::LocalTag;
  using vault::mem::BuffAlloc;
  using vault::mem::BuffDescr;
  using vault::mem::BuffHandle;
  using vault::mem::TypeHandler; ///////////////////////////OOO Rly?
  using vault::mem::BufferProviderSetup;
  
  
  
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
  template<class CON>
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
              TODO ("delegate buffer lock");
              return storageSlot;
            }
          
          void
          mark_emitted (HashVal, BuffAlloc storageSlot)  override
            {
              TODO ("delegate buffer emit");
            }
          
          void
          detachBuffer (HashVal, BuffAlloc storageSlot)  override
            {
              TODO ("delegate buffer release");
            }
        };
      
      struct Setup
        {
          auto buildStage() { return std::make_unique<vault::mem::SimpleBufferStateRegistry>("OutputBufferProxy"); }
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
