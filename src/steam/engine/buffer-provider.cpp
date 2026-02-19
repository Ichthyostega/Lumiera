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
 ** Implementation details related to buffer allocation and lifecycle management in the render engine.
 ** This translation unit comprises the "back side" of the public front-end, where calls from the
 ** client are received and relayed to the appropriate part of the implementation back-end.
 ** Furthermore, some of the API functions on the [Buffer Descriptor](\ref BuffDescr) and the
 ** [Buffer Handle](\ref BuffHandle) are also directly implemented here, by forwarding them
 ** through the related implementation APIs. The actual backend implementation however is
 ** split out and isolated by the internal interfaces BufferProvider::BufferStage (for the
 ** state and lifecycle management) and BufferProvider::BufferStore (for memory handling).
 ** 
 ** The actual backend implementations to use are configured in subclasses.
 */


#include "lib/error.hpp"
#include "steam/engine/buffer-provider.hpp"
#include "steam/engine/buffer-metadata.hpp"  /////////////////////////OOO must be removed from here
#include "lib/util.hpp"

#include <utility>

using std::move;
using util::isSameAdr;

namespace steam {
namespace engine {
  
  // storage for the default-marker constants
  const TypeHandler TypeHandler::RAW{};
  const LocalTag  LocalTag::UNKNOWN{};
  const metadata::Key metadata::Key::INVALID{HashVal(0), size_t(0)};  //////OOO consider to relocate those marker constants to somewhere more obvious
  
  
  LUMIERA_ERROR_DEFINE (BUFFER_MANAGEMENT, "Problem providing working buffers");
  
  
  
  BufferProvider::~BufferProvider() { }
  
  
  /** @internal verify the given descriptor.
   *  @return true if it corresponds to a buffer
   *          currently locked and usable by client code
   */
  bool
  BufferProvider::isValid (HashVal id)  const
  {
    return bool (bufferStage_->lookup (id));
  }
  
  bool
  BufferProvider::isAllotted (HashVal id)  const
  {
    return bufferStage_->isAllotted (id);
  }
  
  bool
  BufferProvider::isAccessible (HashVal id)  const
  {
    return bufferStage_->isAccessible (id);
  }
  
  size_t
  BufferProvider::getBufferSize (HashVal id)  const
  {
    auto& key = bufferStage_->lookup (id);
    return key.storageSize();
  }
  
  
  
