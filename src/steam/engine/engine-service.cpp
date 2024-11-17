/*
  EngineService  -  primary service access point for using the renderengine

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file engine-service.cpp
 ** Implementation parts related to the engine service abstraction
 ** @warning as of 4/2023 Render-Engine integration work is underway ////////////////////////////////////////TICKET #1233
 */


#include "steam/engine/engine-service.hpp"
#include "lib/itertools.hpp"

//#include <string>
//#include <memory>
#include <functional>



namespace steam  {
namespace engine{
  
//    using std::string;
//    using lumiera::Subsys;
  using std::function;
  using std::bind;
  using std::ref;
  using lib::iter_source::transform;
  using lib::append_all;

  
  namespace { // hidden local details of the service implementation....
    
  } // (End) hidden service impl details
  
  
  
  
  
  /** storage for the EngineService interface object */
  lib::Depend<EngineService> EngineService::instance;
  
  
  
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
  {                                        //////////////////////////////////////////////////TICKET #1301 : prepare proper Dispatcher here, including translation ModelPort -> portIDX
    RenderEnvironment& renderConfig = configureCalculation (mPort,nominalTimings,serviceQuality);
    function<CalcStream(play::DataSink)> triggerRenderStart = bind (activateCalculation, _1, ref(renderConfig));

    CalcStreams runningCalculations;
    append_all (transform (output.getOpenedSinks()
                          ,triggerRenderStart
                          )
               ,runningCalculations);
    return runningCalculations;
/////////////////////////////////////////////////////////////////////////////////////////////TICKET #874 : use a pipeline builder to write it as follows:
//                      treat_all(output.getOpenedSinks())
//                        .apply (activateCalculation, _1, renderConfig)
//                        .buildVector();
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
   * engine::RenderEnvironment closure. This enables the created CalcStream
   * to be re-configured and adjusted while running.
   */
  CalcStream
  EngineService::activateCalculation (play::DataSink sink, RenderEnvironment& engineCallback)
  {
    CalcStream calcStream(engineCallback);
    calcStream.sendToOutput (sink);      ////////////////////////////////////////////////////TICKET #1297 : need to re-think the association ModelPort ⟷ output sink
    return calcStream;
  }
  
  
  
  /** @internal extension point
   * Create the environment for rendering a connected and related set of output streams.
   * Configure and prepare all the internal components, pre-allocate resources and add
   * entries to the registration tables necessary to get the related render activities
   * into "running" state. The created setup will typically be used to generate all
   * the individual channel streams linked together for playback or rendering;
   * they all share the same media type and quality settings.
   * @return an abstracted representation of the specific setup for this render;
   *         from this point on, this RenderEnvironment closure will be the only way
   *         for client code to talk to "the engine". The actual instance of this
   *         closure is just a handle and can be copied; any CalcStream created
   *         off this closure will be linked to the same "environment" and be
   *         tracked and managed for resource usage automatically.
   * @note variations and especially mock implementations of the render engine
   *       might choose to configure internals differently. As long as the
   *       CalcStream and the embedded RenderEnvironment are consistent,
   *       such a specific configuration remains opaque for the user of
   *       the created render activity
   */
  RenderEnvironment&
  EngineService::configureCalculation (ModelPort mPort,
                                       Timings nominalTimings,
                                       Quality serviceQuality)
  {
    UNIMPLEMENTED ("Access and wire to the Scheduler-frontend. "
                   "Then access the Segmentation and invoke a builder function for a suitable dispatcher table. "
                   "Package all of this into a suitable RenderEnvironementClosure subclass.");
    RenderEnvironment* todo_fake(0);  ////KABOOOM
    
    return *todo_fake; 
  }
  
  
  
  /**
   * Switch the complete engine into diagnostics mode.
   * This activates additional logging and reporting facilities,
   * allowing to verify some specific operations within the engine
   * did indeed happen. Activating this mode incurs a performance hit.
   */
  void
  EngineService::activateTracing()
  {
    UNIMPLEMENTED ("tracing/diagnostics mode of the render engine");
  }
  
  void
  EngineService::disableTracing()
  {
    UNIMPLEMENTED ("tracing/diagnostics mode of the render engine");
    ///////////TODO ensure this is EX_FREE
  }
  
  
  
  
  
  /* ===== Quality-of-Service ===== */
  
  
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
  
  
  
}} // namespace steam::engine
