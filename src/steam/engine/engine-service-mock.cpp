/*
  EngineServiceMock  -  dummy render engine implementation for test/development

   Copyright (C)
     2012,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file engine-service-mock.cpp
 ** Implementation translation unit for a mock render engine for unit testing.
 ** @todo draft from 2012, and, while stalled, still relevant as of 2016
 */


#include "steam/engine/engine-service-mock.hpp"
#include "steam/engine/worker/dummy-tick.hpp"

//#include <string>
//#include <memory>
//#include <functional>



namespace steam  {
namespace engine{
  
//    using std::string;
//    using lumiera::Subsys;
//    using std::bind;
  
  
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
  RenderEnvironment&
  EngineServiceMock::configureCalculation (ModelPort,Timings,Quality)
  {
    UNIMPLEMENTED ("represent *this as RenderEnvironment Closure)");
    RenderEnvironment* todo_fake(0);  ////KABOOOM
    
    return *todo_fake; 
  }
  
  
  
}} // namespace steam::engine
