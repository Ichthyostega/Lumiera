/*
  RENDER-CONFIGURATOR.hpp  -  strategy to help building an actual render calculation stream

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

/** @file render-configurator.hpp
 ** Strategy to hold all the detail knowledge necessary to establish
 ** a running render CalculationStream. 
 ** 
 ** @see engine::EngineService
 ** @see engine::Feed
 ** @see engine::PlayService
 */


#ifndef STEAM_PLAY_RENDER_CONFIGURATOR_H
#define STEAM_PLAY_RENDER_CONFIGURATOR_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
//#include "include/dummy-player-facade.h"
//#include "include/display-facade.h"
//#include "common/instancehandle.hpp"
//#include "lib/singleton-ref.hpp"
#include "steam/mobject/model-port.hpp"
#include "steam/play/play-process.hpp"
#include "steam/engine/calc-stream.hpp"
#include "steam/play/output-slot.hpp"
#include "steam/play/output-manager.hpp"
//#include "lib/iter-source.hpp"
//#include "lib/util.hpp"

#include <functional>
#include <vector>
//#include <string>


namespace steam {
namespace play {

//    using std::string;
//    using lumiera::Subsys;
//    using lumiera::Display;
//    using lumiera::DummyPlayer;
//  using util::isnil;
  using std::function;
  using proc::mobject::ModelPort;
  
//  typedef proc::play::POutputManager POutputManager;
//  typedef lib::IterSource<ModelPort>::iterator ModelPorts;
  
  namespace error = lumiera::error;
  
  
  /** Strategy for configuring the render process */
  class RenderConfigurator
    : util::NonCopyable
    {
      
    public:
      virtual ~RenderConfigurator();  ///< this is an interface
      
      Feed buildActiveFeed (ModelPort);
      
      typedef function<Feed(ModelPort)> ConnectFunction;
      
    protected:
      /** retrieve a suitable output sink for the data
       *  to be produced at the given model exit point.
       *  While the port already defines the necessary StreamType,
       *  this strategy still has to decide what concrete output sink
       *  to use accordingly.
       */
      virtual OutputSlot& getOutputFor (ModelPort port)                             =0;
      
      
      /** build active rendering connections, thereby delivering each channel
       *  of the given model port into the corresponding output sink.
       *  This strategy will try to allocate the output slot for output (may fail).
       *  Moreover, a suitable combination of output timings and service quality
       *  will be picked
       * @return List of active CalcStream descriptors, created and registered 
       *         with the EngineFacade, one for each channel connection.
       * @note   when this strategy function returns, the corresponding
       *         render activities are already up and running.
       */
      virtual engine::CalcStreams buildCalculationStreams (ModelPort, OutputSlot&)  =0;
      
    };
  
  
  /** Factory function to build a RenderConfigurator
   *  specifically tailored for a PlayProcess, about to be started.
   * @param outputPossibilities an OutputManager instance describing
   *        the situation where output is about to be generated (e.g.
   *        a viewer in the GUI or a file to be rendered)
   * @param playbackTimings characterisation of the kind of play/render.
   *        Besides the required delivery interval, this might also define
   *        quality-of-service expectations.
   * @return the public access point to an RenderConfigurator,
   *         wrapped as generic function object 
   */
  RenderConfigurator::ConnectFunction
  buildRenderConfiguration (POutputManager outputPossibilities, Timings playbackTimings);
  
    
  
  
  
}} // namespace steam::play
#endif
