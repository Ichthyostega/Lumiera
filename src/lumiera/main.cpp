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


#include <iostream>

#include "include/nobugcfg.h"
#include "include/error.hpp"
#include "include/lifecycle.h"
#include "lumiera/appstate.hpp"
#include "lumiera/option.hpp"

using lumiera::AppState;
using lumiera::ON_GLOBAL_INIT;
using lumiera::ON_GLOBAL_SHUTDOWN;


int
main (int argc, char* argv[])
{
  NOTICE (lumiera, "*** Lumiera NLE for Linux ***");
  
  util::Cmdline args (argc,argv);
  AppState& application = AppState::instance();
  application.decide (lumiera::Option (args));
  
  try
    {
      AppState::lifecycle (ON_GLOBAL_INIT);
      
      if (application.isLoadExistingSession())
          Session::current.load (application.getSessionFile());
      
      if (application.isServerNode())
          ServerNode::open (application.getNodeServices());
      
      if (application.isUseGUI())
          GuiFacade::start();
      
      if (application.isRunScript())
          ScriptEnv::run (application.getScriptFile());
      
      return 0;
    }
  
  catch (lumiera::Error& problem)
    {
      application.abort (problem);
    }
  catch (...)
    {
      application.abort();
    }
  
}



void
shutdown ()
{
  /////////////TODO: any sensible way how this function can be activated?
  
  AppState::lifecycle (ON_GLOBAL_SHUTDOWN);
  
}
