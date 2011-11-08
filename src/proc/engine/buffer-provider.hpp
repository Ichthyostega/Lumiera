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
 ** It turns out that --  throughout the render engine implementation -- we never need
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

#ifndef PROC_ENGINE_BUFFR_PROVIDER_H
#define PROC_ENGINE_BUFFR_PROVIDER_H


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "proc/engine/buffhandle.hpp"

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>


namespace engine {
  
  using boost::scoped_ptr;
  using lib::Literal;
  
  
  class BufferMetadata;
  
  
  /**
   * Interface: a facility providing and managing working buffers for media calculations.
   * The pointer to actual buffer storage can be retrieved by
   * - optionally announcing the required buffer(s) beforehand
   * - "locking" a buffer to yield a buffer handle
   * - dereferencing this smart-handle class
   * 
   * @todo as of 6/2011 buffer management within the engine is still a bit vague
   */
  class BufferProvider
    : boost::noncopyable
    {
      scoped_ptr<BufferMetadata> meta_;
      
    protected:
      BufferProvider (Literal implementationID);
      
    public:
      virtual ~BufferProvider();  ///< this is an interface
      
      
      virtual uint announce (uint count, BufferDescriptor const&) =0;
      
      virtual BuffHandle lockBufferFor (BufferDescriptor const&)  =0;
      virtual void mark_emitted  (BuffHandle const&)              =0;
      virtual void releaseBuffer (BuffHandle const&)              =0;
      
      template<typename BU>
      BuffHandle lockBufferFor ();
      
      
      /** describe the kind of buffer managed by this provider */
      BufferDescriptor getDescriptorFor(size_t storageSize=0);
      
      template<typename BU>
      BufferDescriptor getDescriptor();

      
      
      /* === API for BuffHandle internal access === */
      
      bool verifyValidity (BufferDescriptor const&);
      
    };
    
    
    
    
  /* === Implementation === */
  
  /** convenience shortcut:
   *  prepare and claim ("lock") a buffer suitable
   *  to hold an object of the given type.
   * @return a handle embedding a suitably configured
   *         buffer descriptor. The corresponding buffer
   *         has been allocated and marked for exclusive use
   */
  template<typename BU>
  BuffHandle
  BufferProvider::lockBufferFor()
  {
    UNIMPLEMENTED ("convenience shortcut to announce and lock for a specific object type");
  }
  
  
  template<typename BU>
  BufferDescriptor
  BufferProvider::getDescriptor()
  {
    UNIMPLEMENTED ("build descriptor for automatically placing an object instance into the buffer");
  }
  
  
} // namespace engine
#endif
