/*
  DiagnosticOutputSlot  -  helper for testing against the OutputSlot interface

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file diagnostic-output-slot.cpp
 ** Internal implementation parts of a state-tracking dummy implementation
 ** of the OutputSlot interface and framework. This setup uses the standard
 ** base implementation of OutputSlot and layers a diagnostic context on
 ** top, using a _PImpl_ — which in this case is a shared-ptr to a heap
 ** allocated OutputTracker object. Furthermore, the basic OutputSlot
 ** was configured at construction time to create instances of
 ** DummyConnection, which are wired internally to the OutputTracker.
 ** When the client invokes one of the DataSink handles with a frame number,
 ** this call is propagated through the default implementation of OutputSlot
 ** and ends up as invocation of OutputSlot::Connection::claimBufferFor(FrameID).
 ** The DummyConnection implements this call by retrieving a new buffer from the
 ** default BufferProvider (which is always a Test setup, since the real Render
 ** Engine uses a specifically configured BufferProvider, injected through the
 ** steam::engine::EngineCtx). Notably, this (default) test implementation of
 ** the BufferProvider framework will actually never discard any allocated
 ** memory — which is an essential prerequisite for this DiagnosticOutputSlot
 ** implementation, insofar it allows to access the buffer memory even after
 ** it has been released, officially.
 ** 
 ** The OutputTracker establishes a vault::out::diagn::FrameInfo record for each
 ** distinct frame number encountered on each activated output feed. In this
 ** diagnostic record, a timestamp is recorded for each of the three lifecycle
 ** steps (lock, emit, release), together with the size and the actual memory
 ** address of the data buffer handed out to the client. The OutputDiagnostic
 ** wrapper adds some convenience access functions, allowing to query if (and when)
 ** a frame went through some lifecycle step, and to traverse all recorded
 ** FrameInfo records for each data feed.
 ** @note \ref OutputSlotProtocol_test::verifyStandardCase() uses this setup
 **       to walk through the standard stages of the _»Output Slot Protocol«_,
 **       which also validates the default implementation of OutputSlot::AllocState.
 */


#include "lib/integral.hpp"
#include "lib/nocopy.hpp"
#include "vault/out/diagnostic-output-slot.hpp"
#include "vault/out/output-slot-connection.hpp"
#include "vault/mem/naive-buffer-setup.hpp"
#include "vault/mem/buffhandle.hpp"
#include "vault/real-clock.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/util.hpp"

#include <vector>
#include <utility>
#include <algorithm>
#include <functional>
#include <tuple>
#include <map>

using util::contains;
using util::isSameAdr;
using std::vector;
using std::byte;
using std::move;
using std::make_unique;
using std::make_shared;
using std::function;

using vault::mem::NaiveBufferSetup;
using vault::mem::BuffHandle;
using vault::mem::BuffDescr;
using vault::mem::Buff;

namespace vault {
namespace out   {
  namespace err = lumiera::error;
  
