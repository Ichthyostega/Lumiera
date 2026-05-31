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
 ** The setup of DiagnosticOutputSlot is layered, insofar it is foremost
 ** a regular OutputSlot, yet has some kind of special _Output Manager_
 ** layered on top. For context, in regular use, it is the task of some
 ** specialised OutputManager to provide a suitably configured OutputSlot
 ** so that the client can use a specific output facility, like e.g. a
 ** video viewer in the UI, or the sound card output. Yet since this is
 ** a test setup, DiagnosticOutputSlot is backed by a _fake implementation_
 ** of the OutputSlot::Connection interface.
 ** 
 ** This layered construction allows for a simple and straight-forward
 ** usage, where the test can just create a DiagnosticOutputSlot instance,
 ** possibly passing a configuration record. This is in contrast to the
 ** real usage, where, as pointed out above, the OutputSlot must be
 ** maintained and connected to a real output driver by the OutputManager.
 ** By default, a configurable number of DataSink handles can be retrieved
 ** from this setup, and each sink allows to allocate an unlimited number
 ** of different frame buffers, in any order. Furthermore, the OutputSlot
 ** can be copied (and thereby sliced) and passed to the code to be tested.
 ** @todo 4/2026 the plan is to add the ability to define constraints and
 **       limitations through the configuration passed at construction.
 ** 
 ** After invoking the code under test, it is necessary to hold onto
 ** one copy of DiagnostricOutputSlot, which maintains the internal
 ** connection to the diagnostic information collected. Using the
 ** `watch(slot)` syntax, this information can be investigated.
 ** @see output-slot-protocol-test.cpp
 ** @see output-slot.hpp
 */


#ifndef VAULT_OUT_DIAGNOSTIC_OUTPUT_SLOT_H
#define VAULT_OUT_DIAGNOSTIC_OUTPUT_SLOT_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "test/test-frame.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/null-value.hpp"

#include "vault/out/output-slot.hpp"

#include <memory>
#include <vector>
#include <map>


namespace vault {
namespace out   {
  namespace err = lumiera::error;

  using lib::HashVal;
  using test::TestFrame;
//using lib::time::FrameRate; ///////TODO planned for time restrictions
  using lib::time::TimeVar;
  using lib::time::Time;

  using std::shared_ptr;
  
  
  namespace { // diagnostics & internals....
    
    const uint DEFAULT_DATAFEEDS = 5;
  }
  
  
  
  namespace diagn {
    using Buff = vault::mem::Buff;
    
    /** diagnostic record from handling one data frame */
    struct FrameInfo
      {
        TimeVar locked{Time::NEVER};
        TimeVar emitted{Time::NEVER};
        TimeVar released{Time::NEVER};
        
        bool wasLocked() const { return locked != Time::NEVER; }
        bool wasEmitted() const { return emitted != Time::NEVER; }
        bool wasReleased() const { return released != Time::NEVER; }
        
        size_t buffSize{0};
        Buff* storage{nullptr};
        Buff* accessMemory()  const { return storage; }
        
        template<typename BU>
        BU const& accessAs()  const;
        
        template<typename BU>
        bool operator== (BU const&)  const;
      };
    
    /** diagnostic record of handled frames _in a single feed_ */
    struct FeedLog
      {
        std::map<uint, FrameInfo> recorded;
        
        FrameInfo const&
        frame (uint fraNr)  const
          {
            auto pos = recorded.find (fraNr);
            if (pos != recorded.end())
              return pos->second;
            else
              return lib::NullValue<FrameInfo>::get();
          }
        
        auto
        allBlocks(uint startFrame =0)  const
          {
            return lib::explore (lib::NumIter{startFrame, std::numeric_limits<uint>::max()})
                      .transform([this](uint idx){ return frame(idx); })
                      ;
          }
        
        auto
        allRecordedBlocks()  const
          {
            return lib::iter_stl::eachVal (recorded);
          }
      };
    
    template<typename BU>
    inline BU const&
    FrameInfo::accessAs()  const
    {
      if (not storage)
        throw err::Logic ("buffer for this frame was never actually locked"
                         , LERR_(LIFECYCLE));
      return *reinterpret_cast<BU const*> (storage);
    }
    
    template<typename BU>
    inline bool
    FrameInfo::operator== (BU const& refVal)  const
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
      
      /** set the _running current time_ to be fixed to the given value.
       * @remark by default, without calling this function, each tracked invocation
       *         is marked by the current system time; by calling this function
       *         however, instead of a running time, fixed step stones will be
       *         recorded, so that the test can basically define the time axis.
       */
      void fixCurrentTime (TimeVar const&);
      
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
      
      uint cntLocked();
      uint cntEmitted();
      uint cntReleased();
      
      diagn::FeedLog const&
      feed (uint feedNr);
      
      auto
      getFeed (uint feedNr, uint startFrame =0)
        {
          return feed(feedNr).allBlocks(startFrame);
        }
    };
  
  inline OutputDiagnostic
  watch (OutputSlot& theSlot)
  {
    return OutputDiagnostic{static_cast<DiagnosticOutputSlot&> (theSlot)};
  }
  
  
  
}} // namespace vault::out
#endif /*VAULT_OUT_DIAGNOSTIC_OUTPUT_SLOT_H*/
