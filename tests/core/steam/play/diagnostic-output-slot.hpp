/*
  DIAGNOSTIC-OUTPUT-SLOT.hpp  -  helper for testing against the OutputSlot interface

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

/** @file diagnostic-output-slot.hpp
 ** An facility for writing unit-tests against the OutputSlot interface.
 **
 ** @see output-slot-protocol-test.cpp
 */


#ifndef STEAM_PLAY_DIAGNOSTIC_OUTPUT_SLOT_H
#define STEAM_PLAY_DIAGNOSTIC_OUTPUT_SLOT_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "include/logging.h"
#include "steam/play/output-slot.hpp"
#include "steam/play/output-slot-connection.hpp"
#include "steam/asset/meta/time-grid.hpp"
#include "steam/engine/buffhandle.hpp"
#include "steam/engine/tracking-heap-block-provider.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/scoped-ptrvect.hpp"
#include "lib/iter-source.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"
#include "common/advice.hpp"
#include "steam/engine/testframe.hpp"
//#include "lib/sync.hpp"

//#include <string>
//#include <vector>
#include <unordered_set>
#include <memory>


namespace steam {
namespace play {

//using std::string;
  using lib::Symbol;
  using util::unConst;
  using util::contains;
  using lib::time::FrameRate;
  using steam::asset::meta::PGrid;
  using steam::asset::meta::TimeGrid;
  using steam::engine::BufferDescriptor;
  using steam::engine::test::TestFrame;
  using steam::engine::TrackingHeapBlockProvider;
  namespace diagn = steam::engine::diagn;

//using std::vector;
  using std::shared_ptr;
  
  namespace { // diagnostics & internals....
    
    inline PGrid
    getTestTimeGrid()
    {
      Symbol gridID("DiagnosticOutputSlot-buffer-grid");
      lumiera::advice::Request<PGrid> query4grid(gridID) ;
      PGrid testGrid25 = query4grid.getAdvice();
      
      if (!testGrid25)
        testGrid25 = TimeGrid::build (gridID, FrameRate::PAL);
      
      ENSURE (testGrid25);
      return testGrid25;
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
      
      
      TrackingHeapBlockProvider buffProvider_;
      BufferDescriptor bufferType_;
      
      FrameTrackingInfo frameTrackingIndex_;
      PGrid frameGrid_;
      
      bool closed_;
      
      
      BuffHandle
      trackFrame (FrameID frameNr, BuffHandle const& newBuffer)
        {
          TRACE (test, "Con=%p : track buffer %zu for frame-#%lu"
                     , this, newBuffer.entryID(), frameNr);
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
      
      BuffHandle
      claimBufferFor(FrameID frameNr) 
        {
          TRACE (test, "Con=%p : lock buffer for frame-#%lu", this, frameNr);
          REQUIRE (!closed_);
          
          return trackFrame (frameNr,
                             buffProvider_.lockBuffer (bufferType_));
        }
      
      
      bool
      isTimely (FrameID frameNr, TimeValue currentTime)
        {
          TRACE (test, "Con=%p : timely? frame-#%lu"
                     , this, frameNr);
          
          if (Time::ANYTIME == currentTime)
            return true;
          else
            return currentTime < deadlineFor (frameNr);
        }
      
      void
      transfer (BuffHandle const& filledBuffer)
        {
          TRACE (test, "Con=%p : transfer buffer %zu"
                     , this, filledBuffer.entryID());
          REQUIRE (!closed_);
          
          pushout (filledBuffer);
        }
      
      void
      pushout (BuffHandle const& data4output)
        {
          REQUIRE (!closed_);
          buffProvider_.emitBuffer   (data4output);
          buffProvider_.releaseBuffer(data4output);
        }
      
      void
      discard (BuffHandle const& superseededData)
        {
          REQUIRE (!closed_);
          buffProvider_.releaseBuffer (superseededData);
        }
      
      void
      shutDown ()
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
          if (frameNr < buffProvider_.emittedCnt())
            return & accessFrame(frameNr);
          else
            return 0;                                               ////////////////////////////////TICKET #856
        }
      
      diagn::Block const *
      accessEmittedBuffer (uint bufferNr)  const
        {
          if (bufferNr < buffProvider_.emittedCnt())
            return & accessBlock(bufferNr);
          else
            return 0;
        }
      
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
          return unConst(this)->buffProvider_.accessAs<TestFrame> (frameNr);
        }
      
      diagn::Block const&
      accessBlock (uint bufferNr)  const
        {
          return unConst(this)->buffProvider_.access_emitted (bufferNr);
        }
    };
  
  
  
  
  
  
  
  
  
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
    : public OutputSlotImplBase
    {
      
      static const uint MAX_CHANNELS = 5;
      
      /** @note a real OutputSlot implementation
       * would rely on some kind of embedded
       * configuration here */
      uint
      getOutputChannelCount()
        {
          return MAX_CHANNELS;
        }
      
      
      /**
       * Special diagnostic connection state implementation,
       * establishing diagnostic output connections for each channel,
       * thus allowing to verify the handling of individual buffers
       */
      class SimulatedOutputSequences
        : public ConnectionManager<TrackingInMemoryBlockSequence>
        {
          typedef ConnectionManager<TrackingInMemoryBlockSequence> _Base;
          
          void
          buildConnection(ConnectionStorage storage)
            {
              storage.create<TrackingInMemoryBlockSequence>();
            }
          
        public:
          SimulatedOutputSequences (uint numChannels)
            : _Base(numChannels)
            {
              init();
            }
        };
      
      
      
      /** hook into the OutputSlot frontend */
      ConnectionState*
      buildState()
        {
          return new SimulatedOutputSequences(
                        getOutputChannelCount());
        }
        
      /** @internal is self-managed and non-copyable.
       * Clients use #build() to get an instance */
      DiagnosticOutputSlot() { }
      
      /** @internal access the implementation object
       * representing a single stream connection
       */
      TrackingInMemoryBlockSequence const&
      accessSequence (uint channel)
        {
          REQUIRE (!isFree(), "diagnostic OutputSlot not (yet) connected");
          REQUIRE (channel <= getOutputChannelCount());
          return static_cast<TrackingInMemoryBlockSequence&> (state_->access (channel));
        }
      
      
    public:
      /** build a new Diagnostic Output Slot instance,
       *  discard the existing one. Use the static query API
       *  for investigating collected data. */
      static OutputSlot&
      build()
        {
          static lib::ScopedPtrVect<OutputSlot> diagnosticSlots;
          return diagnosticSlots.manage(new DiagnosticOutputSlot);
        }
      
      static DiagnosticOutputSlot&
      access (OutputSlot& to_investigate)
        {
          return dynamic_cast<DiagnosticOutputSlot&> (to_investigate);
        }
      
      
      
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
          REQUIRE (channel < MAX_CHANNELS);
          return OutputFramesLog::build(
              new OutputFramesLog (
                  accessSequence(channel)));
        }
      
      
      bool
      frame_was_allocated (uint channel, FrameID nominalFrame)
        {
          return accessSequence(channel)
                   .wasAllocated(nominalFrame);
        }
      
      
      bool
      output_was_emitted (uint channel, FrameID outputFrame)
        {
          diagn::Block const *block = accessSequence(channel)
                                        .accessEmittedBuffer(outputFrame);
          return block
              && block->was_used();
        }
      
      
      bool
      output_was_closed (uint channel, FrameID outputFrame)
        {
          diagn::Block const *block = accessSequence(channel)
                                        .accessEmittedBuffer(outputFrame);
          return block
              && block->was_closed();
        }
    };
  
  
  
}} // namespace steam::play
#endif
