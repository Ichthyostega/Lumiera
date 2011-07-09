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
 ** i.e. "allocated", "locked" and "released"; the actual meaning of these operations is an implementatino detail.
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
#include "proc/engine/buffhandle.hpp"

#include <boost/noncopyable.hpp>


namespace engine {
  
  
  
  /**
   * Handle for a buffer for processing data, abstracting away the actual implementation.
   * The real buffer pointer can be retrieved by dereferencing this smart-handle class.
   * 
   * @todo as of 6/2011 buffer management within the engine is still a bit vague
   */
  class BufferProvider
    : boost::noncopyable
    {
      
    public:
      virtual ~BufferProvider();  ///< this is an interface
      
      ///////TODO: is there any generic way to obtain a BufferDescriptor; then we should expose it here...
      
      virtual BuffHandle lockBufferFor (BufferDescriptor const&)  =0;
      virtual void releaseBuffer (BuffHandle const&)              =0;  ////////TODO not quite sure what information to pass here
      
    };
  
  
  
} // namespace engine
#endif
