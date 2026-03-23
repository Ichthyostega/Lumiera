/*
  DIAGNOSTIC-OUTPUT-SLOT.hpp  -  helper for testing against the OutputSlot interface

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file diagnostic-output-slot.hpp
 ** An facility for writing unit-tests against the OutputSlot interface.
 **
 ** @see output-slot-protocol-test.cpp
 */


#ifndef VAULT_OUT_DIAGNOSTIC_OUTPUT_SLOT_H
#define VAULT_OUT_DIAGNOSTIC_OUTPUT_SLOT_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "include/logging.h"
#include "vault/out/output-slot.hpp"
#include "vault/out/output-slot-connection.hpp"
#include "vault/mem/buffhandle.hpp"
#include "vault/mem/naive-buffer-setup.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/grid.hpp"
#include "lib/p.hpp"
#include "lib/null-value.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/iter-source.hpp" /////////////OOO wech
#include "lib/symbol.hpp"
#include "lib/util.hpp"
#include "vessel/advice.hpp"
#include "test/test-frame.hpp"

#include <unordered_set>
#include <vector>
#include <memory>


namespace vault {
namespace out   {
  namespace err = lumiera::error;

  using lib::Symbol;
  using lib::HashVal;
  using util::unConst;
  using util::contains;
  using test::TestFrame;
  using lib::time::FrameRate;
  using lib::time::TimeVar;
  using lib::time::Time;
  using vault::mem::NaiveBufferSetup;
  using vault::mem::BuffHandle;
  using vault::mem::BuffDescr;

  using std::shared_ptr;
  using std::make_unique;
  
  using PGrid = lib::P<lib::time::Grid>;

  
  namespace { // diagnostics & internals....
    
    const uint DEFAULT_DATAFEEDS = 5;
  }
  
///////////////////////////////////////////////////////////////////////
/// ///////////////////////////////////////////////////////////////////
      ///////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1395 : draft compatible Tracking-API
  namespace diagn {
    using Buff = vault::mem::Buff;
    
    struct FraInfo
      {
        TimeVar locked{Time::NEVER};
        TimeVar emitted{Time::NEVER};
        TimeVar released{Time::NEVER};
        
        bool wasLocked() const { return locked != Time::NEVER; }
        bool wasEmitted() const { return locked != Time::NEVER; }
        bool wasReleased() const { return locked != Time::NEVER; }
        
        size_t buffSize{0};
        Buff* storage{nullptr};
        Buff* accessMemory()  const { return storage; }
        
        template<typename BU>
        BU const& accessAs()  const;
        
        template<typename BU>
        bool operator== (BU const&)  const;
      };
    
    struct FeedLog
      {
        FraInfo const&
        frame (uint fraNr)  const
          {
            ///////////////////////////OOO access stored log
            return lib::NullValue<FraInfo>::get();
          }
        
        auto
        blockIter(uint startFrame =0)  const
          {
            return lib::explore (lib::NumIter{startFrame, std::numeric_limits<uint>::max()})
                      .transform([this](uint idx){ return frame(idx); })
                      ;
          }
      };
  
    template<typename BU>
    inline BU const&
    FraInfo::accessAs()  const
    {
      if (not storage)
        throw err::Logic ("buffer for this frame was never actually locked"
                         , LERR_(LIFECYCLE));
      return *reinterpret_cast<BU const*> (storage);
    }
    
    template<typename BU>
    inline bool
    FraInfo::operator== (BU const& refVal)  const
    {
      return storage
         and accessAs<BU>() == refVal;
    }
  }//(End)Diagnostic data

  
  
  class OutputDiagnostic;
  
  
  
  /****************************************************************//**
   * Helper for unit tests: Mock output sink.
   * Complete implementation of the OutputSlot interface, with some
   * additional stipulations to support unit testing.
   * - the implementation uses a special protocol output buffer,
   *   which stores each "frame" in memory for later investigation
   * - the output data in the buffers handed over from client
   *   actually hold an TestFrame instance 
   * - the maximum number of channels and the maximum number
   *   of acceptable frames is limited to 5 and 100.
   * @warning any Captured (test) data from all individual instances
   *   remains in memory until shutdown of the current executable
   */
  class DiagnosticOutputSlot
    : public OutputSlot
    {
      class OutputTracker;
      shared_ptr<OutputTracker> tracker_;
      
      /** @internal fake implementation of a connection,
       *            with callback to the tracker */
      class DummyConnection;
      
    public:
      struct Config
        {
          uint numDataFeeds{DEFAULT_DATAFEEDS};
          size_t bufferSize{sizeof(TestFrame)};
          
          
          /** @todo workaround for an arcane complier / language problem,
           *        causing the compiler wrongly to treat Config as incomplete,
           *        see [Stackoverflow], [Bugzilla], [CoreWorkingGroup-2335].
           *  @note using a static function, thus keeping Config a plain aggregate.
           * [Stackoverflow]: https://stackoverflow.com/q/53408962/444796
           * [Bugzilla]: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=88165
           * [CoreWorkingGroup-2335]: https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#2335
           */
          static constexpr Config defaults() { return {}; }
        };
      
      /**
       * Official (default) constructor to create a DiagnosticOutputSlot.
       * When invoked without argument, the default Config applies, otherwise
       * the explicitly given config values are uses. The resulting object
       * has value semantics and is automatically activated and ready to use.
       * It can be used in the same way as a regular OutputSlot, as a copyable
       * value handle; it can even be slice-copied.
       * @note you need to keep around one copy after the test,
       *       in order to [access the diagnostics](\ref watch)
       */
      DiagnosticOutputSlot(Config =Config::defaults());
        
      // standard copy operations acceptable
      
     ~DiagnosticOutputSlot();
      
    private:
      DiagnosticOutputSlot(shared_ptr<OutputTracker>);
      
      static shared_ptr<OutputTracker> setupOutputTracker(Config&&);
      static unique_ptr<Allocation> setupTrackingConnections(OutputTracker&);
      
    private:
      /// „backdoor“ to watch instrumentation from tests
      friend class OutputDiagnostic;
    };
  

  
  /** Accessor-proxy to investigate transactions */
  class OutputDiagnostic
    : util::MoveOnly
    {
      DiagnosticOutputSlot& dos_;
    public:
      OutputDiagnostic (DiagnosticOutputSlot& theSlot)
        : dos_{theSlot}
        { }
      
      uint cntLocked() { return 0; }
      uint cntEmitted() { return 0; }
      uint cntReleased() { return 0; }
      
      diagn::FeedLog const&
      feed (uint feedNr);
      
      auto
      getFeed (uint feedNr)
        {
          return feed(feedNr).blockIter();
        }
    };
  
  inline OutputDiagnostic
  watch (OutputSlot& theSlot)
  {
    return OutputDiagnostic{static_cast<DiagnosticOutputSlot&> (theSlot)};
  }
  
  
  
}} // namespace vault::out
#endif /*VAULT_OUT_DIAGNOSTIC_OUTPUT_SLOT_H*/
