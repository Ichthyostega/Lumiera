/*
  BufferProvider  -  Abstraction for Buffer management during playback/render

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/


/** @file buffer-provider.cpp
 ** Implementation details related to buffer lifecycle management in the render engine.
 */


#include "lib/error.hpp"
#include "steam/engine/buffer-provider.hpp"
#include "steam/engine/buffer-metadata.hpp"
#include "lib/util.hpp"

using util::isSameObject;

namespace steam {
namespace engine {
  
  // storage for the default-marker constants
  const TypeHandler TypeHandler::RAW{};
  const LocalTag LocalTag::UNKNOWN{};
  
  
  namespace { // impl. details and definitions
    
    const uint DEFAULT_DESCRIPTOR = 0;
    
  }
  
  LUMIERA_ERROR_DEFINE (BUFFER_MANAGEMENT, "Problem providing working buffers");
  
  
  /** build a new provider instance, managing a family of buffers.
   *  The metadata of these buffers is organised hierarchically based on
   *  chained hash values, using the #implementationID as a seed.
   * @param implementationID symbolic ID setting these family of buffers apart.
   */
  BufferProvider::BufferProvider (Literal implementationID)
    : meta_(new BufferMetadata (implementationID))
    { }
  
  BufferProvider::~BufferProvider() { }
  
  
  /** @internal verify the given descriptor.
   *  @return true if it corresponds to a buffer
   *          currently locked and usable by client code
   */
  bool
  BufferProvider::verifyValidity (BuffDescr const& bufferID)  const
  {
    return meta_->isLocked (bufferID);
  }
  
  
  BuffDescr
  BufferProvider::getDescriptorFor (size_t storageSize)
  {
    return BuffDescr (*this, meta_->key (storageSize));
  }
  
  
  BuffDescr
  BufferProvider::getDescriptorFor(size_t storageSize, TypeHandler specialTreatment)
  {
    return BuffDescr (*this, meta_->key (storageSize, specialTreatment));
  }
  
  
  size_t
  BufferProvider::getBufferSize (HashVal typeID)  const
  {
    metadata::Key& typeKey = meta_->get (typeID);
    return typeKey.storageSize();
  }
  
  
  /** callback from implementation to build and enrol a BufferHandle,
   * to be returned to the client as result of the #lockBuffer call.
   * Performs the necessary metadata state transition leading from an
   * abstract buffer type to a metadata::Entry corresponding to an
   * actual buffer, which is locked for exclusive use by one client.
   */ 
  BuffHandle
  BufferProvider::buildHandle (HashVal typeID, void* storage, LocalTag const& localTag)
  {
    metadata::Key& typeKey = meta_->get (typeID);
    metadata::Entry& entry = meta_->markLocked(typeKey, storage, localTag);
    
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
  BufferProvider::announce (uint count, BuffDescr const& type)
  {
    uint actually_possible = prepareBuffers (count, type);
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
   */
  BuffHandle
  BufferProvider::lockBuffer (BuffDescr const& type)
  {
    REQUIRE (was_created_by_this_provider (type));
    
    return provideLockedBuffer (type);
  }     // is expected to call buildHandle() --> state transition
  
  
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
    metadata::Entry& metaEntry = meta_->get (handle.entryID());
    mark_emitted (metaEntry.parentKey(), metaEntry.localTag());
    metaEntry.mark(EMITTED);
  }
  
  
  /** BufferProvider API: declare done and detach.
   *  Client code is required to release \em each previously locked buffer eventually.
   * @warning invalidates the BuffHandle, clients mustn't access the buffer anymore.
   *          Right after releasing, an access through the handle will throw;
   *          yet the buffer might be re-used and the handle become valid
   *          later on accidentally. 
   * @note EX_FREE
   */
  void
  BufferProvider::releaseBuffer (BuffHandle const& handle)
  try {
    metadata::Entry& metaEntry = meta_->get (handle.entryID());
    metaEntry.mark(FREE);   // might invoke embedded dtor function
    detachBuffer (metaEntry.parentKey(), metaEntry.localTag());
    meta_->release (metaEntry);
  }
  ERROR_LOG_AND_IGNORE (engine, "releasing a buffer from BufferProvider")
  
  
  
  /** @warning this operation locally modifies the metadata entry of a single buffer
   *           to attach a TypeHandler taking ownership of an object embedded within the buffer.
   *           The client is responsible for actually placement-constructing the object; moreover
   *           the client is responsible for any damage done to already existing buffer content.
   *  @note the buffer must be in locked state and the underlying buffer type must not define
   *        an non-trivial TypeDescriptor, because there is no clean way of superseding an
   *        existing TypeDescriptor, which basically is just a functor and possibly
   *        could perform any operation on buffer clean-up.
   *  @note EX_STRONG
   */
  void
  BufferProvider::attachTypeHandler (BuffHandle const& target, BuffDescr const& reference)
  {
    metadata::Entry& metaEntry = meta_->get (target.entryID());
    metadata::Entry& refEntry = meta_->get (reference);
    REQUIRE (refEntry.isTypeKey());
    REQUIRE (!metaEntry.isTypeKey());
    if (!metaEntry.isLocked())
      throw error::Logic{"unable to attach an object because buffer isn't locked for use"
                        , LERR_(LIFECYCLE)};
    
    metaEntry.useTypeHandlerFrom (refEntry); // EX_STRONG
  }
  
  
  /** @internal abort normal lifecycle, reset the underlying buffer and detach from it.
   *  This allows to break out of normal usage and reset the handle to \em invalid state 
   * @param invokeDtor if possibly the clean-up function of an TypeHandler registered with
   *        the buffer metadata should be invoked prior to resetting the metadata state.
   *        Default is \em not to invoke anything
   * @note EX_FREE
   */
  void
  BufferProvider::emergencyCleanup (BuffHandle const& target, bool invokeDtor)
  try {
    metadata::Entry& metaEntry = meta_->get (target.entryID());
    metaEntry.invalidate (invokeDtor);
    detachBuffer (metaEntry.parentKey(), metaEntry.localTag());
    meta_->release (metaEntry);
  }
  ERROR_LOG_AND_IGNORE (engine, "cleanup of buffer metadata while handling an error")
  
  
  
  bool
  BufferProvider::was_created_by_this_provider (BuffDescr const& descr)  const
  {
    return isSameObject (*this, *descr.provider_);
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
  
  
  /** Install a standard TypeHandler for an already locked buffer.
   *  This causes the dtor function to be invoked when releasing this buffer.
   *  The assumption is that client code will placement-construct an object
   *  into this buffer right away, and thus we're taking ownership on that object.
   * @param type a reference BuffDescr defining an embedded TypeHandler to use
   *        A copy of this TypeHandler will be stored into the local metadata for
   *        this buffer only, not altering the basic buffer type in any way
   * @throw lifecycle error when attempting to treat an buffer not in locked state
   * @throw error::Logic in case of insufficient buffer space to hold the
   *        intended target object
   * @note EX_STRONG
   */
  void
  BuffHandle::takeOwnershipFor(BuffDescr const& type)
  {
    if (!this->isValid())
      throw error::Logic ("attaching an object requires an buffer in locked state", LERR_(LIFECYCLE));
    if (this->size() < type.determineBufferSize())
      throw error::Logic ("insufficient buffer size to hold an instance of that type");
    
    descriptor_.provider_->attachTypeHandler(*this, type); // EX_STRONG
  }
  
  
  
  
}} // namespace engine
