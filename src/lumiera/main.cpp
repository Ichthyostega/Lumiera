/*
  main.cpp  -  start the Lumiera Application

  Copyright (C)         Lumiera.org
    2007-2008,          Joel Holdsworth <joel@airwebreathe.org.uk>
                        Christian Thaeter <ct@pipapo.org>
                        Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#include "include/nobugcfg.h"
#include "include/error.hpp"
#include "lumiera/appstate.hpp"
#include "lumiera/option.hpp"

#include "backend/enginefacade.hpp"
#include "backend/netnodefacade.hpp"
#include "backend/scriptrunnerfacade.hpp"
#include "proc/facade.hpp"
#include "gui/guifacade.hpp"

using util::Cmdline;
using lumiera::Subsys;
using lumiera::AppState;
using lumiera::ON_GLOBAL_INIT;

namespace {
  Subsys& engine  = backend::EngineFacade::getDescriptor();
  Subsys& netNode = backend::NetNodeFacade::getDescriptor();
  Subsys& script  = backend::ScriptRunnerFacade::getDescriptor();
  Subsys& builder = proc::Facade::getBuilderDescriptor();
  Subsys& session = proc::Facade::getSessionDescriptor();
  Subsys& lumigui = gui::GuiFacade::getDescriptor();
}



int
main (int argc, const char* argv[])
{
  NOTICE (lumiera, "*** Lumiera NLE for Linux ***");
  
  AppState& application = AppState::instance();
  try
    {
      Cmdline args (argc,argv);
      lumiera::Option options (args);
      application.init (options);
      
      session.depends (builder);
      netNode.depends (session);
      netNode.depends (engine);
      lumigui.depends (session);
      lumigui.depends (engine);
      script.depends (session);
      script.depends (engine);
      
      application.maybeStart (session);
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
