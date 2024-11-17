/*
  STypeManager  -  entry point for dealing with media stream types

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file stypemanager.cpp
 ** Implementation functions of the stream type system
 */


#include "steam/common.hpp"
#include "steam/control/stypemanager.hpp"
#include "steam/control/styperegistry.hpp"
#include "include/lifecycle.h"


namespace steam {
namespace control {
  
  using lib::Symbol;
  
  /* ======= stream type manager lifecycle ==========*/
  
  
  STypeManager::STypeManager()
    : reg_{}
  {
    reset();
  }
  
  STypeManager::~STypeManager()
  { }
  
  /** access the system-wide stream type manager instance.
   *  Implemented as singleton. */
  lib::Depend<STypeManager> STypeManager::instance;
  
  
  void 
  STypeManager::reset() 
  {
    reg_.reset(new Registry);
    lumiera::LifecycleHook::trigger (ON_STREAMTYPES_RESET);
  }
  
  /** \par
   *  LifecycleHook, on which all the basic setup and configuration
   *  providing the pristine state of the stream type system has to be registered.
   *  @note plugins providing additional streamtype configuration should register
   *        their basic setup functions using this hook, which can be done via
   *        the C interface functions 
   */
  const char* ON_STREAMTYPES_RESET ("ON_STREAMTYPES_RESET");
  
  
  
  
  
  /* ======= implementation of the public interface ========= */
  
  /** */
  StreamType const&
  STypeManager::getType (Symbol sTypeID) 
  {
    UNIMPLEMENTED ("get type just by symbolic ID (query defaults manager)");
  }
  
  
  /** */
  StreamType const&
  STypeManager::getType (StreamType::ID stID) 
  {
    UNIMPLEMENTED ("get type just by symbolic ID (query defaults manager)");
  }
  
  
  StreamType const& 
  STypeManager::getType (StreamType::Prototype const& protoType)
  {
    UNIMPLEMENTED ("build complete StreamType based on prototype; may include querying defaults manager");
  }
  
  
  StreamType const&
  STypeManager::getType (StreamType::ImplFacade const& implType)
  {
    UNIMPLEMENTED ("build complete StreamType round the given implementation type");
  }
  
  
  StreamType::ImplFacade const&
  STypeManager::getImpl (Symbol libID, StreamType::Prototype const& protoType)
  {
    UNIMPLEMENTED ("wire up implementation in compliance to a prototype and using a specific MediaImplLib. really tricky.... ");
  }
  
  
  StreamType::ImplFacade const&
  STypeManager::fetchImpl (StreamType::ImplFacade::TypeTag rawType)
  {
    UNIMPLEMENTED ("STypeManager basic functionality: wire up implementation facade (impl type) from given raw type of the library");
  }
  
}} // namespace steam::control


// ==== C interface for registering setup of basic stream type configuration =======

void 
lumiera_StreamType_registerInitFunction (void setupFun(void))
{
  lumiera::LifecycleHook (steam::control::ON_STREAMTYPES_RESET, setupFun);
}
