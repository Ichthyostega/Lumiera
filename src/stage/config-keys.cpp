/*
  ConfigKeys  -  magic keys to query the global application configuration

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file config-keys.hpp
 ** Definition of access keys for global UI configuration.
 ** 
 ** @see lumiera::AppState general Lumiera application main
 ** @see lumiera::Config
 ** 
 */

#include "stage/config-keys.hpp"

namespace stage {
  
  
  Literal KEY_TITLE      = "Lumiera.title";
  Literal KEY_VERSION    = "Lumiera.version";
  Literal KEY_WEBSITE    = "Lumiera.website";
  Literal KEY_AUTHORS    = "Lumiera.authors";
  Literal KEY_COPYRIGHT  = "Lumiera.copyright";
  
  Literal KEY_STYLESHEET = "Gui.stylesheet";
  Literal KEY_UIRES_PATH = "Gui.resourcepath";
  Literal KEY_ICON_PATH  = "Gui.iconpath";
  
  
  
}// namespace stage
