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

    inline PGrid
    getTestTimeGrid()
    {
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : disabled code to disentangle BufferProvider implementation
      Symbol gridID("DiagnosticOutputSlot-buffer-grid");
      vessel::advice::Request<PGrid> query4grid(gridID) ;
      PGrid testGrid25 = query4grid.getAdvice();
      
      if (!testGrid25)
        testGrid25 = TimeGrid::build (gridID, FrameRate::PAL);
      
      ENSURE (testGrid25);
#endif  /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : (end) disabled code
      PGrid testGrid; ////////OOO Kaboooom!
      return testGrid;
    }
  }
  
  
  /**
   * Diagnostic output connection for a single channel,
   * allowing to track generated frames and verify
   * the processing protocol for output buffers.
   */
  class TrackingInMemoryBlockSequence
    : public OutputSlot::Connection
    , util::NonCopyable
    {
      
      typedef std::unordered_set<FrameID> FrameTrackingInfo;
      
      
      NaiveBufferSetup buffProvider_;
      BuffDescr bufferType_;
      
      FrameTrackingInfo frameTrackingIndex_;
      PGrid frameGrid_;
      
      bool closed_;
      
      
      BuffHandle
      trackFrame (FrameID frameNr, BuffHandle const& newBuffer)
        {
          TRACE (test, "Con=%p : track buffer %zu for frame-#%lu"
                     , this, HashVal(newBuffer), frameNr);
          REQUIRE (!contains (frameTrackingIndex_,frameNr),
                   "attempt to lock already used frame %lu", frameNr);
          
          frameTrackingIndex_.insert (frameNr);
          return newBuffer;
        }
      
      TimeValue
      deadlineFor (FrameID frameNr)
        {
          // a real world implementation
          // would offset by a latency here
          return frameGrid_->timeOf (frameNr);
        }
      
      
      
      
      /* === Connection API === */
      
      size_t
      getBufferSize()  const override
        {
          UNIMPLEMENTED ("wtf buff?");
        }
      
      Buff*
      claimBufferFor (FrameID frameNr)  override
        {
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : disabled code due to OutputSlot refactoring
          TRACE (test, "Con=%p : lock buffer for frame-#%lu", this, frameNr);
          REQUIRE (!closed_);
          
          return trackFrame (frameNr,
                             buffProvider_.lockBuffer (bufferType_));
#endif  /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : (end) disabled code
        }
      
      void
      publish (Buff* filledBuffer)  override
        {
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : disabled code due to OutputSlot refactoring
          TRACE (test, "Con=%p : transfer buffer %zu"
                     , this, HashVal(filledBuffer));
          REQUIRE (!closed_);
          
          pushout (filledBuffer);
#endif  /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : (end) disabled code
        }
      
      void
      release (Buff* data4output)  override
        {
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : disabled code due to OutputSlot refactoring
          REQUIRE (!closed_);
          buffProvider_.emitBuffer   (data4output);
          buffProvider_.releaseBuffer(data4output);
#endif  /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : (end) disabled code
        }
      
      void
      shutDown()  override
        {
          closed_ = true;
        }
      
    public:
      TrackingInMemoryBlockSequence()
        : buffProvider_()
        , bufferType_(buffProvider_.getDescriptor<TestFrame>())
        , frameTrackingIndex_()
        , frameGrid_(getTestTimeGrid())                              /////////////TODO should rather pass that in as part of a "timings" definition
        , closed_(false)
        {
          INFO (engine_dbg, "building in-memory diagnostic output sequence (at %p)", this);
        }
      
      virtual
     ~TrackingInMemoryBlockSequence()
        {
          INFO (engine_dbg, "releasing diagnostic output sequence (at %p)", this);
        }
      
      
      /* === Diagnostic API === */
      
      TestFrame const *
      accessEmittedFrame (uint frameNr)  const
        {
          UNIMPLEMENTED ("suitable diagnostic API");
//        if (frameNr < buffProvider_.emittedCnt())      ///////////////////////////////OOO provide suitable diagnostic API!
//          return & accessFrame(frameNr);
//        else
//          return 0;                                               ////////////////////////////////TICKET #856
        }
      
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : disabled code to disentangle BufferProvider implementation
      diagn::Block const *
      accessEmittedBuffer (uint bufferNr)  const
        {
          UNIMPLEMENTED ("suitable diagnostic API");
//        if (bufferNr < buffProvider_.emittedCnt())      ///////////////////////////////OOO provide suitable diagnostic API!
//          return & accessBlock(bufferNr);
//        else
//          return 0;
        }
#endif  /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : (end) disabled code
      
      bool
      wasAllocated (uint frameNr)  const
        {
          TRACE (test, "query wasAllocated. Con=%p", this);
          
          return contains (frameTrackingIndex_, frameNr);
        }
      
    private:
      TestFrame const&
      accessFrame (uint frameNr)  const
        {
          UNIMPLEMENTED ("suitable diagnostic API");
//        return unConst(this)->buffProvider_.accessAs<TestFrame> (frameNr);      ///////////////////////////////OOO provide suitable diagnostic API!
        }
      
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : disabled code to disentangle BufferProvider implementation
      diagn::Block const&
      accessBlock (uint bufferNr)  const
        {
          UNIMPLEMENTED ("suitable diagnostic API");
//        return unConst(this)->buffProvider_.access_emitted (bufferNr);      ///////////////////////////////OOO provide suitable diagnostic API!
        }
#endif  /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : (end) disabled code
    };
  
  
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
      
      static const uint MAX_CHANNELS = 5; ///////////////////////////////OOO Build heap-based implementation with arbitrary number of feeds!!
      
      /** @note a real OutputSlot implementation
       * would rely on some kind of embedded
       * configuration here */
      uint
      getOutputChannelCount()
        {
          return MAX_CHANNELS; /////////////OOO No!
        }
      
      
      /**
       * Special diagnostic connection state implementation,
       * establishing diagnostic output connections for each channel,
       * thus allowing to verify the handling of individual buffers
       */
      class SimulatedOutputSequences
        : public AllocState<TrackingInMemoryBlockSequence>
        {
          using _Base = AllocState<TrackingInMemoryBlockSequence>;
          
        public:
          SimulatedOutputSequences (uint numChannels)
            : _Base{ numChannels
                   ,[](ConnectionStorage& storage)
                      {
                        UNIMPLEMENTED ("new Tracking implementation for DiagnosticOutputSlot");
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : disabled code to disentangle BufferProvider implementation
                        storage.create<TrackingInMemoryBlockSequence>();
#endif  /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : (end) disabled code
                      }}
            { }
        };
      
      
      
      /** hook into the OutputSlot frontend */
      unique_ptr<Allocation>
      buildState()
        {
          return make_unique<SimulatedOutputSequences> (
                        getOutputChannelCount());
        }
        
      
      /** @internal access the implementation object
       * representing a single stream connection
       */
      TrackingInMemoryBlockSequence const&
      accessSequence (uint channel)
        {
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : disabled code to disentangle BufferProvider implementation
          REQUIRE (!isFree(), "diagnostic OutputSlot not (yet) connected");
          REQUIRE (channel <= getOutputChannelCount());
//        return static_cast<TrackingInMemoryBlockSequence&> (state_->access (channel));
#endif  /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : (end) disabled code
          UNIMPLEMENTED ("the old tracking API is abandoned, the new one not yet implemented");
        }
      
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
      
      
      /* === diagnostics API === */
      
      /**
       * diagnostic facility to verify test data frames
       * written to this Test/Dummy "output". It exposes
       * the emitted Data as a sequence of TestFrame objects.
       */
      class OutputFramesLog
        : public lib::IterSource<const TestFrame>
        , util::NonCopyable
        {
          TrackingInMemoryBlockSequence const& outSeq_;
          uint currentFrame_;
          
          
          virtual Pos                                          //////////////////////////////////////////////TICKET #1125 : this API should use three control functions, similar to IterStateWrapper
          firstResult ()  override
            {
              REQUIRE (0 == currentFrame_);
              return outSeq_.accessEmittedFrame (currentFrame_);
            }
          
          virtual void
          nextResult (Pos& pos)  override
            {
              ++currentFrame_;
              pos = outSeq_.accessEmittedFrame(currentFrame_);
            }
          
          public:
            OutputFramesLog (TrackingInMemoryBlockSequence const& bs)
              : outSeq_(bs)
              , currentFrame_(0)
              { }
        };
        
      typedef OutputFramesLog::iterator OutFrames;
      
      
      OutFrames
      getChannel (uint channel)
        {
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : disabled code to disentangle BufferProvider implementation
          REQUIRE (channel < MAX_CHANNELS);
          return OutputFramesLog::build(
              new OutputFramesLog (
                  accessSequence(channel)));
#endif  /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : (end) disabled code
        }
      
      
      bool
      frame_was_allocated (uint channel, FrameID nominalFrame)
        {
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : disabled code to disentangle BufferProvider implementation
          return accessSequence(channel)
                   .wasAllocated(nominalFrame);
#endif  /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : (end) disabled code
        }
      
      
      bool
      output_was_emitted (uint channel, FrameID outputFrame)
        {
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : disabled code to disentangle BufferProvider implementation
          diagn::Block const *block = accessSequence(channel)
                                        .accessEmittedBuffer(outputFrame);
          return block
             and block->was_used();
#endif  /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : (end) disabled code
        }
      
      
      bool
      output_was_closed (uint channel, FrameID outputFrame)
        {
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : disabled code to disentangle BufferProvider implementation
          diagn::Block const *block = accessSequence(channel)
                                        .accessEmittedBuffer(outputFrame);
          return block
             and block->was_closed();
#endif  /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : (end) disabled code
        }
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1395 : draft compatible Tracking-API
      
      std::vector<diagn::FeedLog> feed_;
      
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
      
      auto const&
      feed (uint feedNr)
        {
          if (feedNr < dos_.feed_.size())
            return dos_.feed_[feedNr];
          else
            return lib::NullValue<diagn::FeedLog>::get();
        }
      
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
