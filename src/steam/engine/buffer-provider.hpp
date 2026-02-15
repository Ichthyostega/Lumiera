/*
  BUFFER-PROVIDER.hpp  -  Abstraction for Buffer management during playback/render

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file buffer-provider.hpp
 ** Abstraction to represent buffer management and lifecycle within the render engine.
 ** When looking at the render engine implementation at a high level, buffers are a scarce
 ** resource to be managed. Looking into the implementation in detail however reveals — maybe
 ** surprisingly — that we never need _direct access_ to the buffers holding media data. This is
 ** a consequence of the Engine's design, which treats _actual media processing_ as an external concern,
 ** handled by specialised libraries and Plug-ins. Buffers are thus treated as some entity to be _managed_,
 ** i.e. "allocated", "locked" and "released"; the actual meaning of these operations is considered a detail
 ** of the implementation.
 ** 
 ** The code within the Render Engine is centred around _invocation_ and the transport of data; the latter is
 ** marked and represented as BufferHandle, that can be passed form invocation to invocation. For this to work,
 ** the _type_ of the buffers must be compatible with the data and the processing functions — and this condition
 ** is accommodated already in the preparation of the rendering, which is the purpose of the Builder. In this
 ** framework, BufferHandle objects act as a front-end, as being created by and linked to a BufferProvider
 ** implementation. There is no need to manage the lifecycle of buffers explicitly because the use of buffers
 ** is embedded into the fabric of the render calculation, which follows a rather strict protocol anyway.
 ** Relying on the capabilities of the scheduler, the sequence of individual jobs in the engine ensures...
 ** - that the availability of sufficient buffer memory was accommodated as part of the planning
 ** - that overall buffer capacity will be announced at start of the actual render invocation
 ** - that a buffer handle was obtained ("locked") prior to any operation requiring a buffer
 ** - that buffers are marked as free ("released") after doing the actual calculations.
 ** 
 ** # Buffer Provider Protocol
 ** 
 ** BufferProvider is a public interface, that also defines an interaction protocol.
 ** This implies that any usage of _buffer memory_ has to proceed through the stages of a process,
 ** which is flexible enough however to accommodate the various usage situations. At the bare minimum,
 ** a buffer can be _obtained and locked_, to get a fixed amount of memory, accessible through a BuffHandle.
 ** After usage, the buffer _must be released._ There is no tracking and no automatisation to ensure this
 ** duty is fulfilled — failure to release will block the buffer memory indeterminately. Usually this
 ** simple arrangement does not cause any serious problems, as the process of rendering media is
 ** bound to operate within very precisely defined boundaries anyway. Furthermore, the C++
 ** language offers various general-purpose capabilities to automate resource clean-up.
 ** 
 ** For the general case, the usage cycle is defined to be more elaborate and flexible:
 ** - in a pre-planning step, a [Buffer Type Descriptor](\ref BuffDescr) can be obtained
 **   + this descriptor is copyable and embeds a back-link to the BufferProvider
 **   + obtaining such a descriptor implies a _type registration_
 **   + in this context a **Buffer Type** describes at least a fixed amount of storage,
 **     and optionally also an _access type,_ or even an attached _object creation_ and
 **     automatic _destruction_.
 ** - from a given BuffDescr, the usage of a number of buffers can optionally be _announced;_
 **   doing so is highly recommended, since it confirms the availability of an actual number
 **   of buffers (which can be lower than requested); these buffers are guaranteed to be
 **   available, and this availability will even be pre-arranged for the actual thread,
 **   as an asynchronous process
 ** - for the actual usage, individual buffers can be _locked_ on the BuffDescr, thereby
 **   providing a BuffHandle. The number of actually obtained buffers need not match the
 **   announced number (yet obviously it should, overall). Buffers exceeding the announced
 **   capacity _can_ be obtained, if possible — in that case the operation may block or
 **   even fail with an exception.
 ** - a BuffHandle, once obtained, represents the client's ownership and liability
 ** - optionally, the client may _emit_ on the BuffHandle, to indicate that data
 **   in the Buffer is complete and will not be touched anymore. The interpretation
 **   of this call is reserved to special services attached behind the BufferProvider
 **   interface, e.g. a Cache service or an DataSink.
 ** - as the last step, a BuffHandle _must_ be _released_. It must not be used further
 **   after that point, since the buffer memory might have been re-assigned to another
 **   client, or way even released altogether.
 ** 
 ** # Providing an Implementation
 ** 
 ** The class BufferProvider is abstract, yet contains some orchestration and clue code
 ** to provide the high-level operations. Functionality is broken down in terms of
 ** two _implementation interfaces:_
 ** - BufferProvider::BufferStage describes the type registration and state transitions
 ** - BufferProvider::BufferStore handles coordination of memory access
 ** 
 ** For creating actual implementations, buffer-provider-setup.hpp defines a framework.
 ** A simplistic heap-based implementation for demonstration and unit test usage is
 ** available through NaiveBufferSetup. This can be extended with tracking functionality
 ** to verify actual operations from a test setup, with the help of DiagnosticBufferProvider.
 ** The [Render Engine setup](\ref engine-facilities.hpp) includes a suitable BufferProvider
 ** implementation; when used from the actual [Render Environment](\ref render-environment.hpp),
 ** this will be backed by a production-strenght and concurrency-safe implementation.
 ** 
 ** @see buffer-provider-protocol-test.cpp
 ** @see output-slot.hpp
 ** @see engine-ctx.hpp
 ** @see proc-node.hpp
 ** @see media-weaving-pattern.hpp
 ** @see weaving-pattern-builder.hpp
 */

