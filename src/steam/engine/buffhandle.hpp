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
 ** @see heap-mem-buffer-store.hpp  demo implementation of storage backend
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
      size_t buffSize() const;
      
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
      /** @internal a buffer handle may be obtained by "locking"
       *  a buffer from the corresponding BufferProvider */
      BuffHandle(BuffDescr const& typeInfo, void* storage = 0)
        : descriptor_(typeInfo)
        , pBuffer_(static_cast<Buff*>(storage))
        { }
      
      // using standard copy operations
      
      explicit operator bool()  const { return isValid(); }
      
      
      void emit();
      void release();
      
      
      template<typename BU>
      BU& create();        /////////////////////TICKET #1410 : this API needs to be retracted; it does not fit into the concept!
      
      template<typename BU>
      BU& accessAs();
      
      Buff*
      rawStorage()  const
        {
          ENSURE (pBuffer_);
          return  pBuffer_;
        }
      
      operator HashVal()  const { return descriptor_; }
      
      bool
      isValid()  const
        {
          return bool(pBuffer_)
             and descriptor_.verifyValidity();
        }
      
      size_t
      size()  const
        {
          return descriptor_.buffSize();
        }
      
    private:
      void emergencyCleanup();
    };
  
  
  /**
   * Access the memory in the buffer by overlaying a specific type.
   * @warning this is a **blind cast**, there is _no type safety_.
   * @remark In the common usage pattern, a client has created a BuffDescr
   *       with suitable typing; it is assumed thus that the call context
   *       of the client ensures a a seamless link of type information
   *       at compile time (it's the clients responsibility to ensure that).
   *       Two flavours of this usage are conceivable:
   *       - the client [creates a descriptor](\ref BufferProvider::getDescriptorFor(size_t))
   *         that only reserves raw storage of sufficient size, e.g. `sizeof(TY)`. When
   *         accessing the raw storage subsequently through this accessor function,
   *         the effect is to have a _virtual overlay_ — without actually creating
   *         a new instance of the type within the storage.
   *       - the client also [registers a constructor functor](\ref BufferProvider::getDescriptor),
   *         which implies that a new instance of the target type is constructed into the buffer
   *         at the moment when the buffer is locked (and is destroyed on `release()`). This
   *         allows to setup elaborate structures, possibly even with a _back link_, by binding
   *         additional constructor arguments into the TypeHandler.
   * @note while clients can not access the internal type metadata, the _implementation_
   *       of BufferProvider can use the HashVal to access the type registry, and can even
   *       attach additional information as metadata::LocalTag to keep track of some
   *       specific property of the buffer, like e.g. the type of object.
   */
  template<typename BU>
  inline BU&
  BuffHandle::accessAs()
  {
    if (!pBuffer_)
      throw error::Logic ("buffer not (yet) locked for access by clients"
                         , LERR_(LIFECYCLE));
    return *reinterpret_cast<BU*> (pBuffer_);
  }
  
  
  
  
}} // namespace steam::engine
#endif
