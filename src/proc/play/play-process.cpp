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


#include "proc/play/play-process.hpp"
#include "proc/play/play-service.hpp"
#include "proc/play/render-configurator.hpp"
#include "lib/itertools.hpp"

//#include <string>
//#include <memory>
#include <tr1/functional>
//#include <boost/scoped_ptr.hpp>



namespace proc {
namespace play {
  
//    using std::string;
//    using lumiera::Subsys;
//    using std::auto_ptr;
//    using boost::scoped_ptr;
//    using std::tr1::bind;
  using lib::transform;
  
  
  namespace { // Implementation details...
    
    
  } // (End) hidden service impl details
  
  
  
  
  
  /**
   * Factory: Initialise and configure a new PlayProcess.
   * The caller gets to own and manage the returned process entry.
   */
  PlayProcess*
  PlayProcess::initiate (ModelPorts dataGenerators, function<Feed(ModelPort)> activeOutputFeedBuilder)
  {
    return new PlayProcess (transform (dataGenerators,
                                       activeOutputFeedBuilder));
  }
  
  
  
  /** @internal actually create and configure a play process instance */
  PlayProcess::PlayProcess (Connections pipeConnections)
  {
    if (isnil (pipeConnections))
      throw error::State ("creating a PlayProcess without any usable output connections"
                         , LUMIERA_ERROR_CANT_PLAY);
    
    UNIMPLEMENTED ("iterate over the connections and allocate/establish each of them, creating and storing Feed objects");
    while (pipeConnections)
      {
        
      }
  }
  
  
  /** */
  Feed::Feed (engine::CalcStreams const& newActiveRenderingConnections)
    : renderStreams_(newActiveRenderingConnections)
    { }

  
  
}} // namespace proc::play
