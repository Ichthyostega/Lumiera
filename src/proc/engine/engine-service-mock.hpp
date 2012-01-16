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


#ifndef PROC_ENGINE_ENGINE_SERVICE_MOCK_H
#define PROC_ENGINE_ENGINE_SERVICE_MOCK_H


#include "lib/error.hpp"
//#include "include/dummy-player-facade.h"
//#include "include/display-facade.h"
#include "proc/engine/calc-stream.hpp"
#include "proc/mobject/model-port.hpp"
#include "proc/play/timings.hpp"
#include "proc/play/output-slot.hpp"
//#include "common/instancehandle.hpp"
//#include "lib/singleton-ref.hpp"
#include "proc/engine/engine-service.hpp"
#include "lib/singleton.hpp"
//
#include <boost/noncopyable.hpp>
//#include <boost/scoped_ptr.hpp>
//#include <string>


namespace proc  {
namespace engine{

//    using std::string;
//    using lumiera::Subsys;
//    using lumiera::Display;
//    using lumiera::DummyPlayer;
  using mobject::ModelPort;
  using proc::play::Timings;
  
  typedef EngineService::Quality Quality;
  
  
  
  
  
  /******************************************************
   * A service to schedule series of calculations,
   * delivering the rendered data into an external output
   * sink in a timely fashion. Actually the CalculationStream
   * instances provided through this (facade) interface are
   * backed by jobs executed through the scheduler in the
   * backend layer. The implementation of this service
   * cares to create the right job entries in the correct
   * order and to enqueue these into the scheduler.
   */
  class EngineServiceMock
    : boost::noncopyable
    {
      
      
    public:
      /** access point to the Engine Interface Mock implementation. */
      static lib::Singleton<EngineServiceMock> instance;
      
      
      EngineServiceMock();
     ~EngineServiceMock() { }
      
      CalcStreams
      calculate(ModelPort mPort,
                Timings nominalTimings,
                OutputConnection& output,
                Quality serviceQuality = EngineService::QoS_DEFAULT);
      
      CalcStreams
      calculateBackground(ModelPort mPort,
                          Timings nominalTimings,
                          Quality serviceQuality = EngineService::QoS_BACKGROUND);
      
      
    protected:
    };
  
  
  
  
}} // namespace proc::engine
#endif
