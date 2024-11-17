/*
  PlayProcess  -  state frame for an ongoing play/render process

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


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


namespace steam {
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
        throw error::State{"creating a PlayProcess without any usable output connections"
                          , LERR_(CANT_PLAY)};
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

  
  
}} // namespace steam::play
