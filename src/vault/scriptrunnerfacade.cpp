/*
  ScriptRunnerFacade  -  access point for running a script within Lumiera application context

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file scriptrunnerfacade.cpp
 ** Subsystem descriptor and configuration for a controlling and operating
 ** Lumiera through bindings to a script language.
 ** 
 ** @todo this just documents a vague plan. We won't be able to implement
 **       anything of that kind for the foreseeable future as of 1/2017
 */


#include "vault/scriptrunnerfacade.hpp"
#include "lib/depend.hpp"

#include <string>


namespace vault {
  
  using std::string;
  using lumiera::Subsys;
  
  class ScriptRunnerSubsysDescriptor
    : public Subsys
    {
      operator string ()  const { return "Script runner"; }
      
      bool 
      shouldStart (lumiera::Option&)  override
        {
          TODO ("determine, if a script should be executed");
          return false;
        }
      
      bool
      start (lumiera::Option&, SigTerm termination)  override
        {
          UNIMPLEMENTED ("start the script as defined by the options and register script abort/exit hook");
          return false;
        }
      
      void
      triggerShutdown ()  noexcept override
        {
          UNIMPLEMENTED ("halt any running script");
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
    lib::Depend<ScriptRunnerSubsysDescriptor> theDescriptor;
  }
  
  
  
  
  /** @internal intended for use by main(). */
  Subsys&
  ScriptRunnerFacade::getDescriptor()
  {
    return theDescriptor();
  }



} // namespace vault
