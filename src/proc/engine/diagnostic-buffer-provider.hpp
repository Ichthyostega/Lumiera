/*
  DIAGNOSTIC-BUFFER-PROVIDER.hpp  -  helper for testing against the BufferProvider interface

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

/** @file diagnostic-buffer-provider.hpp
 ** An facility for writing unit-tests targetting the BufferProvider interface.
 **
 ** @see buffer-provider-protocol-test.cpp
 */

#ifndef PROC_ENGINE_DIAGNOSTIC_BUFFR_PROVIDER_H
#define PROC_ENGINE_DIAGNOSTIC_BUFFR_PROVIDER_H


#include "lib/error.hpp"
#include "proc/engine/buffer-provider.hpp"

#include <boost/noncopyable.hpp>


namespace engine {
  
  
  /********************************************************************
   * Helper for unit tests: Buffer provider reference implementation.
   * 
   * @todo write type comment
   */
  class DiagnosticBufferProvider
    : public BufferProvider
    {
      
      
      /* === BufferProvider Implementation === */
      
      virtual BuffHandle
      lockBufferFor (BufferDescriptor const& descriptor)
        {
          UNIMPLEMENTED ("lock buffer for exclusive use");
        }
      
      virtual void
      releaseBuffer (BuffHandle const& handle)
        {
          UNIMPLEMENTED ("release a buffer and invalidate the handle");
        }
      
      
    public:
      /** build a new Diagnostic Buffer Provider instance,
       *  discard the existing one. Use the static query API
       *  for investigating collected data. */
      static BufferProvider&
      build()
        {
          UNIMPLEMENTED ("Diagnostic Buffer Provider instance");
        }
      
      
      /** access the diagnostic API of the buffer provider
       * @throw error::Invalid if the given provider doesn't allow
       *        for diagnostic access or wasn't registered beforehand.
       */
      static DiagnosticBufferProvider&
      access (BufferProvider const& provider)
        {
          UNIMPLEMENTED ("access existing instance linked to the given provider");
        }
      
      
      
      
      /* === diagnostic API === */
      
      bool
      buffer_was_used (uint bufferID)
        {
          UNIMPLEMENTED ("check usage flag of a specific buffer");
        }
      
      
      bool
      buffer_was_closed (uint bufferID)
        {
          UNIMPLEMENTED ("check closed-flag of a specific buffer");
        }
      
      
      template<typename BU>
      bool
      object_was_attached (uint bufferID)
        {
          UNIMPLEMENTED ("verify object attachment status of a specific buffer");
        }
      
      
      template<typename BU>
      bool
      object_was_destroyed (uint bufferID)
        {
          UNIMPLEMENTED ("verify object attachment status of a specific buffer");
        }
      
      
      void*
      accessStorage (uint bufferID)
        {
          
        }
      
      
      
    private:
      
    };
  
  
  
} // namespace engine
#endif
