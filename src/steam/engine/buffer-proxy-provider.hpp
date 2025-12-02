/*
  BUFFER-PROXY-PROVIDER.hpp  -  Adapter to access existing allocation via buffer handling protocol

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file buffer-proxy-provider.hpp
 ** Adapter to expose a given memory block through a BuffHandle.
 ** This allows to integrate a specific data access (e.g. related to input / output)
 ** with the buffer lifecycle protocol as defined by BufferProvider.
 ** @todo BROKEN as of 12/2024 //////////////////////////////////////////////////////////////////////////////TICKET #1387 : can not properly compose BufferProvider
 ** @see output-slot.hpp
 */

#ifndef STEAM_ENGINE_BUFFER_PROXY_PROVIDER_H
#define STEAM_ENGINE_BUFFER_PROXY_PROVIDER_H


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
   * Adapter to expose access to data blocks via BuffHandle and the BufferProvider protocol.
   * @todo WIP-WIP 12/2024 this is a design sketch to explore extension capabilities of BufferProvider
   */
  class BufferProxyProvider
    : util::NonCopyable
    {
      
      using Listener = std::function<void(size_t,BufferState)>;
      
      class ForwardingBufferProvider
        : public BufferProvider
        {
          Listener listener_;
            
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
        public:
          ForwardingBufferProvider (Listener listener)
            : BufferProvider{"BufferProxyProvider"}
            , listener_{std::move (listener)}
            { }
        };
      
      ForwardingBufferProvider passThroughProvider_;
      
      
    public:
      template<class LIS,                  typename = lib::meta::disable_if_self<BufferProxyProvider, LIS>>
      BufferProxyProvider (LIS&& listener)
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
#endif /*STEAM_ENGINE_BUFFER_PROXY_PROVIDER_H*/