  namespace { // Implementation details of tracking
    inline void
    zeroFill (void* buf, size_t siz)
    {
      auto begin = static_cast<byte*> (buf);
      std::fill (begin, begin+siz, byte(0));
    }
  }//(End)Impl details
  
  
  /**
   * @internal the diagnostic tracking PImpl
   * @remark this defines the _actual identity_ of an DiagnostricOutputSlot
   *         and is also the place where all observed data is collected  */
  class DiagnosticOutputSlot::OutputTracker
    : public Config
    , util::NonCopyable
    {
      std::vector<diagn::FeedLog> feed_;
      NaiveBufferSetup buffProvider_;
      BuffDescr bufferType_;
      

    public:
      OutputTracker (Config&& config)
        : Config{move (config)}
        , feed_{config.numDataFeeds}
        , buffProvider_{}
        , bufferType_{buffProvider_.getDescriptorFor (bufferSize
                                                     ,mem::TypeHandler{[siz=bufferSize](void* buff){ zeroFill (buff, siz);}
                                                                      ,[](void*){ /* do nothing on release */   }
                                                                      })}
        { }
        
        /** configurable functor: what is used as _current time_
         * @remark allows the test framework to mark "now" as some
         *         specific fixed time; default is to return system time
         */
        function<Time()> currentTime{[]{ return RealClock::now(); }};
        
        
        BuffHandle
        getNewBuffer()
          {
            return bufferType_.lockBuffer();
          }
        
        void
        recordClaim (uint feedNr, FrameID frame, Buff* buff)
          {
            diagn::FrameInfo& frameInfo = feed_[feedNr].recorded[frame];
            frameInfo.buffSize = bufferSize;
            frameInfo.storage = buff;
            frameInfo.locked = currentTime();
          }
        
        void
        recordPublish (uint feedNr, FrameID frame, Buff* buff)
          {
            diagn::FrameInfo& frameInfo = feed_[feedNr].recorded[frame];
            REQUIRE (isSameAdr(frameInfo.storage, buff));
            frameInfo.emitted = currentTime();
          }
        
        void
        recordRelease (uint feedNr, FrameID frame, Buff* buff)
          {
            diagn::FrameInfo& frameInfo = feed_[feedNr].recorded[frame];
            REQUIRE (isSameAdr(frameInfo.storage, buff));
            frameInfo.released = currentTime();
          }
        
        
        /* ===== diagnostic functions  ===== */
        
        /** skim through all recorded FrameInfo, over all feeds */
        auto
        allBlocks()  const
          {
            return lib::explore(feed_)
                      .transform([](diagn::FeedLog const& feedLog){ return feedLog.allRecordedBlocks(); })
                      .flatten();
          }
        
        diagn::FeedLog const&
        getFeed (uint feedNr)  const
          {
            REQUIRE (feedNr < feed_.size());
            return feed_[feedNr];
          }
    };
  
  /**
   * Front-end for access by test code.
   * @note tolerates access to arbitrary feeds
   */
  diagn::FeedLog const&
  OutputDiagnostic::feed (uint feedNr)
  {
    if (feedNr < dos_.tracker_->numDataFeeds)
      return dos_.tracker_->getFeed (feedNr);
    else
      return lib::NullValue<diagn::FeedLog>::get();
  }

  uint
  OutputDiagnostic::cntLocked()
  {
    return dos_.tracker_->allBlocks()
                            .filter([](diagn::FrameInfo const& frame){ return frame.wasLocked(); })
                            .count();
  }
  
  uint
  OutputDiagnostic::cntEmitted()
  {
    return dos_.tracker_->allBlocks()
                            .filter([](diagn::FrameInfo const& frame){ return frame.wasEmitted(); })
                            .count();
  }
  
  uint
  OutputDiagnostic::cntReleased()
  {
    return dos_.tracker_->allBlocks()
                            .filter([](diagn::FrameInfo const& frame){ return frame.wasReleased(); })
                            .count();
  }
  
  
  
  
  
  /**
   * An instrumented dummy data output connection.
   * By default, it supports an unlimited number of frames
   * and exposes a new buffer for each different frame requested.
   * @remark the implementation uses an index table to detect if
   *         buffer memory address passed back from the client
   *         was actually a buffer handed out by this connection,
   *         and to retrieve the associated BuffHandle that was used
   *         to get this buffer from a backing memory manager.
   */
  class DiagnosticOutputSlot::DummyConnection
    : public OutputSlot::Connection
    {
      OutputTracker& tracker_;
      
      using Claim = std::tuple<FrameID,BuffHandle>;
      using BuffIdx = std::map<Buff*, Claim>;
      
      BuffIdx buffIdx_;
      uint feedNr_;
      
      
      /* === Connection API === */
      
      size_t
      getBufferSize()  const override
        {
          return tracker_.bufferSize;
        }
      
      Buff*
      claimBufferFor(FrameID frame)  override
        {
          BuffHandle handle = tracker_.getNewBuffer();
          REQUIRE (handle.isValid());
          Buff* buff = handle.rawStorage();
          ENSURE (not contains (buffIdx_, buff));
          buffIdx_.insert ({buff, Claim{frame,handle}});
          tracker_.recordClaim (feedNr_,frame, buff);
          return buff;
        }
      
      void
      publish (Buff* buff)  override
        {
          REQUIRE (contains (buffIdx_,buff));
          auto [frame,handle] = buffIdx_.find(buff)->second;
          handle.emit();
          tracker_.recordPublish (feedNr_,frame, buff);
        }
      
      void
      release (Buff* buff)  override
        {
          REQUIRE (contains (buffIdx_,buff));
          auto [frame,handle] = buffIdx_.find(buff)->second;
          handle.release();
          tracker_.recordRelease (feedNr_,frame, buff);
        }
      
      void
      shutDown()  override
        {
          TODO ("Anything to record at connection shut-down?");
        }
      
      
    public:
      DummyConnection (OutputTracker& tracker, uint thisFeed)
        : tracker_{tracker}
        , feedNr_{thisFeed}
        { }
    };
  
  
  
  
  DiagnosticOutputSlot::~DiagnosticOutputSlot() { }  // emit destructors for hidden implementation here....
  
