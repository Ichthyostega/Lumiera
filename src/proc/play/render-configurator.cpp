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
//#include "lib/itertools.hpp"

//#include <string>
//#include <memory>
#include <tr1/functional>
//#include <boost/scoped_ptr.hpp>



namespace proc {
namespace play {
  
  namespace error = lumiera::error;
//    using std::string;
//    using lumiera::Subsys;
//    using std::auto_ptr;
//    using boost::scoped_ptr;
  using std::tr1::bind;
  using std::tr1::placeholders::_1;
  using engine::EngineService;
  
  typedef EngineService::QoS_Definition RenderQuality;
  
  
  
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
  
  
  RenderConfigurator::RenderConfigurator()
    : function<Feed(ModelPort)> (bind (&RenderConfigurator::buildActiveFeed, this, _1))
    { }
  
  
  
  
  namespace { // Implementation details...
    
//    using std::tr1::bind;
//    using std::tr1::function;
//    using std::tr1::placeholders::_1;
//    using lib::transform;
    
    class DefaultRenderProcessBuilder
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
            
            return EngineService::instance().calculate(port, nominalTimings, activeOutputConnection, renderQuality_);
          }
        
        
      public:
        DefaultRenderProcessBuilder(POutputManager outputManager, Timings playbackSpeed)
          : outputResolver_(outputManager)
          , playbackTimings_(playbackSpeed)
          , renderQuality_(EngineService::QoS_DEFAULT)
          { }
        
      };
  
    
  } // (End) hidden service impl details
  
  
  
  
  
  /** */
  
  
}} // namespace proc::play
