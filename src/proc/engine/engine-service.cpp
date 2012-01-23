/*
  EngineService  -  primary service access point for using the renderengine

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


#include "proc/engine/engine-service.hpp"

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
  
  
  
  /** storage for the EngineService interface object */
  lib::Singleton<EngineService> EngineService::instance;
  
  
  
  /** */  
  EngineService::EngineService()
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
  EngineService::calculate(ModelPort mPort,
                           Timings nominalTimings,
                           OutputConnection& output,
                           Quality serviceQuality)
  {
    UNIMPLEMENTED ("build a list of standard calculation streams");
  }
  
  
  
  /** */
  CalcStreams
  EngineService::calculateBackground(ModelPort mPort,
                                     Timings nominalTimings,
                                     Quality serviceQuality)
  {
    UNIMPLEMENTED ("build calculation streams for background rendering");
  }
  
  
  /** @internal build a representation of a single, ongoing calculation effort.
   * This "CalcStream" is tied to the actual engine implementation, but only
   * through an opaque link, representing this concrete engine as an
   * RenderEnvironmentClosure. This enables the created CalcStream to be
   * re-configured and adjusted while running.
   */
  CalcStream
  EngineService::activateCalculation (RenderEnvironmentClosure& engineCallback)
  {
    return CalcStream (engineCallback); 
  }
  
  
  /** @internal extension point
   * Install and activate a single, ongoing calculation effort.
   * Configure and prepare all the internal components, pre-allocate
   * resources and add entries to the registration tables to get this
   * render activity into running state
   * @return CalcStream representing this newly started rendering activity
   * @note variations and especially mock implementations of the render engine
   *       might choose to configure internal differently. As long as the 
   *       CalcStream and the embedded RenderEnvironmentClosure are consistent,
   *       such a specific configuration remains opaque for the user of the
   *       created render activity
   */
  RenderEnvironmentClosure&
  EngineService::configureCalculation ()
  {
    UNIMPLEMENTED ("represent *this as RenderEnvironmentClosure)");
    RenderEnvironmentClosure* todo_fake(0);  ////KABOOOM
    
    return *todo_fake; 
  }
  
  
  
  
  /* ===== Quality-of-Service ===== */
  
  
  EngineService::Quality::~Quality() { } // emit vtables here...
  
  enum CalcType {
    PLAYBACK,
    RENDER,
    BACKGROUND
  };
  
  
  class DefaultQoS
    : public EngineService::Quality
    {
      CalcType type_;
      
    public:
      DefaultQoS (CalcType type)
        : type_(type)
        { }
    };
  
  class PriorityQoS
    : public DefaultQoS
    {
    
    public:
      PriorityQoS ()
        : DefaultQoS(PLAYBACK)
        { }
    };
  
  class Compromise
    : public DefaultQoS
    {
      
    public:
      Compromise (CalcType type)
        : DefaultQoS(type)
        { }
    };
  
  
  EngineService::QoS_Definition  EngineService::QoS_DEFAULT         = QoS_Definition::build<DefaultQoS> (PLAYBACK);
  EngineService::QoS_Definition  EngineService::QoS_BACKGROUND      = QoS_Definition::build<DefaultQoS> (BACKGROUND);
  EngineService::QoS_Definition  EngineService::QoS_COMPROMISE      = QoS_Definition::build<Compromise> (PLAYBACK); 
  EngineService::QoS_Definition  EngineService::QoS_PERFECT_RESULT  = QoS_Definition::build<DefaultQoS> (RENDER);
  EngineService::QoS_Definition  EngineService::QoS_SYNC_PRIORITY   = QoS_Definition::build<PriorityQoS>();
  
  
  
}} // namespace proc::engine
