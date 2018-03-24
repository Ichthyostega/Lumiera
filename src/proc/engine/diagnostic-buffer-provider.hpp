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
 ** A facility for writing unit-tests targeting the BufferProvider interface.
 ** 
 ** @see buffer-provider-protocol-test.cpp
 */

#ifndef PROC_ENGINE_DIAGNOSTIC_BUFFR_PROVIDER_H
#define PROC_ENGINE_DIAGNOSTIC_BUFFR_PROVIDER_H


#include "lib/error.hpp"
#include "lib/depend.hpp"
#include "lib/util.hpp"
#include "proc/engine/type-handler.hpp"
#include "proc/engine/buffer-provider.hpp"
#include "lib/nocopy.hpp"

#include <memory>


namespace proc {
namespace engine {
  
  namespace error = lumiera::error;
  
  
  /**
   * simple BufferProvider implementation
   * with additional allocation tracking
   */
  class TrackingHeapBlockProvider;
  
  
  /****************************************************************//**
   * Helper for unit tests: Buffer provider reference implementation.
   * 
   * @todo write type comment
   */
  class DiagnosticBufferProvider
    : util::NonCopyable
    {
      
      std::unique_ptr<TrackingHeapBlockProvider>   pImpl_;
      static lib::Depend<DiagnosticBufferProvider> diagnostics;
      
      
      TrackingHeapBlockProvider& reset();
      bool isCurrent (BufferProvider const&);
      
      
      DiagnosticBufferProvider();
     ~DiagnosticBufferProvider();
     
      friend class lib::DependencyFactory;
     
    public:
      /** build a new Diagnostic Buffer Provider instance,
       *  discard the existing one. Use the static query API
       *  for investigating collected data. */
      static BufferProvider& build();
      
      
      /** access the diagnostic API of the buffer provider
       * @throw error::Invalid if the given provider doesn't allow
       *        for diagnostic access or wasn't registered beforehand.
       */
      static DiagnosticBufferProvider&
      access (BufferProvider const& );
      
      
      
      
      /* === diagnostic API === */
      
      bool buffer_was_used (uint bufferID)  const;
      bool buffer_was_closed (uint bufferID) const;
      void* accessMemory (uint bufferID)   const;
      bool all_buffers_released()          const;
      
      
      
    private:
      
    };
  
  
  
}} // namespace proc::engine
#endif
