/*
  BASIC-SETUP.hpp  -  elementary self-configuration of the application

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file basic-setup.hpp
 ** Bootstrap and self-configuration of the application at startup.
 ** The class BasicSetup represents the very basic self-configuration
 ** of the application at start-up. This bootstrap is based on some
 ** conventions for the application to "find its parts".
 ** 
 ** # Motivation: why a bootstrap convention?
 ** We want Lumiera to be usable _without the need of a set-up._
 ** Since the application is meant to be used in media production, users
 ** often have to work in temporary and varying environments. In such a
 ** situation, it is desirable just to "unzip and start" an application
 ** bundle. On the other hand, obviously we want to create a nice and
 ** proper, FSH compliant application, which can be packaged and installed
 ** in a modern Linux distribution as first class citizen. We can fulfil
 ** both demands by establishing a convention how the application can
 ** discover the very basic parts necessary to start up into the `main()`
 ** function and read a setup configuration. This setup configuration
 ** in turn supports the definition of _search paths_ -- which then
 ** allows us to deliver the application in a bundle structure which
 ** both can be run from a single root folder and likewise easily
 ** be mapped into FSH compliant installation folders by the
 ** packager.
 ** 
 ** @see lumiera::AppState::AppState()
 ** @see main.cpp
 ** 
 */


#ifndef COMMON_BASIC_SETUP_H
#define COMMON_BASIC_SETUP_H

#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <boost/program_options.hpp>
#include <string>


/** "bootstrapIni" : the basic setup configuration to load */
#define LUMIERA_LOCATION_OF_BOOTSTRAP_INI  "$ORIGIN/setup.ini" 



namespace lumiera {
  
  using std::string;
  
  namespace opt = boost::program_options;
  
  
  /**
   * Represents the elementary self-configuration
   * of a running Lumiera application instance.
   * This basic setup is tied to the location of the
   * Lumiera executable; from there the initial configuration
   * locates a \c setup.ini to read in the fundamental settings.
   * This is even prerequisite for loading any extension modules
   * or reading any extended application configuration; usually
   * this bootstrap process happens at or before the start of
   * the \c main() function. Any failure leads to immediate
   * termination of the application.
   * 
   * \par WIP 2011 -- preliminary configuration solution
   * The full-blown Configuration subsystem is just drafted
   * and way from being usable. Thus we'll use this basic
   * configuration as a replacement for the time being.
   * 
   * @see configfacade.hpp
   * @see AppState 
   */
  class BasicSetup
    : util::NonCopyable
    {
        opt::options_description syntax;
        opt::variables_map settings;
        
    public:
      BasicSetup (string bootstrapIni);
      
      string
      operator[] (lib::Literal key)  const
        {
          return get (key).as<string>();
        }
      
      opt::variable_value const&
      get (lib::Literal key)  const
        {
          string keyID (key);
          __ensure_hasKey(keyID);
          return settings[keyID];
        }
      
      
    private:
      void
      __ensure_hasKey (string const& key)  const
        {
          if (!util::contains (settings, key))
            throw error::Logic ("Key \""+key+"\" not found in setup.ini");
        }
    };
 
  
} // namespace lumiera
#endif
