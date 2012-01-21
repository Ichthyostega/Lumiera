/*
  EngineServiceMock  -  dummy render engine implementation for test/development

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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


#include "proc/engine/engine-service-mock.hpp"

//#include <string>
//#include <memory>
//#include <tr1/functional>
//#include <boost/scoped_ptr.hpp>



namespace proc  {
namespace engine{
  
//    using std::string;
//    using lumiera::Subsys;
//    using std::auto_ptr;
//    using boost::scoped_ptr;
//    using std::tr1::bind;
  
  
  namespace { // hidden local details of the service implementation....
    
  } // (End) hidden service impl details
  
  
  
  
  
  
  /** */  
  EngineServiceMock::EngineServiceMock()
    { }
  
  
  
  /** core operation: activate the Lumiera Render Engine.
   * Invoking this service effectively hooks up each channel
   * of the given model exit point to deliver into the corresponding
   * output sink on the given OutputConnection (which is assumed
   * to be already allocated for active use by this connection).
   * The generated calculation streams represent actively ongoing
   * calculations within the engine, started right away, according
   * to the given timing constraints and service quality.
   */
  CalcStreams
  EngineServiceMock::calculate(ModelPort mPort,
                               Timings nominalTimings,
                               OutputConnection& output,
                               Quality serviceQuality)
  {
    UNIMPLEMENTED ("MOCK IMPLEMENTATION: build a list of standard calculation streams");
  }
  
  
  
  /** */
  CalcStreams
  EngineServiceMock::calculateBackground(ModelPort mPort,
                                         Timings nominalTimings,
                                         Quality serviceQuality)
  {
    UNIMPLEMENTED ("MOCK IMPLEMENTATION: build calculation streams for background rendering");
  }
  
  
  
}} // namespace proc::engine
