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


//#include "lib/error.hpp"
//#include "include/logging.h"
//#include "lib/meta/function.hpp"

#include "steam/engine/diagnostic-buffer-provider.hpp"
#include "steam/engine/heap-mem-buffer-store.hpp"
#include "lib/format-string.hpp"
#include "lib/util.hpp"

#include <algorithm>


namespace steam {
namespace engine {
  
  using util::_Fmt;
  using util::unConst;
  using diagn::Block;
  using diagn::StateReg;
  using std::ranges::find_if;
  
  
  /**
   * Implementation of tracking and instrumentation
   */
  struct DiagnosticBufferProvider::BlockTracker
    : util::NonCopyable
    {
      StateReg created;
      StateReg emitted;
      StateReg released;
    };

  Block::Block (BuffHandle const& handle)
    : handle{handle}
    { }
    
  Block::Block (BuffDescr const&  descr)
    : handle{descr}
    { }
  
  
  size_t
  StateReg::cnt()  const
  {
    return reg_.size();
  }
  
  StateReg::Result
  StateReg::operator[] (size_t  seqNr)  const
  {
    if (seqNr < cnt())
      return * reg_[seqNr];
    else
      return error::Invalid(_Fmt{"no Buffer with seq-nr %d (%d known entries)"}
                                                   % seqNr % cnt()
                           , LERR_(BOTTOM_VALUE));
  }
  
  StateReg::Result
  StateReg::byHandle (HashVal handle) const
  {
    if (auto pos = find_if (reg_, [&](auto& entry){ return handle == HashVal(*entry); })
       ;pos != reg_.end()
       )
      return **pos;
    else
      return error::Invalid("requested Buffer never encountered"
                           , LERR_(BOTTOM_VALUE));
  }
  
  
  /* ====== Accounting  API ====== */
  void
  StateReg::record (BuffHandle const& handle)
  {
    
  }
  
  
  DiagnosticBufferProvider::DiagnosticBufferProvider()
    : NaiveBufferSetup{}
    , heapStore_{dynamic_cast<HeapMemBufferStore&> (*bufferStore_)}   //////////////////////////////////////////TICKET 1410 : obsolete after switch to newtracking-API
    , tracker_{std::make_unique<BlockTracker>()}
    { }
  
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
  
  bool
  BufferDiagnostic::buffer_was_used (uint bufferID)
    {
      NOTREACHED ("shall use new API only");
//    return dbp_.heapStore_.access_emitted(bufferID).was_used();  //////////////////////////////////////////TICKET 1410 : switch to newly defined tracking-API
    }
  
  
  bool
  BufferDiagnostic::buffer_was_closed (uint bufferID)
    {
      NOTREACHED ("shall use new API only");
//    return dbp_.heapStore_.access_emitted(bufferID).was_closed();//////////////////////////////////////////TICKET 1410 : switch to newly defined tracking-API
    }
  
  
  void*
  BufferDiagnostic::accessMemory (uint bufferID)
    {
      NOTREACHED ("shall use new API only");
//    return dbp_.heapStore_.access_emitted(bufferID).accessMemory(); ///////////////////////////////////////TICKET 1410 : switch to newly defined tracking-API
    }
  

  

}} // namespace engine
