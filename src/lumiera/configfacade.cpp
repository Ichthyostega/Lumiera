/*
  ConfigFacade  -  C++ convenience wrapper and startup of the config system
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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
 
* *****************************************************/


#include "include/nobugcfg.h"
#include "include/lifecycle.h"
#include "include/configfacade.hpp"

extern "C" { // TODO: can't include "lumiera/config.h" from C++ code, because it uses an identifier "new" 
  
  /** Initialise the configuration subsystem.
   *  Must be called exactly once prior to any use 
   *  @param path search path for config files.
   */
  int lumiera_config_init (const char* path);
  
  /** Destroys the configuration subsystem.
   *  Subsequent calls are no-op. */
  void
  lumiera_config_destroy ();

}

#ifndef LUMIERA_CONFIG_PATH
#error LUMIERA_CONFIG_PATH not defined
#endif



namespace lumiera {
  
  
  namespace {
    
    void 
    pull_up_ConfigSystem () 
    {
      TRACE (lumiera, "booting up config system");
      Config::instance();
    }
    
    LifecycleHook trigger__ (ON_BASIC_INIT, &pull_up_ConfigSystem);         
  }
  
  
  
  
  /** storage for the single system-wide config facade instance */
  Singleton<Config> Config::instance;
  
  
  Config::Config ()
  {
    lumiera_config_init (LUMIERA_CONFIG_PATH);
    INFO (config, "Config system ready.");
    
    TODO ("wire Config facade to config interface");
  }
  
  
  Config::~Config()
  {
    lumiera_config_destroy();
    TRACE (lumiera, "config system closed.");
  }
  
  
  
  const string
  Config::get (string const& key)
  {
    UNIMPLEMENTED ("config facade access to config value");
    return string("warwohlnix");
  }



} // namespace lumiera
