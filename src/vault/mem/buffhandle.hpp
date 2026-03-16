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
 ** different [»buffer providers«](\ref buffer-provider.hpp) (for example the frame cache).
 ** Buffers are classified by a _buffer type,_ which essentially is the size of the storage,
 ** but could optionally also include a constructor and destructor function. Which type of
 ** buffer is used at some node invocation and which provider is backing and managing the
 ** allocation is considered static configuration of the Render Node Network and established
 ** by the **Builder**. Typically the actual render job is unaware of these details and only
 ** provides a parameter, that _just happens to select_ the proper source buffer for the
 ** frame to be computed.
 ** 
 ** However, the Node Network as such is strictly typed (at compile time). Information regarding
 ** the _buffer type_ are encoded into a [Buffer Descriptor](\ref BuffDescr). These descriptors
 ** are embedded into the _feed prototype_ of each node and used, by the actual _Node invocation_,
 ** to retrieve a [Buffer Handle](\ref BuffHandle). On the other hand, the _feed prototype_ was
 ** also attached to a typed context at build time, so that the invocation of the actual
 ** _processing function_ can access the buffers and any further parameters strictly typed.
 ** 
 ** Usage of these descriptors and handles is organised in accordance to the »Buffer Provider Protocol«,
 ** which also explains the structure of the BufferProvider API.
 ** - the first step is to obtain a BuffDescr from the BufferProvider, which implies
 **   to define at least the required storage size (optionally also a constructor/destructor).
 ** - the usage of some buffers with a given type can be _announced_ optionally, which allows
 **   to pre-arrange some storage space and associate it with the calling thread.
 ** - an actual allocation is initiated by _locking_ the descriptor, which yields a BuffHandle.
 ** - from this handle, the buffer memory can be accessed.
 ** - the client is then responsible to [»emit«](\ref BuffHandle::emit) on the handle,
 **   once the computation is complete and data can be published. This step can be omitted,
 **   and it depends on the actual situation if invoking this operation matters. For example,
 **   it is very relevant for a result sent to some output sink, or for a buffer attached
 **   to the frame cache. Without _emitting_, the data will not be propagated in those cases.
 ** - however, it is always mandatory to [»release«](\ref BuffHandle::release) the handle.
 ** 
 ** @note both BuffDescr and BuffHandle store a back-link to the managing BufferProvider.
 ** @warning BuffHandle is _not a smart-ptr_ — the client is responsible to `release()`
 ** 
 ** @see BufferProvider
 ** @see BufferProviderProtocol_test usage demonstration
 ** @see OutputSlot
 ** @see heap-mem-buffer-store.hpp  demo implementation of storage backend
 ** @see engine::RenderInvocation
 */

#ifndef VAULT_MEM_BUFFHANDLE_H
#define VAULT_MEM_BUFFHANDLE_H


#include "lib/error.hpp"
#include "lib/hash-value.h"


namespace vault {
namespace mem   {
  namespace err = lumiera::error;
  
  using lib::HashVal;
  
  class BuffHandle;
  class BufferProvider;
  
  /**
   * Placeholder type for the contents of a data buffer.
   * @remark The actual buffer will always be provided by a library implementation;
   *   throughout the engine, this implementation type is represented as BuffDescr.
   *   A `Buff*` represents the fact that there is a memory block managed by some library.
   */
  struct Buff { };
  
  
  /**
   * An opaque descriptor to identify the type and further properties of a data buffer.
   * For each kind of buffer, there is somewhere a BufferProvider responsible for the
   * actual storage management. This provider may "lock" a buffer for actual use,
   * returning a BuffHandle. BuffDescr is a convenient front-end to represent the arrangement
   * for some type / kind of buffer, and to [allocate](\ref BuffDescr::lockBuffer) it actually.
   * @note this descriptor and especially meaning of the #subClassification_ is
   *       implementation-defined and tied to a specific BufferProvider.
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
      
      bool isValid()  const;
      size_t buffSize() const;
      
      operator HashVal()  const { return subClassification_; }
      
      /** callback to engage buffer handling cycle */
      uint announce (uint count);
      BuffHandle lockBuffer();
    };
  
  
  
  
  /**
   * Handle to designate a buffer with data for processing, thereby abstracting from the
   * memory management implementation. The buffer content can be accessed through an unchecked
   * cast operation, assuming that the client has otherwise complete control over the data type
   * to be accessed, since the only way to retrieve such a handle is from a BuffDescr.
   * Besides some information function, BuffHandle exposes the important API functions
   * - `emit()` : indicate that processing is complete and the data can be used / published
   * - `release()` : indicate that the client will not touch this handle any more, so that
   *   storage can be repurposed.
   * @note This is a low-level feature and not a *smart-handle* — it is the clien's sole
   *   responsibility to invoke the `release()` function reliably.
   */
  class BuffHandle
    {
      BuffDescr descriptor_;
      Buff*     pBuffer_;
      
      
    public:
      /** @internal a buffer handle may be obtained by "locking"
       *  a buffer from the corresponding BufferProvider */
      BuffHandle(BuffDescr const& typeInfo, Buff* storage =nullptr)
        : descriptor_{typeInfo}
        , pBuffer_{storage}
        { }
      
      // using standard copy operations
      
      explicit
      operator bool()     const { return isValid(); }
      operator HashVal()  const { return descriptor_; }
      
      bool isAllotted() const;
      bool isValid()  const;
      size_t size()  const;
      
      
      void emit();
      void release();
      
      template<typename BU>
      BU& accessAs();
      
      Buff*
      rawStorage()  const
        {
          ENSURE (pBuffer_);
          return  pBuffer_;
        }
      
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
    if (not pBuffer_)
      throw err::Logic ("buffer not (yet) locked for access by clients"
                       , LERR_(LIFECYCLE));
    return *reinterpret_cast<BU*> (pBuffer_);
  }
  
  
  
  
}} // namespace vault::mem
#endif /*VAULT_MEM_BUFFHANDLE_H*/
