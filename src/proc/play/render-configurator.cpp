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


#include "proc/play/render-configurator.hpp"
#include "proc/engine/engine-service.hpp"
//#include "lib/itertools.hpp"

//#include <string>
//#include <memory>
//#include <tr1/functional>
//#include <boost/scoped_ptr.hpp>



namespace proc {
namespace play {
  
//    using std::string;
//    using lumiera::Subsys;
//    using std::auto_ptr;
//    using boost::scoped_ptr;
//    using std::tr1::bind;
  
  
  namespace { // Implementation details...
    
//    using std::tr1::bind;
//    using std::tr1::function;
//    using std::tr1::placeholders::_1;
//    using lib::transform;
    
    class DefaultRenderProcessBuilder
      : public RenderConfigurator
      {
        
        RenderStreams
        buildCalculationStreams (ModelPort port, OutputSlot& output)
          {
            UNIMPLEMENTED("build an active playback/render feed");
            
            ///TODO allocate the output slot
            ///TODO extract the individual channels
            ///TODO get the timings
            
            engine::EngineService::instance().calculate(port, nominalTimings, activeOutputConnection, serviceQuality);
          }
      };
  
    
  } // (End) hidden service impl details
  
  
  
  
  
  /** */
  
  
}} // namespace proc::play
