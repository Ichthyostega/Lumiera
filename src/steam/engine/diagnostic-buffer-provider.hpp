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
#include "lib/iter-adapter-stl.hpp"
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
        HashVal typeKey{0};
        HashVal stateKey{0};
        size_t buffSize{0};
        Buff* storage{nullptr};
        Buff* accessMemory()  const { return storage; }
        
        Block (metadata::Key, Buff* mem);
        operator HashVal()  const { return storage? stateKey:typeKey; }
        
        template<typename BU>
        BU const& accessAs()  const;
      };
    
    
    class StateReg
      : util::NonCopyable
      {
        using Registry = std::vector<Block>;
        Registry reg_;
        
      public:
        /* ========= Information API ========= */
        using Result = lib::Result<Block const&>;
        Result operator[] (size_t  seqNr)  const;
        Result byHandle   (HashVal handle) const;
        bool   contains   (HashVal handle) const;
        auto   each() const;
        size_t cnt()  const;
        
        /* ========= Accounting  API ========= */
        void record (Block);
      };
      
      
      inline auto
      StateReg::each()  const
      {
        return lib::iter_stl::eachElm (reg_);
      }
      
      template<typename BU>
      inline BU const&
      Block::accessAs()  const
      {
        if (not storage)
          throw error::Logic ("buffer was never actually locked for access by clients"
                             , LERR_(LIFECYCLE));
        return *reinterpret_cast<BU const*> (storage);
      }
    
  }//(End)diagnostic descriptors.

  
  
  /********************************************************************//**
   * Helper for unit tests: Buffer provider that tracks state transitions.
   * This is a variant of the _naive_ heap based BufferProvider implementation,
   * with additional tracking hooks to record the IDs and further information
   * of all allocations encountered at a state transition.
   * 
   * Test code should keep track of the BufferProvider instance used, and may
   * attach through a special [diagnostics access](\ref watch(BufferProvider&)),
   * to extract the recorded information while or after the allocated buffers
   * are used. Since the underlying "naive" implementation does not actually
   * discard any allocated memory block, it is possible to look into memory
   * contents after invoking the test subject.
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
      bool was_created (HashVal);
      bool was_emitted (HashVal);
      bool was_released (HashVal);
      bool is_in_use    (HashVal);
      bool was_used     (HashVal);
      Buff* accessMemory(HashVal);
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
