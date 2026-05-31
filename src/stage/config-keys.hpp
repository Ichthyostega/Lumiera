/*
  CONFIG-KEYS.hpp  -  magic keys to query the global application configuration

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file config-keys.hpp
 ** Hard wired key constants to access the global application configuration.
 ** 
 ** # configuration and resource search
 ** 
 ** The UI layer retrieves the necessary configuration values from vessel::spine::Config,
 ** the config facade in the application core. Currently as of 2/2011 these values are
 ** loaded from setup.ini, because the full-blown config system is not yet implemented.
 ** Amongst others, this configuration defines a _search path_ for icons and a
 ** separate search path for resources. These path specs may use the token `$ORIGIN`
 ** to refer to the installation directory of the currently executing program.
 ** This allows for a relocatable Lumiera installation bundle.
 ** 
 ** @see gtk-lumiera.cpp the plugin to pull up this GUI
 ** @see vessel::BasicSetup definition of the acceptable configuration values
 ** @see vessel::Voyage general Lumiera application main
 ** @see vessel::spine::Config
 ** 
 */

#ifndef STAGE_CONFIG_KEYS_H
#define STAGE_CONFIG_KEYS_H


#include "lib/symbol.hpp"
#include "vessel/spine/config-facade.h"



namespace stage {
  
  using lib::Literal;
  using vessel::spine::Config;
  
  extern Literal KEY_TITLE;
  extern Literal KEY_VERSION;
  extern Literal KEY_WEBSITE;
  extern Literal KEY_AUTHORS;
  extern Literal KEY_COPYRIGHT;
  
  extern Literal KEY_STYLESHEET;
  extern Literal KEY_UIRES_PATH;
  extern Literal KEY_ICON_PATH;
  
  
}// namespace stage
#endif /*STAGE_CONFIG_KEYS_H*/
