/*
  DiagnosticBufferProvider  -  helper for testing against the BufferProvider interface

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file diagnostic-buffer-provider.cpp
 ** Implementation details of unit test support regarding internals of the render engine.
 ** 
 ** @todo WIP from 2013, half finished, stalled
 ** @see buffer-provider-protocol-test.cpp
 */


#include "lib/error.hpp"
#include "lib/meta/function.hpp"

#include "steam/engine/diagnostic-buffer-provider.hpp"
#include "steam/engine/tracking-heap-block-provider.hpp"


namespace steam {
namespace engine {
  
  
  /** Storage for the diagnostics frontend */
  lib::Depend<DiagnosticBufferProvider> DiagnosticBufferProvider::diagnostics;
  
  
  
  
  DiagnosticBufferProvider::DiagnosticBufferProvider()
    : pImpl_()
    { }
  
  DiagnosticBufferProvider::~DiagnosticBufferProvider() { }
  
  
  BufferProvider&
  DiagnosticBufferProvider::build()
  {
    return diagnostics().reset();
  }
  
  
  DiagnosticBufferProvider&
  DiagnosticBufferProvider::access (BufferProvider const& provider)
  {
    if (!diagnostics().isCurrent (provider))
      throw error::Invalid("given Provider doesn't match (current) diagnostic data record."
                           "This might be an lifecycle error. Did you build() this instance beforehand?");
    
    return diagnostics();
  }
  
  
  
  
  TrackingHeapBlockProvider&
  DiagnosticBufferProvider::reset()
  {
    if (pImpl_)
      pImpl_->markAllEmitted();
    pImpl_.reset(new TrackingHeapBlockProvider());
    return *pImpl_;
  }
  
  bool
  DiagnosticBufferProvider::isCurrent (BufferProvider const& implInstance)
  {
    return &implInstance == pImpl_.get();
  }
  
  
  
  
  
  /* === diagnostic API === */
  
  bool
  DiagnosticBufferProvider::buffer_was_used (uint bufferID)  const
    {
      return pImpl_->access_emitted(bufferID).was_used();
    }
  
  
  bool
  DiagnosticBufferProvider::buffer_was_closed (uint bufferID)  const
    {
      return pImpl_->access_emitted(bufferID).was_closed();
    }
  
  
  void*
  DiagnosticBufferProvider::accessMemory (uint bufferID)  const
    {
      return pImpl_->access_emitted(bufferID).accessMemory();
    }
  

}} // namespace engine
