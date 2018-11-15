/*
  ENGINE-SERVICE-MOCK.hpp  -  dummy render engine implementation for test/development

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

*/

/** @file engine-service-mock.hpp
 ** Placeholder implementation of the render engine for test and diagnostics.
 ** This implementation can be used as a drop-in replacement of the real engine.
 ** Of course, it is lacking most of the functionality; it is just usable to detect
 ** and verify the actual engine setup and invocation that \em would happen.
 ** 
 ** @todo 1/2012 until the real render engine is usable, this mock implementation
 **       will stand-in, allowing us to develop the other parts of the play/render subsystem.
 ** 
 ** @see proc::engine::EngineService "the real thing"
 ** @see render-configurator.cpp (activating the mock or the real engine)
 ** @see DummyPlayConnection       
 ** @see EngineInterface_test
 ** @see CalcStream_test
 */


#ifndef STEAM_ENGINE_ENGINE_SERVICE_MOCK_H
#define STEAM_ENGINE_ENGINE_SERVICE_MOCK_H


#include "lib/error.hpp"
//#include "include/dummy-player-facade.h"
//#include "include/display-facade.h"
#include "steam/engine/calc-stream.hpp"
#include "steam/mobject/model-port.hpp"
#include "steam/play/timings.hpp"
#include "steam/play/output-slot.hpp"
//#include "common/instancehandle.hpp"
//#include "lib/singleton-ref.hpp"
#include "steam/engine/engine-service.hpp"
#include "lib/scoped-ptrvect.hpp"


namespace proc {
namespace node { class DummyTick; }  
  
namespace engine{

//    using std::string;
//    using lumiera::Subsys;
//    using lumiera::Display;
//    using lumiera::DummyPlayer;
  using mobject::ModelPort;
  using proc::play::Timings;
  
  typedef EngineService::Quality Quality;
  
  
  
  
  
  /************************************************************//**
   * Variant of the render engine, reconfigured for mock operation.
   * Especially, this setup leaves out most of the actual Lumiera
   * engine's implementation facilities. There is no scheduler
   * and no frame cache; rather we perform simple dependent
   * calculations which might block.
   */
  class EngineServiceMock
    : public EngineService
    {
      lib::ScopedPtrVect<node::DummyTick> processors_;
      
      
    public:
      
      EngineServiceMock();
      
      
    protected:
      virtual RenderEnvironmentClosure& configureCalculation (ModelPort,Timings,Quality);
    };
  
  
  
  
}} // namespace proc::engine
#endif
