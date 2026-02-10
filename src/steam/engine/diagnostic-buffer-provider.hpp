/*
  DIAGNOSTIC-BUFFER-PROVIDER.hpp  -  helper for testing against the BufferProvider interf,ttace

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
#include "lib/util.hpp"
#include "steam/engine/type-handler.hpp"  ///////////////OOO warum?
#include "steam/engine/naive-buffer-setup.hpp"
#include "lib/nocopy.hpp"

#include <memory>


namespace steam {
namespace engine {
  
  namespace error = lumiera::error;
  
  class HeapMemProvider;
  class BufferDiagnostic;
  
  
  
  /****************************************************************//**
   * Helper for unit tests: Buffer provider reference implementation.
   * 
   * @todo write type comment
   */
  class DiagnosticBufferProvider
    : public NaiveBufferSetup
    {
      HeapMemProvider& heapStore_;
      
      
    public:
     ~DiagnosticBufferProvider();
      DiagnosticBufferProvider();
      
    private:
      /// „backdoor“ to watch instrumentation from tests
      friend class BufferDiagnostic;
    };
  
  class BufferDiagnostic
    : util::MoveOnly
    {
      DiagnosticBufferProvider& dbp_;
    public:
      BufferDiagnostic (DiagnosticBufferProvider& thePro)
        : dbp_{thePro}
        { }
      
      
      bool buffer_was_used (uint bufferID);
      bool buffer_was_closed (uint bufferID);
      void* accessMemory (uint bufferID);
      bool all_buffers_released();
    };
  
  inline BufferDiagnostic
  watch (BufferProvider& thePro)
  {
    return BufferDiagnostic{dynamic_cast<DiagnosticBufferProvider&> (thePro)};
  }
  
  
}} // namespace steam::engine
#endif
