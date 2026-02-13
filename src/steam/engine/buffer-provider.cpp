/*
  BufferProvider  -  Abstraction for Buffer management during playback/render

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file buffer-provider.cpp
 ** Implementation details related to buffer lifecycle management in the render engine.
 */


#include "lib/error.hpp"
#include "steam/engine/buffer-provider.hpp"
#include "steam/engine/buffer-metadata.hpp"  /////////////////////////OOO must be removed from here
#include "lib/util.hpp"

using util::isSameAdr;

namespace steam {
namespace engine {
  
  // storage for the default-marker constants
  const TypeHandler TypeHandler::RAW{};
  const LocalTag LocalTag::UNKNOWN{};
  
  
  namespace { // impl. details and definitions
    
    const uint DEFAULT_DESCRIPTOR = 0;
    
  }
  
  LUMIERA_ERROR_DEFINE (BUFFER_MANAGEMENT, "Problem providing working buffers");
  
  
  
  BufferProvider::~BufferProvider() { }
  
  
  /** @internal verify the given descriptor.
   *  @return true if it corresponds to a buffer
   *          currently locked and usable by client code
   */
  bool
  BufferProvider::verifyValidity (BuffDescr const& bufferID)  const
  {
    return bufferStage_->isLocked (bufferID);
  }
  
  
  BuffDescr
  BufferProvider::getDescriptorFor (size_t storageSize)
  {
    return BuffDescr (*this, bufferStage_->key (storageSize));
  }
  
  
  BuffDescr
  BufferProvider::getDescriptorFor(size_t storageSize, TypeHandler specialTreatment)
  {
    return BuffDescr (*this, bufferStage_->key (storageSize, specialTreatment));
  }
  
  
  size_t
  BufferProvider::getBufferSize (HashVal typeID)  const
  {
    metadata::Key& typeKey = bufferStage_->get (typeID);
    return typeKey.storageSize();
  }
  
  
  /**
   * Callback from implementation to build and enrol a BufferHandle,
   * to be returned to the client as result of the #lockBuffer call.
   * Performs the necessary metadata state transition leading from an
   * abstract buffer type to a metadata::Entry corresponding to an
   * actual buffer, which is locked for exclusive use by one client.
   * @note the implementation is free to provide an opaque \a implMarker,
   *       which becomes part of the key to designate this specific buffer.
   */
  BuffHandle
  BufferProvider::buildHandle (HashVal typeID, Buff* storage, LocalTag implMarker)
  {
    metadata::Key& typeKey = bufferStage_->get (typeID);
    metadata::Entry& entry = bufferStage_->markLocked (typeKey, storage, implMarker);
    
    return BuffHandle (BuffDescr(*this, entry), storage);
  }
  
  
  /** BufferProvider API: declare in advance the need for working buffers.
   *  This optional call allows client code to ensure the availability of the
   *  necessary working space, prior to starting the actual operations. The
   *  client may reasonably assume to get the actual number of buffers, as
   *  indicated by the return value. A provider may be able to handle
   *  various kinds of buffers (e.g. of differing size), which are
   *  distinguished by _the type embodied into_ the BuffDescr.
   * @return maximum number of simultaneously usable buffers of this type,
   *         to be retrieved later through calls to #lockBuffer.
   * @throw error::State when no buffer of this kind can be provided
   * @note the returned count may differ from the requested count.
   */
  uint
  BufferProvider::announce (uint cnt, BuffDescr const& type)
  {
    size_t buffSiz = getBufferSize (type);
    uint actually_possible = bufferStore_->prepareBuffers (cnt, buffSiz, type);
    if (!actually_possible)
      throw error::State ("unable to fulfil request for buffers"
                         ,LUMIERA_ERROR_BUFFER_MANAGEMENT);
    return actually_possible;
  }
  
  
  /** BufferProvider API: retrieve a single buffer for exclusive use.
   *  This call actually claims a buffer of this type and marks it for
   *  use by client code. The returned handle allows for convenient access,
   *  but provides no automatic tracking or memory management. The client is
   *  explicitly responsible to invoke #releaseBuffer (which can be done directly
   *  on the BuffHandle).
   * @return a copyable handle, representing this buffer and this usage transaction.
   * @throw error::State when unable to provide this buffer
   * @note this function may be used right away, without prior announcing, but then
   *       the client should be prepared for exceptions. The #announce operation allows
   *       to establish a reliably available baseline.
   * @todo 2/2025 might need to accept a cache key as additional parameter //////////////////////////////////TICKET #1392 : get cache key from computation to the cache backend
   */
  BuffHandle
  BufferProvider::lockBuffer (BuffDescr const& type)
  {
    REQUIRE (was_created_by_this_provider (type));
    metadata::Entry& metaEntry = bufferStage_->get (type);
    size_t buffSiz = metaEntry.storageSize();
    auto [storage, implMarker] = bufferStore_->provideBuffer (buffSiz, type, metaEntry.localTag());
    return buildHandle (type, storage, implMarker);
  }
  
  
  /** BufferProvider API: state transition to \em emitted state.
   *  Client code may signal a state transition through this optional operation.
   *  The actual meaning of an "emitted" buffer is implementation defined; similarly,
   *  some back-ends may actually do something when emitting a buffer (e.g. commit data
   *  to cache), while others just set a flag or do nothing at all. This state transition
   *  may be invoked at most once per locked buffer.
   * @throw error::Fatal in case of invalid state transition sequence. Only a locked buffer
   *        may be emitted, and at most once.
   * @warning by convention, emitting a buffer implies that the contained data is ready and
   *        might be used by other parts of the application.
   *        While an emitted buffer should not be modified anymore, it can still be accessed.
   */
  void
  BufferProvider::emitBuffer (BuffHandle const& handle)
  {
    metadata::Entry& metaEntry = bufferStage_->get (handle.entryID());
    size_t buffSiz = metaEntry.storageSize();
    bufferStore_->mark_emitted (buffSiz, metaEntry.parentKey(), metaEntry.localTag());
    metaEntry.mark(EMITTED);
  }
  
  
  /** BufferProvider API: declare done and detach.
   *  Client code is required to release _each previously locked buffer_ eventually.
   * @warning invalidates the BuffHandle, clients mustn't access the buffer anymore.
   *          Right after releasing, an access through the handle will throw;
   *          yet the buffer might be re-used and the handle become valid
   *          later on accidentally.
   * @note EX_FREE
   */
  void
  BufferProvider::releaseBuffer (BuffHandle const& handle)
  try {
    metadata::Entry& metaEntry = bufferStage_->get (handle.entryID());
    size_t buffSiz = metaEntry.storageSize();
    metaEntry.mark(FREE);   // might invoke embedded dtor function
    bufferStore_->detachBuffer (buffSiz, metaEntry.parentKey()
                               ,std::make_tuple (handle.rawStorage(), metaEntry.localTag()));
    bufferStage_->release (metaEntry);
  }
  ERROR_LOG_AND_IGNORE (engine, "releasing a buffer from BufferProvider")
  
  
  /** @internal abort normal lifecycle, reset the underlying buffer and detach from it.
   *  This allows to break out of normal usage and reset the handle to _invalid state_
   * @param invokeDtor if possibly the clean-up function of an TypeHandler registered with
   *        the buffer metadata should be invoked prior to resetting the metadata state.
   *        Default is \em not to invoke anything
   * @note EX_FREE
   */
  void
  BufferProvider::emergencyCleanup (BuffHandle const& target, bool invokeDtor)
  try {
    metadata::Entry& metaEntry = bufferStage_->get (target.entryID());
    size_t buffSiz = metaEntry.storageSize();
    metaEntry.invalidate (invokeDtor);
    bufferStore_->detachBuffer (buffSiz, metaEntry.parentKey()
                               ,std::make_tuple (target.rawStorage(), metaEntry.localTag()));
    bufferStage_->release (metaEntry);
  }
  ERROR_LOG_AND_IGNORE (engine, "cleanup of buffer metadata while handling an error")
  
  
  