#ifndef STEAM_ENGINE_BUFFR_PROVIDER_H
#define STEAM_ENGINE_BUFFR_PROVIDER_H


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/hash-value.h"
#include "steam/engine/buffhandle.hpp"
#include "steam/engine/type-handler.hpp"
#include "steam/engine/buffer-local-tag.hpp"
#include "steam/engine/buffer-metadata.hpp"  /////////////////////////OOO must be removed from here
#include "lib/nocopy.hpp"

#include <utility>
#include <memory>
#include <tuple>


namespace steam {
namespace engine {
  
  using lib::Literal;
  using std::unique_ptr;
  using std::forward;
  
  class BufferMetadata;
  namespace metadata {
    class Key;
  }
  
  
  LUMIERA_ERROR_DECLARE (BUFFER_MANAGEMENT); ///< Problem providing working buffers
  
  
  /**
   * Interface: a facility providing and managing working buffers for media calculations.
   * The pointer to actual buffer storage can be retrieved by
   * - optionally announcing the required buffer(s) beforehand
   * - "locking" a buffer to yield a buffer handle
   * - then dereferencing the obtained smart-handle
   * 
   * @warning all of BufferProvider is assumed to run within a threadsafe environment.
   * 
   * @todo as of 6/2011 buffer management within the engine is still a bit vague
   * @todo as of 11/11 thread safety within the engine remains to be clarified   ////////////////////////////TICKET #854
   */
  class BufferProvider
    : util::NonCopyable
    {
    protected: /* === for Implementation by concrete providers === */
      
      
    public:
      virtual ~BufferProvider();  ///< this is an ABC
      
      
      uint announce (uint count, BuffDescr const&);
      
      BuffHandle lockBuffer (BuffDescr  const&);
      void       emitBuffer (BuffHandle const&);
      void    releaseBuffer (BuffHandle const&);
      
      template<typename BU, typename...ARGS>
      BuffHandle lockBufferFor (ARGS ...args);
      
      void emergencyCleanup (BuffHandle const& target, bool invokeDtor =false);
      
      
      /** describe the kind of buffer managed by this provider */
      BuffDescr getDescriptorFor(size_t storageSize=0);
      BuffDescr getDescriptorFor(size_t storageSize, TypeHandler specialTreatment);
      
      template<typename BU, typename...ARGS>
      BuffDescr getDescriptor (ARGS ...args);
      
      
      
      /* === API for BuffHandle internal access === */
      
      bool verifyValidity (BuffDescr const&)  const;
      size_t getBufferSize (HashVal typeID)   const;
      
    protected:
      bool was_created_by_this_provider (BuffDescr const&)  const;
      
      using Buff = StreamType::ImplFacade::DataBuffer;  ///< marker type for an actual data buffer
      
      
      class BufferStage
        : public BufferMetadata         /////////////////////////////////////////////////////////////////////TICKET #1410 : mix-in exiting old implementation -- for sake of refactoring only!
        {
        protected:
          using ID = metadata::Key const&;
        public:
          virtual ~BufferStage() { } ///< this is an interface
          using BufferMetadata::BufferMetadata;   ///////////////////////////////////////////////////////////TICKET #1410 : actual implementation structures should move down into this classes implementation
          
          virtual ID defineBufferType (size_t, TypeHandler)    =0;
          virtual ID lookup (HashVal)                          =0;
          virtual ID mark_locked (ID typeKey, Buff*, LocalTag) =0;
          virtual ID mark_emitted (HashVal stateKey)           =0;
          virtual ID mark_released (HashVal stateKey)          =0;
          virtual ID abandon (HashVal, bool destroy=false)     =0;
          virtual void discard (HashVal stateKey)              =0;
        };
      
      class BufferStore
        : util::NonCopyable
        {
        protected:
          using Slot = std::tuple<Buff*,LocalTag>;
        public:
          virtual ~BufferStore() { } ///< this is an interface
          
          virtual uint prepareBuffers (uint cnt, size_t,HashVal typeID) =0;
          virtual Slot provideBuffer (size_t,HashVal typeID, LocalTag)  =0;
          virtual void mark_emitted (size_t,HashVal, LocalTag const&)   =0;
          virtual void detachBuffer (size_t,HashVal, Slot alloc)        =0;
        };
      
      unique_ptr<BufferStage> bufferStage_;       ///////////////////////////////////////////////////////////TICKET #1410 : must be turned into an internal interface
      unique_ptr<BufferStore> bufferStore_;
    };
  
  
  
  
  /* === Implementation === */
  
  /** convenience shortcut:
   *  prepare and claim ("lock") a buffer suitable
   *  to hold an object of the given type.
   * @return a handle embedding a suitably configured
   *         buffer descriptor. The corresponding buffer
   *         has been allocated and marked for exclusive use
   */
  template<typename BU, typename...ARGS>
  BuffHandle
  BufferProvider::lockBufferFor (ARGS ...args)
  {
    BuffDescr attach_object_automatically = getDescriptor<BU> (forward<ARGS> (args)...);
    return lockBuffer (attach_object_automatically);
  }
  
  
  /** define a "buffer type" for automatically creating
   *  an instance of the template type embedded into the buffer
   *  and destroying that embedded object when releasing the buffer.
   */
  template<typename BU, typename...ARGS>
  BuffDescr
  BufferProvider::getDescriptor (ARGS ...args)
  {
    return getDescriptorFor (sizeof(BU), TypeHandler::create<BU> (forward<ARGS> (args)...));
  }
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_BUFFR_PROVIDER_H*/
