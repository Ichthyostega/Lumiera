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
#include "common/appstate.hpp"
#include "common/option.hpp"

#include "vault/enginefacade.hpp"
#include "vault/netnodefacade.hpp"
#include "vault/scriptrunnerfacade.hpp"
#include "include/dummy-player-facade.h"
#include "steam/facade.hpp"
#include "stage/guifacade.hpp"

using lib::Cmdline;
using lumiera::Subsys;
using lumiera::AppState;

namespace {
  Subsys& engine  = vault::EngineFacade::getDescriptor();
  Subsys& netNode = vault::NetNodeFacade::getDescriptor();
  Subsys& script  = vault::ScriptRunnerFacade::getDescriptor();
  Subsys& player  = lumiera::DummyPlayer::getDescriptor();        ///////TODO: just a dummy, until we're able to render
  Subsys& session = steam::Facade::getSessionDescriptor();
  Subsys& playOut = steam::Facade::getPlayOutDescriptor();
  Subsys& lumigui = stage::GuiFacade::getDescriptor();
}



int
main (int argc, const char* argv[])
{
  NOTICE (main, "*** Lumiera NLE for Linux ***");
  
  AppState& application = AppState::instance();
  try
    {
      Cmdline args (argc,argv);
      lumiera::Option options (args);
      application.init (options);
      
      netNode.depends (session);
      netNode.depends (engine);
//    playOut.depends (engine);
      playOut.depends (session);
      lumigui.depends (session);
//    lumigui.depends (engine);
      player.depends (playOut);    //////TODO dummy player, until we're able to render
      lumigui.depends (player);
      script.depends (session);
      script.depends (engine);
      
      application.maybeStart (playOut);
      application.maybeStart (netNode);
      application.maybeStart (lumigui);
      application.maybeStart (script);
      
      return application.maybeWait();
    }
  
  
  catch (lumiera::Error& problem)
    {
      return application.abort (problem);
    }
  catch (...)
    {
      return application.abort();
    }
}
