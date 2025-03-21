/*
  EngineFacade  -  access point for communicating with the render engine

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file enginefacade.cpp
 ** Subsystem descriptor and configuration for the render engine.
 ** This is the implementation part to manage render engine lifecycle
 ** as a Subsystem of the whole application.
 ** 
 ** @todo placeholder/draft as of 1/2017
 ** @warning as of 4/2023 Render-Engine integration work is underway ////////////////////////////////////////TICKET #1280
 ** @see main.cpp
 ** 
 */


#include "vault/enginefacade.hpp"
#include "lib/depend.hpp"

#include <string>


namespace vault {

  using std::string;
  using lumiera::Subsys;
  
  class EngineSubsysDescriptor
    : public Subsys
    {
      operator string ()  const { return "Engine"; }
      
      bool 
      shouldStart (lumiera::Option&)  override
        {
          TODO ("determine, if renderengine should be started");
          return false;
        }
      
      bool
      start (lumiera::Option&, Subsys::SigTerm termination)  override
        {
          UNIMPLEMENTED ("pull up renderengine and register shutdown hook");
          return false;
        }
      
      void
      triggerShutdown ()  noexcept override
        {
          UNIMPLEMENTED ("initiate halting the engine");
        }
      
      bool 
      checkRunningState ()  noexcept override
        {
          //Lock guard{*this};
          TODO ("implement detecting running state");
          return false;
        }
    };
  
  namespace {
    lib::Depend<EngineSubsysDescriptor> theDescriptor;
  }
  
  
  
  
  /** @internal intended for use by main(). */
  Subsys&
  EngineFacade::getDescriptor()
  {
    return theDescriptor();
  }



} // namespace vault
