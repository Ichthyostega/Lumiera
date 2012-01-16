/*
  RenderConfigurator  -  strategy to help building an actual render calculation stream

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

* *****************************************************/


#include "lib/error.hpp"
#include "proc/play/render-configurator.hpp"
#include "proc/play/output-manager.hpp"
#include "proc/engine/engine-service.hpp"
#include "proc/engine/engine-service-mock.hpp"
//#include "lib/itertools.hpp"

//#include <string>
//#include <memory>
#include <tr1/memory>
#include <tr1/functional>
//#include <boost/scoped_ptr.hpp>



namespace proc {
namespace play {
  
  namespace error = lumiera::error;
//    using std::string;
//    using lumiera::Subsys;
//    using std::auto_ptr;
//    using boost::scoped_ptr;
  using std::tr1::shared_ptr;
  using std::tr1::bind;
  using std::tr1::placeholders::_1;
  using engine::EngineService;
  using engine::EngineServiceMock;
  
  typedef EngineService::QoS_Definition RenderQuality;
  
  
 
  RenderConfigurator::~RenderConfigurator() { } // emit VTable here...
  
  
  
  /** Template Method: how to build an active render feed,
   *  pulling from the given exit point of the model and
   *  feeding the OutputSlot established appropriately
   *  to deliver media data of suitable type */
  Feed
  RenderConfigurator::buildActiveFeed (ModelPort port)
  {
    OutputSlot& slot = getOutputFor (port);
    return Feed (buildCalculationStreams (port,slot)); 
  }
  
  
  
  
  
  
  namespace { // Implementation details...
    
//    using std::tr1::bind;
//    using std::tr1::function;
//    using std::tr1::placeholders::_1;
//    using lib::transform;
    
    class LumieraRenderProcessBuilder
      : public RenderConfigurator
      {
        
        POutputManager outputResolver_;
        Timings        playbackTimings_;
        RenderQuality  renderQuality_;
        
        
        OutputSlot&
        getOutputFor (ModelPort port)
          {
            REQUIRE (outputResolver_);
            OutputSlot& slot = outputResolver_->getOutputFor (port);
            if (!slot.isFree())
              throw error::State("unable to acquire a suitable output slot"   /////////////////////TICKET #197 #816
                                , LUMIERA_ERROR_CANT_PLAY);
            return slot;
          }
        
        
        engine::CalcStreams
        buildCalculationStreams (ModelPort port, OutputSlot& output)
          {
            OutputSlot::Allocation& activeOutputConnection = output.allocate();
            Timings nominalTimings = activeOutputConnection.getTimingConstraints()
                                                           .constrainedBy(playbackTimings_);
            
            return activateEngine (port, nominalTimings, activeOutputConnection, renderQuality_);
          }
        
      protected:
        virtual engine::CalcStreams
        activateEngine (ModelPort port, Timings timings, OutputSlot::Allocation& activeOutputConnection, RenderQuality quality)
          {
            return EngineService::instance().calculate (port, timings, activeOutputConnection, quality);
          }
        
        
      public:
        LumieraRenderProcessBuilder (POutputManager outputManager, Timings playbackSpeed)
          : outputResolver_(outputManager)
          , playbackTimings_(playbackSpeed)
          , renderQuality_(EngineService::QoS_DEFAULT)
          { }
        
      };
    
    class MockRenderProcessBuilder
      : public LumieraRenderProcessBuilder
      {
        engine::CalcStreams
        activateEngine (ModelPort port, Timings timings, OutputSlot::Allocation& activeOutputConnection,RenderQuality quality)
          {
            return EngineServiceMock::instance().calculate (port, timings, activeOutputConnection, quality);
          }
        
      public:
        MockRenderProcessBuilder (POutputManager outputManager, Timings playbackSpeed)
          : LumieraRenderProcessBuilder(outputManager,playbackSpeed)
          { }
      };
    
    
    
    /** @internal decision point about how to configure the rendering */
    inline RenderConfigurator*
    how_to_render (POutputManager outputPossibilities, Timings playTimings)
    {
      if (playTimings.isMockEngineRun())
        return new MockRenderProcessBuilder (outputPossibilities, playTimings);
            
      else
        return new LumieraRenderProcessBuilder (outputPossibilities, playTimings);
    }
  
    
  } // (End) hidden service impl details
  
  
  
  
  
  /** @internal this builder function is used by the PlayService
   * when it comes to creating a new PlayProcess. The generated ConnectFunction
   * treats a single ModelPort to produce a suitable rendering setup, pulling data
   * from this port; it thus embodies the specific knowledge how to configure and
   * setup the rendering or playback at the EngineFacade, based on the playback
   * speed and quality desirable for this playback process to be initiated.
   * @remarks building a special subclass here and managing this instance
   *          by smart-ptr. Then wrapping all of this up into a functor,
   *          which can thus be passed on by value. This functor will
   *          later on be used to transform each desired model port
   *          into a suitable output connection, where the actual
   *          output will be resolved through the given
   *          OutputManager
   */
  RenderConfigurator::ConnectFunction
  buildRenderConfiguration (POutputManager outputPossibilities, Timings playTimings)
  {
    shared_ptr<RenderConfigurator> specialConfig (how_to_render (outputPossibilities,playTimings));
    
    return bind (&RenderConfigurator::buildActiveFeed, specialConfig, _1 );
  }
  
 
  
}} // namespace proc::play
