/*
  DUMMY-BUILDER-CONTEXT.hpp  -  fake simulated builder result for player tests

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
 ** @todo this was invented in 2012 -- but development of the player subsystem stalled thereafter.
 ** 
 ** @see mock-dispatcher.hpp
 ** @see JobPlanningPipeline_test
 */


#ifndef STEAM_PLAY_DUMMY_BUILDER_CONTEXT_H
#define STEAM_PLAY_DUMMY_BUILDER_CONTEXT_H


#include "steam/mobject/model-port.hpp"
#include "steam/fixture/model-port-registry.hpp"
#include "steam/play/output-slot-connection.hpp"
#include "steam/play/output-manager.hpp"
#include "steam/asset/timeline.hpp"
#include "steam/asset/pipe.hpp"
#include "common/query.hpp"

#include "lib/iter-source.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <vector>


namespace steam{
namespace play {
namespace test {
  
  using fixture::ModelPortRegistry;
  
  
  /**
   * @todo 5/2023 quick-n-dirty placeholder to be able to fabricate fake DataSink handles (`Handle<Connection>`)
   */
  class UnimplementedConnection
    : public play::OutputSlot::Connection
    {
      BuffHandle claimBufferFor(FrameID) override { UNIMPLEMENTED ("claimBufferFor(FrameID)");      }
      bool isTimely (FrameID, TimeValue) override { return true;                                    }
      void transfer (BuffHandle const&)  override { UNIMPLEMENTED ("transfer (BuffHandle const&)"); }
      void pushout  (BuffHandle const&)  override { UNIMPLEMENTED ("pushout  (BuffHandle const&)"); }
      void discard  (BuffHandle const&)  override { UNIMPLEMENTED ("discard  (BuffHandle const&)"); }
      void shutDown ()                   override { UNIMPLEMENTED ("shutDown() Connection");        }
      
    public:
     ~UnimplementedConnection();
      UnimplementedConnection()  = default;
    };
  
  
  using asset::Pipe;
  using asset::PPipe;
  using asset::Struct;
  using asset::Timeline;
  using asset::PTimeline;
  using mobject::ModelPort;
  using fixture::ModelPortRegistry;
  using util::contains;
  using lumiera::Query;
  
  
  using PID = asset::ID<Pipe>;
  using TID = asset::ID<Struct>;
  
  using ModelPorts = lib::IterSource<mobject::ModelPort>::iterator;
  using DummyOutputLink = std::pair<mobject::ModelPort, play::DataSink>;
  
  
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
          dataSinks_.emplace_back().activate(std::make_shared<UnimplementedConnection>());
          dataSinks_.emplace_back().activate(std::make_shared<UnimplementedConnection>());
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
    };
  
  
}}} // namespace steam::play::test
#endif /*STEAM_PLAY_DUMMY_BUILDER_CONTEXT_H*/
