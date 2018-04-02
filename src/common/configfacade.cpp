/*
  ConfigFacade  -  C++ convenience wrapper and startup of the config system

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/


/** @file configfacade.cpp
 ** Draft for a facade to access and retrieve configuration values
 ** This was created as part of a first draft towards an application
 ** wide configuration system. Later (around 2012) it became clear that
 ** we can not judge the requirements for such a system yet, so we deferred
 ** the topic altogether. Meanwhile, this facade is sporadically used to
 ** mark the necessity to retrieve some "parametrisation values".
 ** 
 ** A preliminary implementation is backed by a `setup.ini` file,
 ** located relative to the application binary.
 ** 
 ** @todo as of 2016, this seems not to be used much, if at all.
 **       The GTK-UI, which in itself is very preliminary, retrieves
 **       some values from configuration, most notably the name of
 **       the GTK stylesheet (`gtk-lumiera.css` is the default)
 */


#include "include/logging.h"
#include "include/lifecycle.h"
#include "include/config-facade.h"
#include "common/appstate.hpp"
#include "lib/searchpath.hpp"
#include "lib/util.hpp"

extern "C" {
  #include "common/config.h"
}


/** key to fetch the search path for extended configuration.
 *  Will corresponding value is defined in the basic setup.ini
 *  and will be fed to the (planned) full-blown config system
 *  after the basic application bootstrap was successful.
 */
#define KEY_CONFIG_PATH "Lumiera.configpath"

/** Similarly, this key is used to fetch the configured default
 *  plugin/module search path from the basic setup.ini
 *  This patch is used by the plugin-loader to discover
 *  lumiera plugins and extensions.
 */
#define KEY_PLUGIN_PATH "Lumiera.modulepath"




namespace lumiera {
  
  using util::cStr;
  using util::isnil;
  using lib::Literal;
  

  /** storage and setup for the single system-wide config facade instance */
  lib::Depend<Config> Config::instance;
  
  
  namespace {
    
    void
    pull_up_ConfigSystem ()
    {
      TRACE (common, "booting up config system");
      Config::instance();
    }
    
    LifecycleHook trigger__ (ON_BASIC_INIT, &pull_up_ConfigSystem);
  }
  
  
  
  
  Config::Config ()
  {
    string extendedConfigSearchPath = AppState::instance().fetchSetupValue (KEY_CONFIG_PATH);
    lumiera_config_init (cStr(extendedConfigSearchPath));
    TRACE (config, "Config system ready.");
  }
  
  
  Config::~Config()
  {
    lumiera_config_destroy();
    TRACE (config, "config system closed.");
  }
  
  
  
  /** @note because the full-blown Config system isn't implemented yet
   *        we retrieve the contents of setup.ini as a preliminary solution
   */
  string
  Config::get (lib::Literal key)
  {
    string value = AppState::instance().fetchSetupValue (key);
    if (isnil (value))
      throw error::Config ("Configuration value for key=\""+key+"\" is missing");
    
    return value;
  }



} // namespace lumiera


extern "C" { /* ==== implementation C interface for accessing setup.ini ======= */
  
  
  using std::string;
  using lumiera::Config;
  using lib::SearchPathSplitter;
  using util::isnil;
  using util::cStr;
  
  
  
  const char*
  lumiera_get_plugin_path_default ()
  {
    static string pathSpec;
    if (isnil (pathSpec))
      {
        pathSpec += "plugin.path="; // syntax expected by lumiera_config_setdefault
        
        // fetch plugin search path from setup.ini and expand any $ORIGIN token
        SearchPathSplitter pathElement(Config::get (KEY_PLUGIN_PATH));
        while (pathElement)
          pathSpec += pathElement.next() +":";
      }
    
    return cStr(pathSpec);
  }
}
