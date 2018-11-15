/*
  PlayProcess  -  state frame for an ongoing play/render process

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


/** @file play-process.cpp
 ** Implementation to organise an ongoing playback/rendering process
 */


#include "steam/play/play-process.hpp"
#include "steam/play/play-service.hpp"
#include "steam/play/render-configurator.hpp"
#include "lib/itertools.hpp"

//#include <string>
//#include <memory>
//#include <functional>

using lib::iter_source::transform;
using lib::append_all;


namespace proc {
namespace play {
  
//    using std::string;
//    using lumiera::Subsys;
//    using std::bind;
  
  
  namespace { // Implementation details...
    
    
  } // (End) hidden service impl details
  
  
  PlayProcess::PlayProcess (OutputFeeds feeds)
    : outputFeeds_(feeds)
    {
      if (isnil (feeds))
        throw error::State ("creating a PlayProcess without any usable output connections"
                           , LUMIERA_ERROR_CANT_PLAY);
    }
  
  
  
  /**
   * Factory: Initialise and configure a new PlayProcess.
   * The caller gets to own and manage the returned process entry.
   */
  PlayProcess*
  PlayProcess::initiate (ModelPorts dataGenerators, FeedBuilder activeOutputFeedBuilder)
  { 
    OutputFeeds newFeeds;
    append_all (transform (dataGenerators, activeOutputFeedBuilder), newFeeds);
    return new PlayProcess (newFeeds);
/////////////////////////////////////////////////////////////////////////////////////////////TICKET #874 : use a pipeline builder to write it as follows:
//                      treat_all(dataGenerators)
//                        .apply (activeOutputFeedBuilder)
//                        .buildVector();
  }
  
  
  
  
  
  /** */
  Feed::Feed (engine::CalcStreams const& newActiveRenderingConnections)
    : renderStreams_(newActiveRenderingConnections)
    { }

  
  
}} // namespace proc::play
