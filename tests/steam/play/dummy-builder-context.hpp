/*
  DUMMY-BUILDER-CONTEXT.hpp  -  fake simulated builder result for player tests

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file dummy-builder-context.hpp
 ** Simulated result of a builder run for test purposes, without actual session and model content.
 ** Integration tests for the render engine or the player have to deal with a lot of prerequisites,
 ** since both an external environment and actual content form the Session is required to start any
 ** processing. Effectively the complete core application is required — while in fact the engine is
 ** built in an abstracted way and does not need to know much about the rest of the application.
 ** Thus, as a remedy, it is possible to establish a _faked context,_ exposing just those interfaces
 ** used by the engine for processing. As additional benefit, dedicated test operations can be rigged
 ** and verification routines can be used in place of actual media output.
 ** 
 ** This faked builder context provides a selection of consistent media element IDs with sensible
 ** properties and type markers, plus the attachment points for a simulated model structure, as
 ** if generated from an actual session model by a real Builder run.
 ** - a set of ModelPort elements
 ** - a set of DataSink handles, as if an actual playback connection has been established
 ** @todo 5/2023 add more elements as needed for test driven integration of Player and engine
 ** @todo this was invented in 2012 -- as an act of desperation, since everything to build upon
 **       seemed to evade my grip. My hope at that time was so somehow _bootstrap_ an explorative
 **       design in order to get away from that top-down reasoning, that seemed to lead nowhere.
 **       However, work on the player had to be postponed to care for the GUI first; meanwhile
 **       I follow a similar idea, relying on »vertical integration slices«
 ** @deprecated 3/2026 not sure if we need this henceforth...
 **       It might be useful though in case we consider to build some kind of
 **       _component integration test_ -- yet for this to work, we'd need
 **       - a fake calculation
 **       - a fake OutputSlot
 ** @todo 5/2026 now I have beefed up the OutputSlot so that it can receive
 **       two feeds with an arbitrary number of frames. Still not sure if it's needed;
 **       however it should be noted that the MockDispatcher relies on the DummyPlayConnection,
 **       which in turn uses SimulatedBuilderContext, mostly to get _just some model ports_.
 ** 
 ** @see mock-dispatcher.hpp
 ** @see JobPlanningPipeline_test
 */


#ifndef STEAM_PLAY_DUMMY_BUILDER_CONTEXT_H
#define STEAM_PLAY_DUMMY_BUILDER_CONTEXT_H


#include "lib/error.hpp"
#include "lib/format-string.hpp"
#include "steam/mobject/model-port.hpp"
#include "steam/fixture/model-port-registry.hpp"
#include "vault/out/output-slot-connection.hpp"
#include "steam/play/output-manager.hpp"
#include "steam/asset/timeline.hpp"
#include "steam/asset/pipe.hpp"
#include "vessel/query.hpp"

#include "lib/iter-explorer.hpp"
#include "lib/iter-source.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <vector>
#include <deque>
#include <array>


namespace steam{
namespace play {
namespace test {
  namespace err = lumiera::error;
  
  using fixture::ModelPortRegistry;
  using vault::mem::Buff;
  using vault::mem::BuffHandle;
  using vault::out::FrameID;
  using lib::time::TimeValue;
  using util::_Fmt;
  
  using DataSink = vault::out::OutputSlot::DataSink;
  
  
  
  
  /**
   * A placeholder/dummy OutputSlot that captures all
   * delivered output data into heap-allocated memory.
   * @todo 5/2026 it can now be created and allocates a fixed layout
   *       of buffer storage, which implies it should not be copied
   *       and is thus used differently than a real OutputSlot;
   *       should reconsider what is actually needed here!
   * @warning this used to be placeholder code, and now it was adapted
   *       to store frames, yet this new functionality is untested as of 5/2026
   */
  struct FakeOutput
    : OutputSlot
    , util::NonCopyable
    {
      static constexpr uint NUM_FEEDS = 2;
      static constexpr bool SINGLE_THREADED_TEST = true;
      
      using DummyDat = uint64_t;
      static constexpr size_t DUMMY_BUFF_SIZ = 1_KiB * sizeof(DummyDat);
      
      using DummyBuff = std::array<DummyDat, DUMMY_BUFF_SIZ>;
      using DummyBuffSeq = std::deque<DummyBuff>;

      /**
       * @todo 5/2026 quick-n-dirty placeholder code
       *  to expose some output buffers and provide
       *  the corresponding DataSink handles
       */
      class DummyOutputConnection
        : public vault::out::OutputSlot::Connection
        {
          DummyBuffSeq& buffers_;
          
          size_t getBufferSize()       const override { return DUMMY_BUFF_SIZ; }
          Buff* claimBufferFor (FrameID)     override;
          void publish  (Buff*)              override { /* fake "publish buffer content" */ }
          void release  (Buff*)              override { /* fake "complete buffer cycle"  */ }
          void shutDown ()                   override { /* fake "shutDown() Connection"  */ }
          
        public:
         ~DummyOutputConnection();
          DummyOutputConnection (DummyBuffSeq& buffers)
           : buffers_{buffers}
           { }
          // OutputSlot::Connection is non-copyable
        };
      
      using Connections = lib::ScopedCollection<DummyOutputConnection>;
      using MockConnectState = OutputSlot::AllocState<DummyOutputConnection, SINGLE_THREADED_TEST>;
      
      using OutBuffs = std::array<DummyBuffSeq, NUM_FEEDS>;
      OutBuffs buffers_{};
      
      FakeOutput()
        : OutputSlot{std::make_unique<MockConnectState> (NUM_FEEDS
                                                        ,[&,i=0](auto& storage) mutable
                                                                {
                                                                  storage.template create<DummyOutputConnection> (buffers_[i++]);
                                                                })}
        { }
    };
  
