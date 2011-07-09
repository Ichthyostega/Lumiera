/*
  PlayProcess.hpp  -  state frame for an ongoing play/render process

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
  
  
  namespace { // Implementation details...
    
    using std::tr1::bind;
    using std::tr1::function;
    using std::tr1::placeholders::_1;
    using lib::transform;
    
    
    Feed
    resolveOutputConnection (ModelPort port, POutputManager outputResolver)
    {
      REQUIRE (outputResolver);
      OutputSlot& slot = outputResolver->getOutputFor (port);
      if (!slot.isFree())
        throw error::State("unable to acquire a suitable output slot"   /////////////////////TICKET #197 #816
                          , LUMIERA_ERROR_CANT_PLAY);
      return Feed (port,slot);
    }
    
    
    typedef function<Feed(ModelPort)> ConnectFunction;
    
    /** try to establish an output slot for the given 
     *  global bus or data production exit point.
     * @param outputResolver a facility able to resolve to
     *        a concrete output slot within the actual context 
     * @throw error::State when resolution fails 
     */
    ConnectFunction
    resolve (POutputManager outputResolver)
    {
      return bind (resolveOutputConnection, _1, outputResolver);
    }
    
  } // (End) hidden service impl details
  
  
  
  
  /**
   * Factory: Initialise and configure a new PlayProcess.
   * The caller gets to own and manage the returned process entry.
   */
  PlayProcess*
  PlayProcess::initiate (ModelPorts dataGenerators, POutputManager outputDestinations)
  {
    return new PlayProcess (transform (dataGenerators,
                            resolve(outputDestinations)));

  }
  
  
  
  /** @internal actually create and configure a play process instance */
  PlayProcess::PlayProcess (Feed::Connections pipeConnections)
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
  Feed::Feed (ModelPort port, OutputSlot& output)
  {
    UNIMPLEMENTED("build an active playback/render feed");
  }

  
  
}} // namespace proc::play