  bool
  BufferProvider::was_created_by_this_provider (BuffDescr const& descr)  const
  {
    return isSameAdr (this, descr.provider_);
  }
  
  
  
  
  
  /* === BuffDescr and BuffHandle === */
  
  bool
  BuffDescr::verifyValidity()  const
  {
    ENSURE (provider_);
    return provider_->verifyValidity(*this);
  }
  
  
  size_t
  BuffDescr::determineBufferSize() const
  {
    ENSURE (provider_);
    return provider_->getBufferSize (*this);
  }
  
  
  uint
  BuffDescr::announce (uint count)
  {
    ENSURE (provider_);
    return provider_->announce(count, *this);
  }
  
  
  BuffHandle
  BuffDescr::lockBuffer()
  {
    ENSURE (provider_);
    return provider_->lockBuffer(*this);
  }
  
  
  void
  BuffHandle::emit()
  {
    REQUIRE (isValid());
    descriptor_.provider_->emitBuffer(*this);
  }
  
  
  void
  BuffHandle::release()
  {
    if (pBuffer_)
      {
        REQUIRE (isValid());
        descriptor_.provider_->releaseBuffer(*this);
        pBuffer_ = 0;
      }
    ENSURE (!isValid());
  }
  
  
  void
  BuffHandle::emergencyCleanup()
  {
    descriptor_.provider_->emergencyCleanup(*this); // EX_FREE
    pBuffer_ = 0;
  }
  
  
  
}} // namespace engine
