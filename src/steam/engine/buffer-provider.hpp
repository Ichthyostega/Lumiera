/*
  BUFFER-PROVIDER.hpp  -  Abstraction for Buffer management during playback/render

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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

*/

/** @file buffer-provider.hpp
 ** Abstraction to represent buffer management and lifecycle within the render engine.
 ** It turns out that -- throughout the render engine implementation -- we never need
 ** direct access to the buffers holding media data. Buffers are just some entity to be \em managed,
 ** i.e. "allocated", "locked" and "released"; the actual meaning of these operations is an implementation detail.
 ** The code within the render engine just pushes around BufferHandle objects, which act as a front-end,
 ** being created by and linked to a BufferProvider implementation. There is no need to manage the lifecycle
 ** of buffers automatically, because the use of buffers is embedded into the render calculation cycle,
 ** which follows a rather strict protocol anyway. Relying on the capabilities of the scheduler,
 ** the sequence of individual jobs in the engine ensures...
 ** - that the availability of a buffer was ensured prior to planning a job ("buffer allocation")
 ** - that a buffer handle was obtained ("locked") prior to any operation requiring a buffer
 ** - that buffers are marked as free ("released") after doing the actual calculations.
 ** 
 ** @see state.hpp
 ** @see output-slot.hpp
 */

#ifndef STEAM_ENGINE_BUFFR_PROVIDER_H
#define STEAM_ENGINE_BUFFR_PROVIDER_H


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/hash-value.h"
#include "steam/engine/buffhandle.hpp"
#include "steam/engine/type-handler.hpp"
#include "steam/engine/buffer-local-key.hpp"
#include "lib/nocopy.hpp"

#include <utility>
#include <memory>


namespace steam {
namespace engine {
  
  using lib::Literal;
  using std::unique_ptr;
  using std::forward;
  
  
  class BufferMetadata;
  
  
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
   * @todo as of 11/11 thread safety within the engine remains to be clarified   ///////////////////////////TICKET #854
   */
  class BufferProvider
    : util::NonCopyable
    {
      unique_ptr<BufferMetadata> meta_;
      
      
    protected: /* === for Implementation by concrete providers === */
      
      BufferProvider (Literal implementationID);
      
      virtual uint prepareBuffers (uint count, HashVal typeID)    =0;
      
      virtual BuffHandle provideLockedBuffer  (HashVal typeID)    =0;
      virtual void mark_emitted (HashVal typeID, LocalTag const&) =0;
      virtual void detachBuffer (HashVal typeID, LocalTag const&) =0;
      
      
    public:
      virtual ~BufferProvider();  ///< this is an ABC
      
      
      uint announce (uint count, BuffDescr const&);
      
      BuffHandle lockBuffer (BuffDescr  const&);
      void       emitBuffer (BuffHandle const&);
      void    releaseBuffer (BuffHandle const&);
      
      template<typename BU, typename...ARGS>
      BuffHandle lockBufferFor (ARGS ...args);
      
      /** allow for attaching and owing an object within an already created buffer */
      void attachTypeHandler (BuffHandle const& target, BuffDescr const& reference);
      
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
      BuffHandle buildHandle (HashVal typeID, void* storage, LocalTag const& =LocalTag::UNKNOWN);
      
      bool was_created_by_this_provider (BuffDescr const&)  const;
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
#endif
