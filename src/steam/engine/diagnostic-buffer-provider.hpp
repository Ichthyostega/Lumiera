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
#include "lib/result.hpp"
#include "lib/nocopy.hpp"

#include <memory>
#include <vector>


namespace steam {
namespace engine {
  
  namespace error = lumiera::error;
  
  using Buff = StreamType::ImplFacade::DataBuffer;  ///////////TODO consider to define that marker type at some suitable central location
  
  class HeapMemBufferStore;  //////////////////////////////OOO fällt dann weg nach dem Umbau
  class BufferDiagnostic;
  
  namespace diagn {// state descriptors for diagnostics....
    
    /** represent the status of one allocation */
    struct Block
      {
        BuffHandle handle;
        Buff* storage{nullptr};
        Buff* accessMemory()  const { return storage; }
        
        Block (BuffDescr, metadata::Key, Buff* mem);
        Block (BuffDescr const&);
        operator HashVal()  const { return handle; }
      };
    
    
    class StateReg
      : util::NonCopyable
      {
        using PBlock   = std::shared_ptr<Block>;
        using Registry = std::vector<PBlock>;
        Registry reg_;
        
      public:
        /* ========= Information API ========= */
        using Result = lib::Result<Block const&>;
        Result operator[] (size_t  seqNr)  const;
        Result byHandle   (HashVal handle) const;
        bool   contains   (HashVal handle) const;
        size_t cnt()  const;
        
        /* ========= Accounting  API ========= */
        void record (Block);
        void record (PBlock);
      };
    
  }//(End)diagnostic descriptors.

  
  
  /****************************************************************//**
   * Helper for unit tests: Buffer provider reference implementation.
   * 
   * @todo write type comment
   */
  class DiagnosticBufferProvider
    : public NaiveBufferSetup
    {
      HeapMemBufferStore& heapStore_;  //////////////////////////////OOO fällt dann weg nach dem Umbau
      class InstrumentedStageProxy;
      struct BlockTracker;
      std::unique_ptr<BlockTracker> tracker_;
      
    public:
     ~DiagnosticBufferProvider();
      DiagnosticBufferProvider();
      
    private:
      /// „backdoor“ to watch instrumentation from tests
      friend class BufferDiagnostic;
    };
  
  
  /** Accessor-proxy to investigate transactions */
  class BufferDiagnostic
    : util::MoveOnly
    {
      DiagnosticBufferProvider& dbp_;
    public:
      BufferDiagnostic (DiagnosticBufferProvider&);
      
      bool buffer_was_used (uint bufferID);   ///< @deprecated       ////////////////////////////////////////TICKET 1410
      bool buffer_was_closed (uint bufferID); ///< @deprecated       ////////////////////////////////////////TICKET 1410
      void* accessMemory (uint bufferID);     ///< @deprecated       ////////////////////////////////////////TICKET 1410
      bool all_buffers_released();
      diagn::StateReg const& created;
      diagn::StateReg const& emitted;
      diagn::StateReg const& released;
    };
  
  inline BufferDiagnostic
  watch (BufferProvider& thePro)
  {
    return BufferDiagnostic{static_cast<DiagnosticBufferProvider&> (thePro)};
  }
  
  
}} // namespace steam::engine
#endif
