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
#include "steam/engine/buffer-metadata.hpp"
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
      
      using Listener = std::function<void(size_t,BufferState)>;
      
      class ForwardingBufferProvider
        : public BufferProvider                 /////////////////////////////////////////////////////////////TICKET #1410 : its clear now (2/26) that this must be some other base type; maybe we'll offer the ability to decorate, directly through BufferProviderSetup?
        {
          Listener listener_;
            
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : disabled code to while BufferProvider is being refactored...
            /* === BufferProvider API === */
            
            uint
            prepareBuffers (uint, HashVal)  override
              {
                NOTREACHED ("this part of the API should not be used");
                return 1; // can not sensibly do anything for "pre-allocation",
              }          //  other than telling the caller that we only "have one buffer to provide"
            
            BuffHandle
            provideLockedBuffer (HashVal typeID)  override
              {
                        /////////////////////////////////////////////////////////////////////////////////////TICKET #1387 : BufferProvider default impl. is lacking means to compose and delegate
//              return buildHandle (typeID, asBuffer(newBlock.accessMemory()), &newBlock);
              }
            
            void
            mark_emitted (HashVal, LocalTag const&)  override
              {
                  
              }

            void
            detachBuffer (HashVal, LocalTag const&, Buff&)  override
              {
                  
              }
#endif  /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : (end) disabled code
        public:
          ForwardingBufferProvider (Listener listener)
            : BufferProvider{}
            , listener_{std::move (listener)}
            { }
        };
      
      ForwardingBufferProvider passThroughProvider_;
      
      
    public:
      template<class LIS,                  typename = lib::meta::disable_if_self<BufferProxyAdaptor, LIS>>
      BufferProxyAdaptor (LIS&& listener)
        : passThroughProvider_{std::forward<LIS> (listener)}
        { }
      
      template<typename TAR>
      BuffHandle
      lockBuffer (TAR& dataBlock)
        {
           //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1387 : impossible due to inner contradictions in BufferProvider and OutputSlot
          UNIMPLEMENTED ("setup type handler and then create a locked BuffHandle");
        }
      
    };
  
  
  
  
  /* === Implementation === */
  
  /** convenience shortcut: */
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_BUFFER_PROXY_ADAPTOR_H*/
