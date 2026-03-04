/*
  main.cpp  -  start the Lumiera Application

   Copyright (C)
     2007,            Joel Holdsworth <joel@airwebreathe.org.uk>
     2007,            Christian Thaeter <ct@pipapo.org>
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file main.cpp
 ** Lumiera application main function
 */


#include "include/logging.h"
#include "lib/error.hpp"
#include "vessel/voyage.hpp"
#include "vessel/option.hpp"

#include "vault/enginefacade.hpp"
#include "vault/netnodefacade.hpp"
#include "vault/scriptrunnerfacade.hpp"
#include "steam/facade.hpp"
#include "stage/guifacade.hpp"

using lib::Cmdline;
using vessel::Subsys;
using vessel::Voyage;

namespace {
  Subsys& engine  = vault::EngineFacade::getDescriptor();
  Subsys& netNode = vault::NetNodeFacade::getDescriptor();
  Subsys& script  = vault::ScriptRunnerFacade::getDescriptor();
  Subsys& session = steam::Facade::getSessionDescriptor();
  Subsys& playOut = steam::Facade::getPlayOutDescriptor();
  Subsys& lumigui = stage::GuiFacade::getDescriptor();
}



int
main (int argc, const char* argv[])
{
  NOTICE (main, "*** Lumiera NLE for Linux ***");
  
  Voyage& voyage = Voyage::access();
  try
    {
      Cmdline args (argc,argv);
      vessel::Option options (args);
      voyage.init (options);
      
      netNode.depends (session);
      netNode.depends (engine);
//    playOut.depends (engine);         ///////////////////////////////////////TICKET #1149 actually start an »Engine subsystem«
      playOut.depends (session);
      lumigui.depends (session);
//    lumigui.depends (engine);
      script.depends (session);
      script.depends (engine);
      
      voyage.maybeStart (playOut);
      voyage.maybeStart (netNode);
      voyage.maybeStart (lumigui);
      voyage.maybeStart (script);
      
      return voyage.maybeWait();
    }
  
  
  catch (lumiera::Error& problem)
    {
      return voyage.abort (problem);
    }
  catch (...)
    {
      return voyage.abort();
    }
}
