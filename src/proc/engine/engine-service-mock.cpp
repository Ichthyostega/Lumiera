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
#include "proc/engine/worker/dummy-tick.hpp"

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
  
  
  
  
  
  
  /**
   * Initialise a mock render engine.
   * This dummy implementation manages a collection of
   * "Processors", each running in a separate thread.
   */
  EngineServiceMock::EngineServiceMock()
    : processors_()
    { }
  
  
  
  /** special engine configuration for mock/testing operation.
   */
  RenderEnvironmentClosure&
  EngineServiceMock::configureCalculation ()
  {
    UNIMPLEMENTED ("represent *this as RenderEnvironmentClosure)");
    RenderEnvironmentClosure* todo_fake(0);  ////KABOOOM
    
    return *todo_fake; 
  }
  
  
  
}} // namespace proc::engine
