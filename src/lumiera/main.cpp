/*
  main.cpp  -  start the Lumiera Application

  Copyright (C)         Lumiera.org
    2007,2011,          Joel Holdsworth <joel@airwebreathe.org.uk>
                        Christian Thaeter <ct@pipapo.org>
                        Hermann Vosseler <Ichthyostega@web.de>

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


/** @file main.cpp
 ** Lumiera application main function
 */


#include "include/logging.h"
#include "lib/error.hpp"
#include "common/appstate.hpp"
#include "common/option.hpp"

#include "backend/enginefacade.hpp"
#include "backend/netnodefacade.hpp"
#include "backend/scriptrunnerfacade.hpp"
#include "include/dummy-player-facade.h"
#include "proc/facade.hpp"
#include "gui/guifacade.hpp"

using lib::Cmdline;
using lumiera::Subsys;
using lumiera::AppState;

namespace {
  Subsys& engine  = backend::EngineFacade::getDescriptor();
  Subsys& netNode = backend::NetNodeFacade::getDescriptor();
  Subsys& script  = backend::ScriptRunnerFacade::getDescriptor();
  Subsys& player  = lumiera::DummyPlayer::getDescriptor();        ///////TODO: just a dummy, until we're able to render
  Subsys& session = proc::Facade::getSessionDescriptor();
  Subsys& playOut = proc::Facade::getPlayOutDescriptor();
  Subsys& lumigui = gui::GuiFacade::getDescriptor();
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
