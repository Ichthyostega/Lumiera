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


#ifndef PROC_PLAY_DIAGNOSTIC_OUTPUT_SLOT_H
#define PROC_PLAY_DIAGNOSTIC_OUTPUT_SLOT_H


#include "lib/error.hpp"
#include "include/logging.h"
#include "proc/play/output-slot.hpp"
#include "proc/play/output-slot-connection.hpp"
#include "proc/asset/meta/time-grid.hpp"
#include "proc/engine/buffhandle.hpp"
#include "proc/engine/tracking-heap-block-provider.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/scoped-ptrvect.hpp"
#include "lib/iter-source.hpp"
#include "lib/util.hpp"
#include "proc/engine/testframe.hpp"
//#include "lib/sync.hpp"

#include <boost/noncopyable.hpp>
//#include <string>
//#include <vector>
#include <tr1/unordered_set>
#include <tr1/memory>
//#include <boost/scoped_ptr.hpp>


namespace proc {
namespace play {

//using std::string;
  using util::contains;
  using lib::time::FrameRate;
  using proc::asset::meta::PGrid;
  using proc::asset::meta::TimeGrid;
  using proc::engine::BufferDescriptor;
  using proc::engine::test::TestFrame;
  using proc::engine::TrackingHeapBlockProvider;
  namespace diagn = proc::engine::diagn;

//using std::vector;
  using std::tr1::shared_ptr;
//using boost::scoped_ptr;
  
  /** 
   * Diagnostic output connection for a single channel,
   * allowing to track generated frames and verify 
   * the processing protocol for output buffers.
   */
  class TrackingInMemoryBlockSequence
    : public OutputSlot::Connection
    {
      
      typedef std::tr1::unordered_set<FrameID> FrameTrackingInfo;
      
      
      shared_ptr<TrackingHeapBlockProvider> buffProvider_;
      BufferDescriptor bufferType_;
      
      FrameTrackingInfo frameTrackingIndex_;
      PGrid frameGrid_;
      
      
      BuffHandle
      trackFrame (FrameID frameNr, BuffHandle const& newBuffer)
        {
          REQUIRE (contains (frameTrackingIndex_,frameNr),
                   "attempt to lock already used frame %lu", frameNr);
          
          frameTrackingIndex_.insert (frameNr);
          return newBuffer;
        }
      
      TimeValue
      deadlineFor (FrameID frameNr)
        {
          return frameGrid_->timeOf (frameNr);
        }
      
      
      
      
      /* === Connection API === */
      
      BuffHandle
      claimBufferFor(FrameID frameNr) 
        {
          return trackFrame (frameNr,
                             buffProvider_->lockBuffer (bufferType_));
        }
      
      
      bool
      isTimely (FrameID frameNr, TimeValue currentTime)
        {
          if (Time::ANYTIME == currentTime)
            return true;
          
          return currentTime < deadlineFor (frameNr);
        }
      
      void
      transfer (BuffHandle const& filledBuffer)
        {
          pushout (filledBuffer);
        }
      
      void
      pushout (BuffHandle const& data4output)
        {
          buffProvider_->emitBuffer   (data4output);
          buffProvider_->releaseBuffer(data4output);
        }
      
      void
      discard (BuffHandle const& superseededData)
        {
          buffProvider_->releaseBuffer (superseededData);
        }
      
      void
      shutDown ()
        {
          buffProvider_.reset();
        }
      
    public:
      TrackingInMemoryBlockSequence()
        : buffProvider_(new TrackingHeapBlockProvider())
        , bufferType_(buffProvider_->getDescriptor<TestFrame>())
        , frameTrackingIndex_()
        , frameGrid_(TimeGrid::build ("DiagnosticOutputSlot-buffer-grid", FrameRate::PAL))  /////////////TODO should rather pass that in as part of a "timings" definition
        {
          INFO (engine_dbg, "building in-memory diagnostic output sequence");
        }
      
      virtual
     ~TrackingInMemoryBlockSequence()
        {
          INFO (engine_dbg, "releasing diagnostic output sequence");
        }
      
      
      /* === Diagnostic API === */
      
      TestFrame*
      accessEmittedFrame (uint frameNr)  const
        {
          REQUIRE (buffProvider_);
          if (frameNr <= buffProvider_->emittedCnt())
            return &buffProvider_->accessAs<TestFrame> (frameNr);
          else
            return 0;                                               ////////////////////////////////TICKET #856
        }
      
      diagn::Block*
      accessEmittedBuffer (uint bufferNr)  const
        {
          REQUIRE (buffProvider_);
          if (bufferNr <= buffProvider_->emittedCnt())
            return & buffProvider_->access_emitted (bufferNr);
          else
            return 0;
        }
      
      bool
      wasAllocated (uint frameNr)  const
        {
          REQUIRE (buffProvider_);
          return contains (frameTrackingIndex_, frameNr);
        }
    };
  
  
  /**
   * Special diagnostic connection state implementation,
   * establishing diagnostic output connections for each channel,
   * thus allowing to verify the handling of individual buffers
   */
  class SimulatedOutputSequences
    : public ConnectionStateManager<TrackingInMemoryBlockSequence>
    {
      TrackingInMemoryBlockSequence
      buildConnection()
        {
          return TrackingInMemoryBlockSequence();
        }
      
    public:
      SimulatedOutputSequences (uint numChannels)
        {
          init (numChannels);
        }
    };
  
    
    
    
  
  /********************************************************************
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
      
      static const uint MAX_CHANNELS = 5;
      
      /** @note a real OutputSlot implementation
       * would rely on some kind of embedded
       * configuration here */
      uint
      getOutputChannelCount()
        {
          return MAX_CHANNELS;
        }
      
      
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
          return static_cast<SimulatedOutputSequences&> (*state_).at(channel);
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
        : public lib::IterSource<TestFrame>
        , boost::noncopyable
        {
          TrackingInMemoryBlockSequence const& outSeq_;
          uint currentFrame_;
          
          
          virtual Pos
          firstResult ()
            {
              REQUIRE (0 == currentFrame_);
              return outSeq_.accessEmittedFrame (currentFrame_);
            }
          
          virtual void
          nextResult (Pos& pos)
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
      buffer_was_used (uint channel, FrameID frame)
        {
          return accessSequence(channel)
                   .wasAllocated(frame);
        }
      
      
      bool
      buffer_unused   (uint channel, FrameID frame)
        {
          return !buffer_was_used(channel, frame);
        }
      
      
      bool
      buffer_was_closed (uint channel, FrameID frame)
        {
          diagn::Block* block = accessSequence(channel)
                                  .accessEmittedBuffer(frame);
          return block
              && block->was_closed();
        }
      
      
      bool
      emitted (uint channel, FrameID frame)
        {
          diagn::Block* block = accessSequence(channel)
                                  .accessEmittedBuffer(frame);
          return block
              && block->was_used();
        }
      
      
      
    private:
      
    };
  
  
  
}} // namespace proc::play
#endif
