/*
  BASIC-SETUP.hpp  -  elementary self-configuration of the application

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file basic-setup.hpp
 ** TODO basic-setup.hpp
 */


#ifndef COMMON_BASIC_SETUP_H
#define COMMON_BASIC_SETUP_H

#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <boost/program_options.hpp>
#include <boost/noncopyable.hpp>
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
    : boost::noncopyable
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
