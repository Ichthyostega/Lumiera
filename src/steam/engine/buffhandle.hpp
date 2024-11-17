/*
  BUFFHANDLE.hpp  -  Buffer handling support for the render engine

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file buffhandle.hpp
 ** A front-end to support the buffer management within the render nodes.
 ** When pulling data from predecessor nodes and calculating new data, each render node
 ** needs several input and output buffers. These may be allocated and provided by various
 ** different "buffer providers" (for example the frame cache). Typically, the real buffers
 ** will be passed as parameters to the actual job instance when scheduled, drawing on the
 ** results of prerequisite jobs. Yet the actual job implementation remains agnostic with
 ** respect to the way actual buffers are provided; the invocation just pushes BuffHandle
 ** objects around. The actual render function gets an array of C-pointers to the actual
 ** buffers, and for accessing those buffers, the node needs to keep a table of buffer
 ** pointers, and for releasing the buffers later on, we utilise the buffer handles.
 ** 
 ** These buffer handles are based on a [Buffer Descriptor record](\ref BuffDescr),
 ** which is opaque as far as the client is concerned. BuffDescr acts as a representation
 ** of the type or kind of buffer. The only way to obtain such a BuffDescr is from a concrete
 ** BufferProvider implementation. A back-link to this owning and managing provider is embedded
 ** into the BuffDescr, which thus may be used as a _configuration tag,_ allowing to retrieve a
 ** concrete buffer handle when needed, corresponding to an actual buffer provided and managed
 ** behind the scenes. There is no automatic resource management; clients are responsible to
 ** invoke BuffHandle#release when done.
 ** 
 ** @warning buffer management via BuffHandle and BuffDescr does _not automatically maintain
 **          proper alignment._ Rather, it relies on the storage allocator to provide a buffer
 **          suitably aligned for the target type to hold. In most cases, this target location
 **          will actually be storage maintained on heap through some STL collection;
 **          this topic is a possible subtle pitfall non the less.
 ** 
 ** @see BufferProvider
 ** @see BufferProviderProtocol_test usage demonstration
 ** @see OutputSlot
 ** @see bufftable.hpp      storage for the buffer table /////////////////////OOO where to point into the new structure for documentation?
 ** @see engine::RenderInvocation
 */

#ifndef ENGINE_BUFFHANDLE_H
#define ENGINE_BUFFHANDLE_H


#include "lib/error.hpp"
#include "lib/hash-value.h"
#include "steam/streamtype.hpp"


namespace steam {
namespace engine {
  namespace error = lumiera::error;
  
  using lib::HashVal;
  
  class BuffHandle;
  class BufferProvider;
  
  
  
  /**
   * An opaque descriptor to identify the type and further properties of a data buffer.
   * For each kind of buffer, there is somewhere a BufferProvider responsible for the
   * actual storage management. This provider may "lock" a buffer for actual use,
   * returning a BuffHandle.
   * @note this descriptor and especially the #subClassification_ is really owned
   *       by the BufferProvider, which may use (and even change) the opaque contents
   *       to organise the internal buffer management.
   */
  class BuffDescr
    {
    protected:
      BufferProvider* provider_;
      HashVal subClassification_;
      
      BuffDescr(BufferProvider& manager, HashVal detail)
        : provider_(&manager)
        , subClassification_(detail)
      { }
      
      friend class BufferProvider;
      friend class BuffHandle;
      
    public:
      // using standard copy operations
      
      bool verifyValidity()  const;
      size_t determineBufferSize() const;
      
      operator HashVal()  const { return subClassification_; }
      
      /** convenience shortcut to start a buffer handling cycle */
      uint announce (uint count);
      BuffHandle lockBuffer();
    };
  
  
  
  
  /**
   * Handle for a buffer for processing data, abstracting away the actual implementation.
   * The real buffer pointer can be retrieved by dereferencing this smart-handle class.
   */
  class BuffHandle
    {
      using Buff = StreamType::ImplFacade::DataBuffer;  ///< marker type for an actual data buffer
      
      BuffDescr descriptor_;
      Buff*     pBuffer_;
      
      
    public:
      typedef Buff* PBuff;
      
      /** @internal a buffer handle may be obtained by "locking"
       *  a buffer from the corresponding BufferProvider */
      BuffHandle(BuffDescr const& typeInfo, void* storage = 0)
        : descriptor_(typeInfo)
        , pBuffer_(static_cast<PBuff>(storage))
        { }
      
      // using standard copy operations
      
      explicit operator bool()  const { return isValid(); }
      
      
      void emit();
      void release();
      
      
      template<typename BU>
      BU& create();
      
      template<typename BU>
      BU& accessAs();
      
      
      //////////////////////////////////////////TICKET #249 this operator looks obsolete. The Buff type is a placeholder type,
      //////////////////////////////////////////TODO         it should never be accessed directly from within Lumiera engine code
      Buff&
      operator* ()  const
        {
          ENSURE (pBuffer_);
          return *pBuffer_;
        }
      
      bool
      isValid()  const
        {
          return bool(pBuffer_)
              && descriptor_.verifyValidity();
        }
      
      HashVal
      entryID()  const
        {
          return HashVal(descriptor_);
        }
      
      size_t
      size()  const
        {
          return descriptor_.determineBufferSize();
        }
      
    private:
      template<typename BU>
      void takeOwnershipFor();
      void takeOwnershipFor(BuffDescr const& type);
      
      void emergencyCleanup();
    };
  
  
  
  
}} // namespace steam::engine
#endif
