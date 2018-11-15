/*
  BUFFHANDLE.hpp  -  Buffer handling support for the render engine

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
 ** These buffer handles are based on a buffer descriptor record, which is opaque as far
 ** as the client is concerned. BufferDescriptor acts as a representation of the type or
 ** kind of buffer. The only way to obtain such a BufferDescriptor is from a concrete
 ** BufferProvider implementation. A back-link to this owning and managing provider is
 ** embedded into the BufferDescriptor, allowing to retrieve an buffer handle, corresponding
 ** to an actual buffer provided and managed behind the scenes. There is no automatic
 ** resource management; clients are responsible to invoke BuffHandle#release when done.
 ** 
 ** @see BufferProvider
 ** @see BufferProviderProtocol_test usage demonstration
 ** @see OutputSlot
 ** @see bufftable.hpp      storage for the buffer table
 ** @see engine::RenderInvocation
 */

#ifndef ENGINE_BUFFHANDLE_H
#define ENGINE_BUFFHANDLE_H


#include "lib/error.hpp"
#include "lib/hash-value.h"
#include "steam/streamtype.hpp"


namespace proc {
namespace engine {
  
  namespace error = lumiera::error;
  using error::LERR_(LIFECYCLE);
  
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
  class BufferDescriptor
    {
    protected:
      BufferProvider* provider_;
      HashVal subClassification_;
      
      BufferDescriptor(BufferProvider& manager, HashVal detail)
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
    };
  
  
  
  
  /**
   * Handle for a buffer for processing data, abstracting away the actual implementation.
   * The real buffer pointer can be retrieved by dereferencing this smart-handle class.
   */
  class BuffHandle
    {
      typedef StreamType::ImplFacade::DataBuffer Buff;
      
      BufferDescriptor descriptor_;
      Buff* pBuffer_; 
      
      
    public:
      typedef Buff* PBuff;
      
      /** @internal a buffer handle may be obtained by "locking"
       *  a buffer from the corresponding BufferProvider */
      BuffHandle(BufferDescriptor const& typeInfo, void* storage = 0)
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
      void takeOwnershipFor(BufferDescriptor const& type);
      
      void emergencyCleanup();
    };
  
  
  
  
}} // namespace proc::engine
#endif
