/*
  EngineCtxFacilities  -  Implementation of global render engine operational services

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file engine-ctx-facilities.hpp
 ** Implementation parts of render engine operational services.
 ** @warning as of 10/2024 a preliminary integration of Render-Engine underway ////////////////////////////////////////TICKET #1367
 */


#include "steam/engine/engine-ctx-facilities.hpp"
#include "steam/engine/tracking-heap-block-provider.hpp"

//#include <string>
//#include <memory>
//#include <functional>



namespace steam  {
namespace engine{
  
//    using std::string;
//    using lumiera::Subsys;
//  using std::function;
//  using std::bind;
  using std::make_unique;
//  using std::ref;

  
  namespace { // hidden local details of the service implementation....
    
  } // (End) hidden service impl details
  
  
  
  
  
  /** storage for the EngineService interface object */
  lib::Depend<EngineCtx> EngineCtx::access;
  
  
  
  EngineCtx::~EngineCtx() { }
  
  /** */
  EngineCtx::EngineCtx()
    : services_{make_unique<Facilities>()}
    , mem  {services_->getMemProvider()}
    , cache{services_->getCacheProvider()}
    { }
  
  
  /** */
  EngineCtx::Facilities::Facilities()
    : memProvider_{make_unique<TrackingHeapBlockProvider>()}         ////////////////////////////////////////TICKET #1367 : only suitable for first prototype
    , cacheProvider_{}
    { }
  
  
  
  /** core operation: blah blubb....
   */
  
  
  
}} // namespace steam::engine
