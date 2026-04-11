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
 ** Implementation details for an instrumented BufferProvider to support
 ** verifying render engine buffer allocation schemes through unit tests.
 ** 
 ** @see buffer-provider-protocol-test.cpp
 */


#include "lib/error.hpp"
#include "vault/mem/diagnostic-buffer-provider.hpp"
#include "vault/mem/heap-mem-buffer-store.hpp"
#include "lib/format-string.hpp"
#include "lib/util-foreach.hpp"
#include "lib/util.hpp"

#include <algorithm>
#include <utility>
#include <memory>


namespace vault {
namespace mem   {
  namespace err = lumiera::error;
  
  using util::_Fmt;
  using util::unConst;
  using diagn::Block;
  using diagn::StateReg;
  using std::unique_ptr;
  using std::ranges::find_if;
  using std::move;
  
  using metadata::Key;
  
  
  
  /* ====== Accounting  API ====== */
  
  Block::Block (Key key, Buff* mem)
    : typeKey{mem? key.parentKey(): HashVal(key)}
    , stateKey{mem? key : Key::INVALID}
    , buffSize{key.storageSize()}
    , storage{mem}
    { }
  
  
  void
  StateReg::record (Block block)
  {
    reg_.emplace_back (move (block));
  }
  
  size_t
  StateReg::cnt()  const
  {
    return reg_.size();
  }
  
  StateReg::Result
  StateReg::operator[] (size_t  seqNr)  const
  {
    if (seqNr < cnt())
      return reg_[seqNr];
    else
      return err::Invalid(_Fmt{"no Buffer with seq-nr %d (%d known entries)"}
                                                 % seqNr % cnt()
                         , LERR_(BOTTOM_VALUE));
  }
  
  StateReg::Result
  StateReg::byHandle (HashVal handle) const
  {
    if (auto pos = find_if (reg_, [&](auto& entry){ return handle == HashVal(entry); })
       ;pos != reg_.end()
       )
      return *pos;
    else
      return err::Invalid("requested Buffer never encountered"
                         , LERR_(BOTTOM_VALUE));
  }
  
  bool
  StateReg::contains (HashVal handle) const
    {
      return bool(byHandle (handle));
    }
  
  
  
  /**
   * Implementation of tracking and instrumentation
   */
  struct DiagnosticBufferProvider::BlockTracker
    : util::NonCopyable
    {
      StateReg created;
      StateReg emitted;
      StateReg released;
      
      void
      record_locked (Key stateKey, Buff* mem)
        {
          REQUIRE (not created.contains (stateKey));
          created.record (Block{stateKey, mem});
        }
      
      void
      record_emitted (HashVal id)
        {
          emitted.record (created.byHandle (id));
        }
      
      void
      record_released (HashVal id)
        {
          released.record (created.byHandle (id));
        }
    };
  
  
  class DiagnosticBufferProvider::InstrumentedStageProxy
    : public BufferProviderSetup::Stage
    {
      using StageImp = unique_ptr<BufferProvider::BufferStage>;
      using Tracker  = DiagnosticBufferProvider::BlockTracker;
      
      StageImp stage_;
      Tracker& tracker_;
      DiagnosticBufferProvider& provider_;
      
      
      /* === BufferStage proxy implementation === */

      ID
      lookup (HashVal key)  override
        {
          return stage_->lookup (key);
        }
      
      bool
      isAllotted (HashVal stateKey)  const override
        {
          return stage_->isAllotted (stateKey);
        }
      
      bool
      isAccessible (HashVal stateKey)  const override
        {
          return stage_->isAccessible (stateKey);
        }
      
      ID
      defineBufferType (size_t buffSiz, TypeHandler handlerFunctions, LocalTag localTag)
        {
          return stage_->defineBufferType (buffSiz, move(handlerFunctions), localTag);
        }
      
      ID
      mark_locked (ID typeKey, BuffAlloc storage)  override
        {
          ID stateKey = stage_->mark_locked (typeKey, storage);
          tracker_.record_locked (stateKey, getAddr(storage));
          return stateKey;
        }
      
      ID
      mark_emitted (HashVal id)  override
        {
          ID stateKey = stage_->mark_emitted (id);
          tracker_.record_emitted (stateKey);
          return stateKey;
        }
      
      ID
      mark_released (HashVal id)  override
        {
          ID stateKey = stage_->mark_released (id);
          tracker_.record_released (stateKey);
          return stateKey;
        }
      
      ID
      abandon (HashVal stateKey, bool invokeDtor)  override
        {
          return stage_->abandon (stateKey, invokeDtor);
        }
      
      void
      discard (HashVal stateKey)  override
        {
          stage_->discard (stateKey);
        }
      
    public:
      InstrumentedStageProxy (StageImp rawImpl, Tracker& tracker, DiagnosticBufferProvider& provider)
        : stage_{move(rawImpl)}
        , tracker_{tracker}
        , provider_{provider}
        { }
    };
  
  
  
  DiagnosticBufferProvider::DiagnosticBufferProvider()
    : NaiveBufferSetup{}
    , tracker_{std::make_unique<BlockTracker>()}
    {
      decorate<InstrumentedStageProxy> (bufferStage_, *tracker_, *this);
    }
  
  DiagnosticBufferProvider::~DiagnosticBufferProvider()
    {
//    INFO (proc_mem, "discarding %zu diagnostic buffer entries", outSeq_.size());   ////////////OOO implement based on dedicated tracking information in-object
    }
  
  BufferDiagnostic::BufferDiagnostic (DiagnosticBufferProvider& thePro)
    : dbp_{thePro}
    , created{dbp_.tracker_->created}
    , emitted{dbp_.tracker_->emitted}
    , released{dbp_.tracker_->released}
    { }
  
  
  
  /* === diagnostic API === */
  
  Buff*
  BufferDiagnostic::accessMemory (HashVal id)
    {                             // may throw when found
      return created.byHandle(id).get().accessMemory();
    }
  
  bool
  BufferDiagnostic::was_created (HashVal id)
  {
    return created.contains (id);
  }
  
  bool
  BufferDiagnostic::was_emitted (HashVal id)
  {
    return emitted.contains (id);
  }
  
  bool
  BufferDiagnostic::was_released (HashVal id)
  {
    return released.contains (id);
  }
  
  bool
  BufferDiagnostic::is_in_use (HashVal id)
  {
    return was_created (id)
       and not (was_released (id) or was_emitted (id));
  }
  
  bool
  BufferDiagnostic::was_used (HashVal id)
  {
    return was_created (id)
       and was_released (id);
  }
  
  bool
  BufferDiagnostic::all_buffers_released()
  {
    return util::and_all (created.each()
                         ,[this](Block const& entry){ return released.contains(entry); }
                         );
  }
  
  
  
}} // namespace vault::mem