  inline Buff*
  FakeOutput::DummyOutputConnection::claimBufferFor (FrameID fra)
    {
      REQUIRE (fra >= 0);
      if (uint64_t(fra) >= buffers_.size())
        buffers_.resize(fra+10);
      return reinterpret_cast<Buff*> (& buffers_[fra]);
    }
  
  
  using asset::Pipe;
  using asset::PPipe;
  using asset::Struct;
  using asset::Timeline;
  using asset::PTimeline;
  using mobject::ModelPort;
  using fixture::ModelPortRegistry;
  using util::contains;
  using vessel::Query;
  
  
  using PID = asset::ID<Pipe>;
  using TID = asset::ID<Struct>;
  
  using ModelPorts = lib::IterSource<mobject::ModelPort>::iterator;
  using DummyOutputLink = std::pair<mobject::ModelPort, DataSink>;
  
  
  inline PID
  getPipe (string id)
  {
    return Pipe::query ("id("+id+")");
  }
  
  inline TID
  getTimeline (string id)
  {
    return asset::Struct::retrieve (Query<Timeline> ("id("+id+")"))->getID();
  }
  
  const uint NUMBER_OF_PORTS = 2;
  const string namePortA("bus-A");
  const string namePortB("bus-B");
  
  /**
   * helper for dummy render engine:
   * Simulate the result of a build process,
   * without actually running the builder.
   * Produces some mock pipes, model ports etc.
   */
  struct SimulatedBuilderContext
    {
      ModelPortRegistry registry_;
      ModelPortRegistry* existingRegistry_;
      
      std::vector<ModelPort> modelPorts_;
      std::vector<DataSink>  dataSinks_;
      
      FakeOutput fakeOutput_;
      
      /** setup */
      SimulatedBuilderContext()
        : registry_()
        , existingRegistry_(ModelPortRegistry::setActiveInstance (registry_))
        {
          performMockBuild();
        }
      
      /** tear-down */
     ~SimulatedBuilderContext()
        {
          if (existingRegistry_)
            ModelPortRegistry::setActiveInstance (*existingRegistry_);
          else
            ModelPortRegistry::shutdown();
        }
      
      void
      performMockBuild()
        {
          PID pipeA        = getPipe (namePortA);
          PID pipeB        = getPipe (namePortB);
          TID someTimeline = getTimeline ("dummy_Timeline");
          
          // start out with defining some new model ports......
          registry_.definePort (pipeA, someTimeline);
          registry_.definePort (pipeB, someTimeline);
          
          registry_.commit();
          
          // now "bus-A" and "bus-B" are known as model ports
          modelPorts_.push_back (ModelPort(pipeA));
          modelPorts_.push_back (ModelPort(pipeB));
          
          // prepare corresponding placeholder DataSink (a fake active output connection)
          dataSinks_.clear();
          lib::explore(fakeOutput_.getOpenedSinks())
              .effuse (dataSinks_);
        }
      
      
      ModelPorts
      getAllModelPorts()
        {
          return lib::iter_source::eachEntry (modelPorts_.begin(), modelPorts_.end());
        }
      
      DummyOutputLink
      getModelPort (uint index)
        {
          REQUIRE (index < modelPorts_.size());
          return {modelPorts_[index]
                 ,dataSinks_[index]
                 };
        }
      
      /**
       * Access a generated output frame.
       * @remark FakeOutput maintains Deque-Storage for each feed.
       * @tparam X data type to assume as content of the frame (only checked for size)
       * @param feed number of the output feed to access
       * @param frame frame numer within the output feed
       */
      template<typename X>
      X&
      accessOutputBufferAs (uint feed, FrameID frame)
        {
          static_assert (sizeof(X) <= FakeOutput::DUMMY_BUFF_SIZ
                        ,"desired data type would not fit into FakeOutput buffer");
          if (feed >= FakeOutput::NUM_FEEDS)
            throw err::Invalid{_Fmt{"Feed #%i > %i allowed"}
                                     % feed   % (FakeOutput::NUM_FEEDS-1)};
          
          auto availFrames = fakeOutput_.buffers_[feed].size();
          if (frame >= availFrames)
            throw err::Invalid{_Fmt{"Frame #%02i > %02i available frames"}
                                     % frame  % availFrames};
          
          return reinterpret_cast<X&> (fakeOutput_.buffers_[feed][frame]);
        }
    };
    
}}} // namespace steam::play::test
#endif /*STEAM_PLAY_DUMMY_BUILDER_CONTEXT_H*/
