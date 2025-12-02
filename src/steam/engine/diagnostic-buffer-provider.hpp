/*
  DIAGNOSTIC-BUFFER-PROVIDER.hpp  -  helper for testing against the BufferProvider interface

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file diagnostic-buffer-provider.hpp
 ** A facility for writing unit-tests targeting the BufferProvider interface.
 ** 
 ** @see buffer-provider-protocol-test.cpp
 */

#ifndef STEAM_ENGINE_DIAGNOSTIC_BUFFR_PROVIDER_H
#define STEAM_ENGINE_DIAGNOSTIC_BUFFR_PROVIDER_H


#include "lib/error.hpp"
#include "lib/depend.hpp"
#include "lib/util.hpp"
#include "steam/engine/type-handler.hpp"
#include "steam/engine/buffer-provider.hpp"
#include "lib/nocopy.hpp"

#include <memory>


namespace steam {
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
     
      friend class lib::DependencyFactory<DiagnosticBufferProvider>;
     
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
  
  
  
}} // namespace steam::engine
#endif