  BuffDescr
  BufferProvider::getDescriptorFor (size_t storageSize)
  {
    auto& typeKey = bufferStage_->defineBufferType (storageSize, TypeHandler::RAW);
    return buildDescriptor (typeKey);
  }
  
  
  BuffDescr
  BufferProvider::getDescriptorFor(size_t storageSize, TypeHandler specialTreatment)
  {
    auto& typeKey = bufferStage_->defineBufferType (storageSize, move (specialTreatment));
    return buildDescriptor (typeKey);
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
   *  use by client code. Performs the necessary metadata state transition
   *  leading from an abstract buffer type to a derived metadata::Entry
   *  that corresponds to an actual buffer and has a lifecycle state.
   *  This entry is marked as _locked_ for exclusive use by one client.
   *  The returned handle allows to query state and perform further
   *  state transitions (_emitted_, _released_), yet provides no
   *  automatic tracking and clean-up. The client is explicitly
   *  responsible to invoke #releaseBuffer (either on the
   *  BufferProvider, or directly on the BuffHandle).
   * @return a copyable handle, representing this buffer and this usage transaction.
   * @throw error::State when unable to provide this buffer
   * @note this function may be used right away, without prior announcing, but then
   *       the client should be prepared for exceptions. The #announce operation allows
   *       to establish a reliably available baseline.
   * @todo 2/2025 might need to accept a cache key as additional parameter //////////////////////////////////TICKET #1392 : get cache key from computation to the cache backend
   * @todo 2/2026 my intention is to use metadata::Entry within a threadsafe allocator
   *       to pass buffers around. For this to be possible, buffer-metadata.hpp needs
   *       some refactoring, and then the BufferStore::provideBuffer would have to
   *       return a metadata::Entry
   */
  BuffHandle
  BufferProvider::lockBuffer (BuffDescr const& type)
  {
    REQUIRE (was_created_by_this_provider (type));
    auto& typeKey = bufferStage_->lookup (type);
    auto [storage, localTag] = bufferStore_->provideBuffer (typeKey.storageSize(), type, typeKey.localTag());
    auto& stateKey = bufferStage_->mark_locked (typeKey, storage, localTag);
    
    return BuffHandle (buildDescriptor(stateKey), storage);
  }                      // NOTE: not the underlying parent descriptor!
  
  
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
    auto& stateKey = bufferStage_->mark_emitted (handle);
    bufferStore_->mark_emitted (stateKey.storageSize(), stateKey.parentKey(), stateKey.localTag());
  }
  
  
  /** BufferProvider API: declare buffer usage as completed and detach from storage.
   *  Client code is required to release _each previously locked buffer_ eventually.
   * @warning invalidates the BuffHandle, clients mustn't access the buffer anymore.
   *          Right after releasing, an access through the handle will throw;
   *          yet the buffer might be re-used and the handle become valid
   *          later on accidentally.
   * @note EX_FREE
   */
  void
  BufferProvider::releaseBuffer (BuffHandle& handle)
  try {                         // might invoke embedded dtor function
    auto& stateKey = bufferStage_->mark_released (handle);
    bufferStore_->detachBuffer (stateKey.storageSize(), stateKey.parentKey()
                               ,std::make_tuple (handle.rawStorage(), stateKey.localTag()));
    bufferStage_->discard (handle);
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
  BufferProvider::emergencyCleanup (BuffHandle& handle, bool invokeDtor)
  try {
    auto& stateKey = bufferStage_->abandon (handle, invokeDtor);
    bufferStore_->detachBuffer (stateKey.storageSize(), stateKey.parentKey()
                               ,std::make_tuple (handle.rawStorage(), stateKey.localTag()));
    bufferStage_->discard (handle);
  }
  ERROR_LOG_AND_IGNORE (engine, "cleanup of buffer metadata while handling an error")
  
  
  
  
  
  
  
  /* === BuffDescr and BuffHandle === */
  
  BuffDescr
  BufferProvider::buildDescriptor (HashVal key)
  {
    return {*this, key};
  }
  
  bool
  BufferProvider::was_created_by_this_provider (BuffDescr const& descr)  const
  {
    return isSameAdr (this, descr.provider_);
  }
  
  /**
   * A Buffer Descriptor is considered _valid_
   * iff it represents a registered buffer type.
   * @note this implies that the buffer size is not zero.
   */
  bool
  BuffDescr::isValid()  const
  {
    ENSURE (provider_);
    return provider_->isValid (*this);
  }
  
  /**
   * A concrete Buffer Handle is considered valid,
   * iff it is currently registered in the buffer metadata table,
   * and the corresponding metadata::Entry indicates LOCKED state,
   * but not yet emitted (or released).
   * @note this implies also that it is in some active state, and
   *       indirectly (due to the _Buffer Provider Protocol_) also
   *       that the _parent type_ is valid, which means that it was
   *       created from a valid BuffDescr. But note that we can not
   *       see this parent type without lookup by the BufferProvider;
   *       The embedded #descriptor_ is synthetic, and reflects the
   *       handle's _state-key_ (⟶ registry Entry) not the type-key.
   */
  bool
  BuffHandle::isValid()  const
  {
    ENSURE (descriptor_.provider_);
    return bool(pBuffer_)
       and descriptor_.provider_->isAccessible (*this);
  }
  
  bool
  BuffHandle::isAllotted()  const
  {
    ENSURE (descriptor_.provider_);
    return bool(pBuffer_)
       and descriptor_.provider_->isAllotted (*this);
  }
  
  
  size_t
  BuffDescr::buffSize() const
  {
    ENSURE (provider_);
    return provider_->getBufferSize (*this);
  }
  
  size_t
  BuffHandle::size()  const
  {
    return descriptor_.buffSize();
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
        REQUIRE (isAllotted());
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