  DiagnosticOutputSlot::DiagnosticOutputSlot (Config config)
    : DiagnosticOutputSlot{setupOutputTracker(move (config))}
    { }
  
  /**
   * @internal setup and construction sequence for the diagnostic tracking.
   * This tricky call sequence is required to resolve two conflicting goals
   * - we want a complete one-shot setup and wiring in the constructor.
   * - we want DiagnosticOutputSlot be usable like an OutputSlot,
   *   even when this implies to create a slicing copy (its a value object!)
   * These requirements imply that OutputSlot should be the (first) base object,
   * but should also be initialised with the OutputTracker, which however can
   * only be initialised _after_ the base object.
   * 
   * Yet the OutputTracker is installed as a »PImpl« and managed by a shared-ptr anyway,
   * so that this contradiction can be circumvented by constructing it first, as a constructor
   * argument \a trackingSetup prepared through a chained constructor call. It is thus visible
   * in the complete scope of the constructor, so that we can pass it to the base class
   * constructor, and later then move it into the final location in the derived class.
   * 
   * @note \ref setupTrackingConnections performs that kind of wiring,
   *       that usually would be done within an OutputManager: it prepares a set of
   *       OutputSlot::Connection objects, which however are implemented as DummyConnection here,
   *       and back-wired internally to the OututTracker, so that it is possible to verify
   *       the invocations that actually happened while using the DiagnosticOutputSlot.
   */
  DiagnosticOutputSlot::DiagnosticOutputSlot (shared_ptr<OutputTracker> trackingSetup)
    : OutputSlot{setupTrackingConnections (*trackingSetup)}
    , tracker_{move(trackingSetup)}
    { }
  
  shared_ptr<DiagnosticOutputSlot::OutputTracker>
  DiagnosticOutputSlot::setupOutputTracker (Config&& config)
  {
    return make_shared<OutputTracker> (move (config));
  }
  
  unique_ptr<OutputSlot::Allocation>
  DiagnosticOutputSlot::setupTrackingConnections (DiagnosticOutputSlot::OutputTracker& outputTracker)
  {
    constexpr bool SIMPLE_METADATA_TABLE_FOR_TEST{true};
    
    using AllocState = OutputSlot::AllocState<DummyConnection, SIMPLE_METADATA_TABLE_FOR_TEST>;
    using ConStorage = AllocState::ConnectionStorage;
    
    return make_unique<AllocState> (outputTracker.numDataFeeds
                                   ,[&,i=0](ConStorage& storage) mutable { storage.create<DummyConnection> (outputTracker, i++); }
                                   );                        //  a »population functor« that creates a sequence of DummyConnections
  }
  
  
  /**
   * @param timePoint reference to a "current" time point,
   *        that can be manipulated from the test setup.
   */
  void
  DiagnosticOutputSlot::fixCurrentTime (TimeVar const& timePoint)
  {
    REQUIRE (tracker_);
    tracker_->currentTime = [&timePoint] ->Time { return timePoint; };
  }
  
  
}} // namespace vault::out
